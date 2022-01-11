#include "cgltf/cgltf.h"
#include "util/common.h"

#include "smClip.h"
#include "smPose.h"
#include "smSkeleton.h"
#include "smSkinnedMesh.h"
#include "smTrack.h"
#include "smTransformTrack.h"

// helpers
cgltf_data *gltf_loader_load_file(const char *path) {
  cgltf_options options;
  memset(&options, 0, sizeof(cgltf_options));

  cgltf_data *data = NULL;
  cgltf_result result = cgltf_parse_file(&options, path, &data);
  if (result != cgltf_result_success) {
    log_error("could not parse gltf file: %s", path);
    return NULL;
  }

  result = cgltf_load_buffers(&options, data, path);
  if (result != cgltf_result_success) {
    cgltf_free(data);
    log_error("could not load buffers: %s", path);
    return NULL;
  }

  result = cgltf_validate(data);
  if (result != cgltf_result_success) {
    cgltf_free(data);
    log_error("invalid gltf file: %s", path);
    return NULL;
  }

  return data;
}

transform_s __gltf_get_local_transform(cgltf_node *n) {

  transform_s result = transform_zero();
  if (n->has_matrix) {
    mat4 mat;
    memcpy(&mat.v, n->matrix, 16 * sizeof(float));
    result = transform_mat4_to_transform(mat);
  }

  if (n->has_translation)
    memcpy(&result.position, n->translation, 3 * sizeof(float));

  if (n->has_rotation)
    memcpy(&result.rotation, n->rotation, 4 * sizeof(float));

  if (n->has_scale)
    memcpy(&result.scale, n->scale, 3 * sizeof(float));

  return result;
}

// __gltf_get_node_index is a helper function to get the index of cgltf_node
// from an array. The GLTFNodeIndex function can perform a simple linear lookup
// by looping through all the nodes in a .gltf file and returning the index of
// the node that you are searching for. If the index is no found, return -1 to
// signal an invalid index
int __gltf_get_node_index(cgltf_node *target, cgltf_node *all_nodes, uint32_t num_nodes) {
  if (target == NULL) {
    log_debug("returning -1");
    return -1;
  }

  for (size_t i = 0; i < num_nodes; ++i) {
    if (target == &all_nodes[i]) {
      return (int)i;
    }
  }

  return -1;
}

// With these helper functions, loading the rest pose takes very little work.
// Loop through all the nodes in the current glTF file. For each node, assign
// the local transform to the pose that will be returned. You can find the
// parent of a node using the GetNodeIndex helper function, which returns -1 if
// a node has no parent:
void gltf_loader_load_rest_pose(cgltf_data *data, pose_s *pose) {

  uint32_t bone_count = data->nodes_count;

  pose_resize(pose, bone_count);
  // pose_resize(pose, bone_count);

  for (size_t i = 0; i < bone_count; ++i) {
    cgltf_node *node = &(data->nodes[i]);

    transform_s transform = __gltf_get_local_transform(&data->nodes[i]);
    pose_set_local_transform(pose, i, transform);

    int32_t parent = __gltf_get_node_index(node->parent, data->nodes, bone_count);
    pose_set_parent(pose, i, parent);
  }
}

char **__gltf_loader_load_joint_names(cgltf_data *data) {

  size_t bone_count = data->nodes_count;
  char **result = NULL; // malloc(bone_count * sizeof(char *));
  arrsetlen(result, bone_count);

  for (size_t i = 0; i < bone_count; ++i) {
    cgltf_node *node = &(data->nodes[i]);

    if (node->name == NULL)
      result[i] = strdup("EMPTY NODE");
    else
      result[i] = strdup(node->name);
  }

  return result;
}

void __gltf_track_from_channel(track_s *result, int32_t stride, const cgltf_animation_channel *channel) {

  cgltf_animation_sampler *sampler = channel->sampler;
  interpolation_e interpolation = CONSTANT_INTERP;
  if (sampler->interpolation == cgltf_interpolation_type_linear)
    interpolation = LINEAR_INTERP;
  else if (sampler->interpolation == cgltf_interpolation_type_cubic_spline)
    interpolation = CUBIC_INTERP;

  bool is_sampler_cubic = interpolation == CUBIC_INTERP;

  result->interpolation = interpolation;

  float *time = NULL;
  arrsetlen(time, sampler->input->count * 1);
  for (size_t i = 0; i < sampler->input->count; ++i) {
    cgltf_accessor_read_float(sampler->input, i, &time[i], 1);
  }

  float *val = NULL;
  arrsetlen(val, sampler->output->count * stride);
  for (size_t i = 0; i < sampler->output->count; ++i) {
    cgltf_accessor_read_float(sampler->output, i, &val[i * stride], stride);
  }

  size_t num_frames = sampler->input->count;
  size_t comp_count = arrlenu(val) / arrlenu(time);

  track_resize_frame(result, num_frames);

  for (size_t i = 0; i < num_frames; ++i) {
    size_t base_index = i * comp_count;
    frame_s *frame = &result->frames[i];

    track_resize_frame_in(result, stride, i);
    track_resize_frame_out(result, stride, i);
    track_resize_frame_value(result, stride, i);

    int32_t offset = 0;

    frame->t = time[i];

    for (int32_t comp = 0; comp < stride; ++comp) {
      frame->in[comp] = is_sampler_cubic ? val[base_index + offset++] : 0.0f;
    }

    for (int32_t comp = 0; comp < stride; ++comp) {
      frame->value[comp] = val[base_index + offset++];
    }

    for (int32_t comp = 0; comp < stride; ++comp) {
      frame->out[comp] = is_sampler_cubic ? val[base_index + offset++] : 0.0f;
    }
  }

  arrfree(time);
  arrfree(val);
}

// Loop through all the clips in the provided gltf_data. For every clip, set its
// name. Loop through all of the channels in the clip and find the index of the
// node that the current channel affects
struct clip_s **gltf_loader_load_animation_clips(cgltf_data *data) {

  size_t num_clips = data->animations_count;
  size_t num_nodes = data->nodes_count;

  struct clip_s **result = NULL;
  arrsetlen(result, num_clips);

  for (size_t i = 0; i < num_clips; ++i) {
    size_t num_channels = data->animations[i].channels_count;

    result[i] = clip_new();
    if (!clip_ctor(result[i], data->animations[i].name))
      exit(1);

    // clip_set_name(result[i], data->animations[i].name);

    clip_set_cap_tracks(result[i], num_channels);

    for (size_t j = 0; j < num_channels; ++j) {
      cgltf_animation_channel *channel = &data->animations[i].channels[j];
      cgltf_node *target = channel->target_node;
      int node_id = __gltf_get_node_index(target, data->nodes, num_nodes);

      if (channel->target_path == cgltf_animation_path_type_translation) {
        transform_track_s *track = clip_get_transform_track_from_joint(result[i], node_id);
        __gltf_track_from_channel(&track->position, 3, channel);

      } else if (channel->target_path == cgltf_animation_path_type_scale) {
        transform_track_s *track = clip_get_transform_track_from_joint(result[i], node_id);
        __gltf_track_from_channel(&track->scale, 3, channel);

      } else if (channel->target_path == cgltf_animation_path_type_rotation) {
        transform_track_s *track = clip_get_transform_track_from_joint(result[i], node_id);
        __gltf_track_from_channel(&track->rotation, 4, channel);
      }
    } // End channels loop

    clip_recalculate_duration(result[i]);
  } // End clips loop

  for (size_t i = 0; i < arrlenu(result); ++i) {
    for (size_t j = 0; j < clip_get_size(result[i]); ++j) {
      int32_t joint = clip_get_id_at_index(result[i], j);

      transform_track_s *ttrack = clip_get_transform_track_from_joint(result[i], joint);

      track_index_look_up_table(&ttrack->position);
      track_index_look_up_table(&ttrack->rotation);
      track_index_look_up_table(&ttrack->scale);
    }
  }

  return result;
}

void gltf_loader_free_data(cgltf_data *data) {
  assert(data != NULL);

  cgltf_free(data);
}

void gltf_loader_load_bind_pose(cgltf_data *data, pose_s *bind_pose) {

  pose_s rest_pose = pose_new();
  gltf_loader_load_rest_pose(data, &rest_pose);
  size_t num_bones = arrlenu(rest_pose.nodes);
  transform_s *world_bind_pose = NULL;
  arrsetlen(world_bind_pose, num_bones);

  for (size_t i = 0; i < num_bones; ++i) {
    world_bind_pose[i] = pose_get_global_transform(&rest_pose, i);
  }

  // Loop through every skinned mesh in the glTF file. Read the inverse_bind_
  // matrices accessor into a large vector of float values. The vector needs to
  // contain the contain numJoints * 16 elements since each matrix is a 4x4
  // matrix
  size_t num_skins = data->skins_count;
  size_t max_size = 0;
  for (size_t i = 0; i < num_skins; ++i) {

    if (data->skins[i].inverse_bind_matrices->count > max_size)
      max_size = data->skins[i].inverse_bind_matrices->count;
  }

  float *inverse_bind_accessor = NULL;
  arrsetlen(inverse_bind_accessor, max_size * 16);

  for (size_t i = 0; i < num_skins; ++i) {
    cgltf_skin *skin = &(data->skins[i]);
    // __gltf_get_scalar_values(inverse_bind_accessor, 16,
    // skin->inverse_bind_matrices);

    for (cgltf_size i = 0; i < skin->inverse_bind_matrices->count; ++i) {
      cgltf_accessor_read_float(skin->inverse_bind_matrices, i, &inverse_bind_accessor[i * 16], 16);
    }

    // For each joint in the skin, get the inverse bind matrix. Invert the
    // inverse bind pose matrix to get the bind pose matrix. Convert the bind
    // pose matrix into a transform.
    // Store this world space transform in the worldBindPose vector
    size_t num_joints = skin->joints_count;
    for (size_t j = 0; j < num_joints; ++j) {
      // read the inverse bind matrix of the joint
      float *matrix = &(inverse_bind_accessor[j * 16]);
      mat4 inverse_bind_matrix;
      memcpy(&inverse_bind_matrix, matrix, sizeof(float) * 16);
      // invert, convert to transform

      mat4 bind_matrix = mat4_inverse(inverse_bind_matrix);
      transform_s bind_transform = transform_mat4_to_transform(bind_matrix);

      // set that transform in the world_nind_pose
      cgltf_node *joint_node = skin->joints[j];
      int32_t joint_index = __gltf_get_node_index(joint_node, data->nodes, num_bones);
      world_bind_pose[joint_index] = bind_transform;

    } // end for each joints
  }   // end for each skin
  arrfree(inverse_bind_accessor);

  // convert the world bind pose to regular bind pose

  pose_copy(bind_pose, &rest_pose);
  pose_dtor(&rest_pose);

  for (size_t i = 0; i < num_bones; ++i) {
    transform_s current = world_bind_pose[i];
    int32_t p = pose_get_parent(bind_pose, i);

    if (p >= 0) { // bring into parent space
      transform_s parent = world_bind_pose[p];
      current = transform_combine(transform_inverse(parent), current);
    }

    pose_set_local_transform(bind_pose, i, current);
  }
  arrfree(world_bind_pose);
}

struct skeleton_s *gltf_loader_load_skeleton(cgltf_data *data) {

  // TODO: investigate: constructing and destructing bind_pose and rest_pose is
  // slowing down?
  pose_s rest_pose = pose_new();
  pose_s bind_pose = pose_new();
  gltf_loader_load_rest_pose(data, &rest_pose);
  gltf_loader_load_bind_pose(data, &bind_pose);

  char **names = __gltf_loader_load_joint_names(data);

  struct skeleton_s *skeleton = skeleton_new();
  if (!skeleton_ctor(skeleton, &rest_pose, &bind_pose, (const char **)names))
    log_warn("error building skeleton\n");

  pose_dtor(&rest_pose);
  pose_dtor(&bind_pose);

  for (size_t i = 0; i < arrlenu(names); ++i) {
    free(names[i]);
  }
  arrfree(names);

  return skeleton;
}

void __gltf_loader_mesh_from_attribute(skinned_mesh_s *mesh, cgltf_attribute *attribute, cgltf_skin *skin,
                                       cgltf_node *nodes, uint32_t node_count) {
  cgltf_attribute_type attr_type = attribute->type;
  cgltf_accessor *accessor = attribute->data;

  size_t component_count = 0;
  if (accessor->type == cgltf_type_vec2)
    component_count = 2;
  else if (accessor->type == cgltf_type_vec3)
    component_count = 3;
  else if (accessor->type == cgltf_type_vec4)
    component_count = 4;

  float *values = NULL;
  arrsetlen(values, accessor->count * component_count);
  for (cgltf_size i = 0; i < accessor->count; ++i) {
    cgltf_accessor_read_float(accessor, i, &values[i * component_count], component_count);
  }

  size_t accessor_count = accessor->count;

  /* vec3 **positions = skinned_mesh_get_position(mesh); */
  /* vec3 **normals = skinned_mesh_get_normal(mesh); */
  /* vec2 **tex_coords = skinned_mesh_get_tex_coord(mesh); */
  /* ivec4 **influences = skinned_mesh_get_influences(mesh); */
  /* vec4 **weights = skinned_mesh_get_weight(mesh); */

  for (size_t i = 0; i < accessor_count; ++i) {
    int32_t index = i * component_count;
    switch (attr_type) {
    case cgltf_attribute_type_position:
      if (i == 0)
        arrsetlen(mesh->vertex.positions, accessor_count);

      mesh->vertex.positions[i] = vec3_new(values[index + 0], values[index + 1], values[index + 2]);

      break;
    case cgltf_attribute_type_texcoord:
      if (i == 0)
        arrsetlen(mesh->vertex.tex_coords, accessor_count);

      mesh->vertex.tex_coords[i] = vec2_new(values[index + 0], (1.0f - values[index + 1]));
      break;
    case cgltf_attribute_type_weights:
      if (i == 0)
        arrsetlen(mesh->weights, accessor_count);

      mesh->weights[i] = vec4_new(values[index + 0], values[index + 1], values[index + 2], values[index + 3]);
      break;

    case cgltf_attribute_type_normal: {
      if (i == 0)
        arrsetlen(mesh->vertex.normals, accessor_count);

      vec3 norm = vec3_new(values[index + 0], values[index + 1], values[index + 2]);
      if (vec3_len_sq(norm) < EPSILON) {
        norm = vec3_new(0.0f, 1.0f, 0.0f);
      }
      mesh->vertex.normals[i] = norm;
    } break;
    case cgltf_attribute_type_joints: {
      if (i == 0)
        arrsetlen(mesh->influences, accessor_count);
      // These indices are skin relative. This function has no information about
      // the skin that is being parsed. Add +0.5f to round, since we can't read
      // integers
      ivec4 joints = ivec4_new((int)(values[index + 0] + 0.5f), (int)(values[index + 1] + 0.5f),
                               (int)(values[index + 2] + 0.5f), (int)(values[index + 3] + 0.5f));

      joints.x = __gltf_get_node_index(skin->joints[joints.x], nodes, node_count);
      joints.y = __gltf_get_node_index(skin->joints[joints.y], nodes, node_count);
      joints.z = __gltf_get_node_index(skin->joints[joints.z], nodes, node_count);
      joints.w = __gltf_get_node_index(skin->joints[joints.w], nodes, node_count);

      mesh->influences[i] = joints;

    } break;
    default:
      break;
    }
  }

  arrfree(values);
}

// Loop through all of the nodes
// in the glTF file. Only process nodes that have both a mesh and a skin; any
// other nodes should be skipped
skinned_mesh_s *gltf_loader_load_meshes(cgltf_data *data) {

  cgltf_node *nodes = data->nodes;
  size_t node_count = data->nodes_count;

  size_t msize = 0;
  for (size_t i = 0; i < node_count; ++i) {
    cgltf_node *node = &nodes[i];
    if (node->mesh == NULL || node->skin == NULL)
      continue;
    int32_t num_prime = node->mesh->primitives_count;
    for (int j = 0; j < num_prime; ++j) {
      msize++;
    }
  }

  skinned_mesh_s *meshes = NULL;
  arrsetcap(meshes, msize);
  for (size_t i = 0; i < node_count; ++i) {
    cgltf_node *node = &nodes[i];
    if (node->mesh == NULL || node->skin == NULL)
      continue;
    int32_t num_prime = node->mesh->primitives_count;
    for (int j = 0; j < num_prime; ++j) {
      skinned_mesh_s m = skinned_mesh_new();
      if (!skinned_mesh_ctor(&m))
        log_warn("failed to contruct mesh");

      /* arrput(meshes, m); */
      cgltf_primitive *primitive = &node->mesh->primitives[j];
      size_t ac = primitive->attributes_count;
      for (size_t k = 0; k < ac; ++k) {
        cgltf_attribute *attribute = &primitive->attributes[k];
        __gltf_loader_mesh_from_attribute(&m, attribute, node->skin, nodes, node_count);
      }

      // check whether the primitive contains indices. If it does, the index
      // buffer of the mesh needs to be filled out as well
      if (primitive->indices != 0) {
        size_t ic = primitive->indices->count;
        arrsetlen(m.indices, ic);
        for (uint32_t k = 0; k < ic; ++k) {
          m.indices[k] = cgltf_accessor_read_index(primitive->indices, k);
        }
      }

      skinned_mesh_update_gl_buffers(&m);
      arrput(meshes, m);
    }
  }

  return meshes;
}
