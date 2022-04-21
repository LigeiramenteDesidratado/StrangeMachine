#include "util/bitmask.h"
#include "util/common.h"

#include "data/array.h"

#include "smController.h"
#include "smGLTFLoader.h"
#include "smInput.h"
#include "smMem.h"
#include "smRearrangeBones.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smSkinnedMesh.h"
#include "smTexture.h"
#include "smUniform.h"

typedef struct {

  skinned_mesh_s *meshes;
  struct clip_s **clips;
  struct skeleton_s *skeleton;

  mat4 *pose_palette;

  // TODO: materials?
  texture_s texture;

  struct controller_s *fade_controller;
  unsigned char current_clip, next_clip;

} skinned_model_s;

void next_animation(skinned_model_s *sample);
skinned_model_s *skinned_model_new(void) {
  skinned_model_s *skinned_model = SM_CALLOC(1, sizeof(skinned_model_s));

  SM_ASSERT(skinned_model != NULL);

  return skinned_model;
}

// void load_model(skinned_model_s *model, const char *filename);
//
// bool skinned_model_ctor(skinned_model_s *skinned_model, const char *gltf_path, const char *texture_path) {
//
//   SM_ASSERT(skinned_model != NULL);
//
//   load_model(skinned_model, gltf_path);
//
//   bone_map_s *opt_maps = rearrange_skeleton(skinned_model->skeleton);
//   for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->meshes); ++i) {
//     rearrange_mesh(&skinned_model->meshes[i], opt_maps);
//   }
//
//   for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->clips); ++i) {
//     rearrange_clip(skinned_model->clips[i], opt_maps);
//   }
//   hmfree(opt_maps);
//
//   skinned_model->texture = texture_new();
//   if (!texture_ctor(&skinned_model->texture, texture_path)) {
//     SM_LOG_ERROR("failed to construct texture");
//     return false;
//   }
//
//   struct controller_s *control = controller_new();
//   if (!controller_ctor(control, skinned_model->skeleton))
//     return false;
//   skinned_model->fade_controller = control;
//
//   controller_play(skinned_model->fade_controller, skinned_model->clips[0]);
//   controller_do(skinned_model->fade_controller, 0.0f);
//   pose_get_matrix_palette(controller_get_current_pose(skinned_model->fade_controller), &skinned_model->pose_palette);
//
//   skinned_model->next_clip = 0;
//   skinned_model->current_clip = 0;
//
//   return true;
// }

bool skinned_model_ctor2(skinned_model_s *skinned_model, const char *gltf_path, const char *texture_path) {

  SM_ASSERT(skinned_model != NULL);

  cgltf_data *data = gltf_loader_load_file(gltf_path);
  skinned_model->meshes = gltf_loader_load_meshes(data);
  skinned_model->skeleton = gltf_loader_load_skeleton(data);
  skinned_model->clips = gltf_loader_load_animation_clips(data);
  /* for (size_t i = 0; i < data->materials_count; i++) { */
  /* printf("%s\n", data->materials[i].name); */
  /* } */
  gltf_loader_free_data(data);

  bone_map_s *opt_maps = rearrange_skeleton(skinned_model->skeleton);
  for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->meshes); ++i) {
    rearrange_mesh(&skinned_model->meshes[i], opt_maps);
  }

  for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->clips); ++i) {
    rearrange_clip(skinned_model->clips[i], opt_maps);
  }
  hmfree(opt_maps);

  skinned_model->texture = texture_new();
  if (!texture_ctor(&skinned_model->texture, texture_path)) {
    SM_LOG_ERROR("failed to construct texture");
    return false;
  }

  struct controller_s *control = controller_new();
  if (!controller_ctor(control, skinned_model->skeleton))
    return false;
  skinned_model->fade_controller = control;

  controller_play(skinned_model->fade_controller, skinned_model->clips[0]);
  controller_do(skinned_model->fade_controller, 0.0f);
  pose_get_matrix_palette(controller_get_current_pose(skinned_model->fade_controller), &skinned_model->pose_palette);

  skinned_model->next_clip = 0;
  skinned_model->current_clip = 0;

  return true;
}

void skinned_model_dtor(skinned_model_s *skinned_model) {

  SM_ASSERT(skinned_model != NULL);

  controller_dtor(skinned_model->fade_controller);

  SM_ALIGNED_ARRAY_DTOR(skinned_model->pose_palette);

  skeleton_dtor(skinned_model->skeleton);

  for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->clips); ++i) {
    clip_dtor(skinned_model->clips[i]);
  }
  SM_ARRAY_DTOR(skinned_model->clips);

  for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->meshes); ++i) {
    skinned_mesh_dtor(&skinned_model->meshes[i]);
  }
  SM_ARRAY_DTOR(skinned_model->meshes);

  SM_FREE(skinned_model);
  skinned_model = NULL;
}

void skinned_model_do(skinned_model_s *skinned_model, float dt) {

  controller_do(skinned_model->fade_controller, dt);

  if (input_scan_key_lock(sm_key_n)) {
    next_animation(skinned_model);
  }
  if (skinned_model->current_clip != skinned_model->next_clip) {
    // sample->fade_timer = 3.0;
    skinned_model->current_clip = skinned_model->next_clip;

    controller_fade_to(skinned_model->fade_controller, skinned_model->clips[skinned_model->current_clip], 0.5f);
  }

  pose_get_matrix_palette(controller_get_current_pose(skinned_model->fade_controller), &skinned_model->pose_palette);

  mat4 **inverse_bind_pose = skeleton_get_inverse_bind_pose(skinned_model->skeleton);
  for (size_t i = 0; i < SM_ALIGNED_ARRAY_SIZE(skinned_model->pose_palette); ++i) {
    glm_mat4_mul(skinned_model->pose_palette[i], (*inverse_bind_pose)[i], skinned_model->pose_palette[i]);
  }
}

void skinned_model_draw(skinned_model_s *skinned_model) {
  SM_ASSERT(skinned_model != NULL);

  shader_bind(SHADERS[SKINNED_SHADER]);
  if (skinned_model->pose_palette) {
    GLuint loc = glGetUniformLocation(SHADERS[SKINNED_SHADER], "animated");
    uniform_set_array(loc, skinned_model->pose_palette, (int32_t)SM_ALIGNED_ARRAY_SIZE(skinned_model->pose_palette));
  }

  texture_set(&skinned_model->texture, glGetUniformLocation(SHADERS[SKINNED_SHADER], "tex0"), 0);

  uint8_t flags = 0;
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.position);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.tex_coord);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.normal);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.weight);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.joint);

  for (size_t i = 0; i < SM_ARRAY_SIZE(skinned_model->meshes); ++i) {
    skinned_mesh_bind(&skinned_model->meshes[i], flags);

    GLuint handle = skinned_model->meshes[i].index_buffer.ebo;
    size_t num_indices = skinned_model->meshes[i].index_buffer.count;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElements(GL_TRIANGLES, (int32_t)num_indices, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    skinned_mesh_unbind(&skinned_model->meshes[i], flags);
  }

  texture_unset(0);
  shader_unbind();
}

bool skinned_model_set_animation(skinned_model_s *skinned_model, const char *animation) {

  for (unsigned int i = 0; i < SM_ARRAY_SIZE(skinned_model->clips); ++i) {
    if (strcmp(clip_get_name(skinned_model->clips[i]), animation) == 0) {
      skinned_model->next_clip = i;
      return true;
    }
  }

  return false;
}

char **skinned_model_get_animations(skinned_model_s *skinned_model) {

  char **animations = NULL;
  size_t size = SM_ARRAY_SIZE(skinned_model->clips);
  SM_ARRAY_NEW(animations, size);

  for (unsigned int i = 0; i < size; ++i) {
    animations[i] = strdup(clip_get_name(skinned_model->clips[i]));
  }

  return animations;
}

void next_animation(skinned_model_s *sample) {
  sample->next_clip++;
  if (sample->next_clip >= SM_ARRAY_SIZE(sample->clips))
    sample->next_clip = 0;
}

// #include <assimp/cimport.h>
// #include <assimp/postprocess.h>
// #include <assimp/scene.h>
// #include <assimp/version.h>
//
// int get_node_index(const struct aiNode *node, const struct aiBone **all_nodes, uint32_t num_nodes) {
//   if (node == NULL) {
//     return -1;
//   }
//
//   for (size_t i = 0; i < num_nodes; ++i) {
//     if (strcmp(node->mName.data, all_nodes[i]->mNode->mName.data) == 0) {
//       return i;
//     }
//   }
//
//   return -1;
// }
//
// void load_mesh(skinned_mesh_s *skinned_mesh, const struct aiMesh *mesh) {
//
//   /* skinned_mesh_s skinned_mesh = skinned_mesh_new(); */
//   /* if (!skinned_mesh_ctor(&skinned_mesh)) { */
//   /* printf("failed to create skinned mesh\n"); */
//   /* return skinned_mesh; */
//   /* } */
//
//   if (mesh->mVertices != NULL && mesh->mNumVertices > 0) {
//     SM_ARRAY_SET_SIZE(skinned_mesh->vertex.positions, mesh->mNumVertices);
//     memcpy(skinned_mesh->vertex.positions, mesh->mVertices, mesh->mNumVertices * sizeof(float) * 3);
//   }
//
//   if (mesh->mNormals != NULL && mesh->mNumVertices > 0) {
//     SM_ARRAY_SET_SIZE(skinned_mesh->vertex.normals, mesh->mNumVertices);
//     memcpy(skinned_mesh->vertex.normals, mesh->mNormals, mesh->mNumVertices * sizeof(float) * 3);
//   }
//
//   if (mesh->mTextureCoords[0] != NULL && mesh->mNumVertices > 0) {
//     SM_ARRAY_SET_SIZE(skinned_mesh->vertex.tex_coords, mesh->mNumVertices);
//     for (uint32_t uv = 0; uv < mesh->mNumVertices; ++uv) {
//       skinned_mesh->vertex.tex_coords[uv].v[0] = mesh->mTextureCoords[0][uv].x;
//       skinned_mesh->vertex.tex_coords[uv].v[1] = mesh->mTextureCoords[0][uv].y;
//     }
//   } else {
//     SM_ARRAY_SET_SIZE(skinned_mesh->vertex.tex_coords, mesh->mNumVertices);
//     for (uint32_t uv = 0; uv < mesh->mNumVertices; ++uv) {
//       skinned_mesh->vertex.tex_coords[uv].v[0] = 1.0f;
//       skinned_mesh->vertex.tex_coords[uv].v[1] = 1.0f;
//     }
//   }
//
//   // load indices
//   if (mesh->mFaces != NULL && mesh->mNumFaces > 0) {
//     for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
//       struct aiFace face = mesh->mFaces[i];
//       for (uint32_t j = 0; j < face.mNumIndices; ++j) {
//         SM_ARRAY_PUSH(skinned_mesh->indices, face.mIndices[j]);
//       }
//     }
//   }
//
//   SM_ARRAY_SET_SIZE(skinned_mesh->weights, mesh->mNumVertices);
//   memset(skinned_mesh->weights, 0, sizeof(vec4) * mesh->mNumVertices);
//
//   SM_ARRAY_SET_SIZE(skinned_mesh->influences, mesh->mNumVertices);
//   memset(skinned_mesh->influences, -1, sizeof(ivec4) * mesh->mNumVertices);
//
//   for (uint32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
//     int boneID = -1;
//     boneID = bone_index;
//     SM_ASSERT(boneID != -1);
//     struct aiVertexWeight *weights = mesh->mBones[bone_index]->mWeights;
//     int num_weights = mesh->mBones[bone_index]->mNumWeights;
//     for (int weight_index = 0; weight_index < num_weights; ++weight_index) {
//       int vertex_id = weights[weight_index].mVertexId;
//       float weight = weights[weight_index].mWeight;
//
//       for (int vi = 0; vi < 4; ++vi) {
//         if (skinned_mesh->influences[vertex_id].v[vi] == -1) {
//           skinned_mesh->influences[vertex_id].v[vi] = boneID;
//           skinned_mesh->weights[vertex_id].v[vi] = weight;
//           break;
//         }
//       }
//     }
//   }
// }
//
// void load_skeleton(skinned_model_s *model, const struct aiMesh *mesh) {
//
//   if (model->skeleton != NULL) {
//     return;
//   }
//
//   pose_s rest_pose = pose_new();
//   pose_resize(&rest_pose, mesh->mNumBones);
//
//   pose_s bind_pose = pose_new();
//   pose_resize(&bind_pose, mesh->mNumBones);
//
//   // equivalent to gltf_loader_load_rest_pose
//   for (uint32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
//
//     struct aiBone *b = mesh->mBones[bone_index];
//
//     // Get the bone local transform matrix and convert it to transform_s
//     struct aiMatrix4x4 mat = b->mNode->mTransformation;
//     mat4 m = mat4_transpose(mat4_new(mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3, mat.b4, mat.c1, mat.c2,
//                                      mat.c3, mat.c4, mat.d1, mat.d2, mat.d3, mat.d4));
//     transform_s local_transform = transform_mat4_to_transform(m);
//
//     // Get the bone parent index
//     int parent_index = get_node_index(b->mNode->mParent, (const struct aiBone **)mesh->mBones, mesh->mNumBones);
//
//     pose_set_parent(&rest_pose, bone_index, parent_index);
//     pose_set_local_transform(&rest_pose, bone_index, local_transform);
//     pose_set_name(&rest_pose, bone_index, b->mName.data);
//   }
//
//   size_t num_bones = SM_ARRAY_SIZE(rest_pose.nodes);
//   transform_s *world_bind_pose = NULL;
//   SM_ARRAY_SET_SIZE(world_bind_pose, num_bones);
//
//   for (size_t i = 0; i < num_bones; ++i) {
//     world_bind_pose[i] = pose_get_global_transform(&rest_pose, i);
//   }
//
//   for (size_t i = 0; i < num_bones; ++i) {
//     struct aiBone *b = mesh->mBones[i];
//
//     struct aiMatrix4x4 ai_inverse_bind_pose = b->mOffsetMatrix;
//     mat4 inverse_bind_pose_mtraix = mat4_transpose(
//         mat4_new(ai_inverse_bind_pose.a1, ai_inverse_bind_pose.a2, ai_inverse_bind_pose.a3, ai_inverse_bind_pose.a4,
//                  ai_inverse_bind_pose.b1, ai_inverse_bind_pose.b2, ai_inverse_bind_pose.b3, ai_inverse_bind_pose.b4,
//                  ai_inverse_bind_pose.c1, ai_inverse_bind_pose.c2, ai_inverse_bind_pose.c3, ai_inverse_bind_pose.c4,
//                  ai_inverse_bind_pose.d1, ai_inverse_bind_pose.d2, ai_inverse_bind_pose.d3,
//                  ai_inverse_bind_pose.d4));
//
//     mat4 bind_pose_matrix = mat4_inverse(inverse_bind_pose_mtraix);
//     transform_s bind_pose_transform = transform_mat4_to_transform(bind_pose_matrix);
//
//     int32_t joint_index = get_node_index(b->mNode, (const struct aiBone **)mesh->mBones, num_bones);
//     world_bind_pose[joint_index] = bind_pose_transform;
//   }
//   pose_copy(&bind_pose, &rest_pose);
//
//   for (size_t i = 0; i < num_bones; ++i) {
//     transform_s current = world_bind_pose[i];
//     int32_t p = pose_get_parent(&bind_pose, i);
//
//     if (p >= 0) { // bring into parent space
//       transform_s parent = world_bind_pose[p];
//       current = transform_combine(transform_inverse(parent), current);
//     }
//
//     pose_set_local_transform(&bind_pose, i, current);
//   }
//   SM_ARRAY_DTOR(world_bind_pose);
//
//   const char **bone_names = NULL;
//   SM_ARRAY_NEW(bone_names, mesh->mNumBones);
//   SM_ARRAY_SET_SIZE(bone_names, mesh->mNumBones);
//
//   for (uint32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
//     struct aiBone *b = mesh->mBones[bone_index];
//     if (b->mName.length > 0) {
//       bone_names[bone_index] = strdup(b->mName.data);
//       printf("bone name: %s\n", bone_names[bone_index]);
//     } else {
//       bone_names[bone_index] = strdup("EMPTY NODE");
//       printf("WARNING: bone %d has no name\n", bone_index);
//     }
//   }
//   printf("assimp names: %lu\n", SM_ARRAY_SIZE(bone_names));
//
//   model->skeleton = skeleton_new();
//   if (!skeleton_ctor(model->skeleton, &rest_pose, &bind_pose, (const char **)bone_names))
//     printf("error building skeleton\n");
//
//   SM_ARRAY_DTOR(bone_names);
//
//   pose_dtor(&rest_pose);
//   pose_dtor(&bind_pose);
// }
//
// void process_nodes(skinned_model_s *model, struct aiNode *node, const struct aiScene *scene) {
//   // process each mesh located at the current node
//   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
//
//     struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
//     printf("mesh bones: %d\n", mesh->mNumBones);
//
//     // the node object only contains indices to index the actual objects in the scene.
//     // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
//     skinned_mesh_s skinned_mesh = skinned_mesh_new();
//     if (!skinned_mesh_ctor(&skinned_mesh)) {
//       printf("error building skinned mesh\n");
//     }
//
//     // print count of bones
//     printf("mesh bones: %d\n", mesh->mNumBones);
//
//     load_mesh(&skinned_mesh, mesh);
//     SM_ARRAY_PUSH(model->meshes, skinned_mesh);
//     load_skeleton(model, mesh);
//   }
//   // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
//   for (unsigned int i = 0; i < node->mNumChildren; i++) {
//     process_nodes(model, node->mChildren[i], scene);
//   }
// }
//
// void load_model(skinned_model_s *model, const char *filename) {
//
//   unsigned int major = aiGetVersionMajor();
//   unsigned int minor = aiGetVersionMinor();
//   printf("version: %d.%d\n", major, minor);
//   const struct aiScene *scene =
//       aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_PopulateArmatureData);
//   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//     printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
//     exit(1);
//   }
//   // has animations?
//   printf("animations: %d\n", scene->mNumAnimations);
//   // process ASSIMP's root node recursively
//   process_nodes(model, scene->mRootNode, scene);
//   uint32_t anim_count = scene->mNumAnimations;
//
//   SM_ARRAY_SET_SIZE(model->clips, anim_count);
//
//   for (uint32_t i = 0; i < anim_count; ++i) {
//     struct aiAnimation *anim = scene->mAnimations[i];
//     uint32_t num_channels = anim->mNumChannels;
//
//     model->clips[i] = clip_new();
//     if (!clip_ctor(model->clips[i], anim->mName.data))
//       exit(1);
//
//     struct clip_s *clip = model->clips[i];
//
//     clip_set_cap_tracks(clip, num_channels);
//     for (uint32_t j = 0; j < num_channels; ++j) {
//       struct aiNodeAnim *channel = anim->mChannels[j];
//       int32_t node_index = pose_get_index_by_name(skeleton_get_rest_pose(model->skeleton), channel->mNodeName.data);
//
//       interpolation_e interpolation = CONSTANT_INTERP;
//       if (channel->mPreState == aiAnimBehaviour_LINEAR)
//         interpolation = LINEAR_INTERP;
//       else if (channel->mPreState == aiAnimBehaviour_REPEAT)
//         interpolation = CUBIC_INTERP;
//
//       transform_track_s *track = clip_get_transform_track_from_joint(clip, node_index);
//       track_resize_frame(&track->position, channel->mNumPositionKeys);
//       track_resize_frame(&track->rotation, channel->mNumRotationKeys);
//       track_resize_frame(&track->scale, channel->mNumScalingKeys);
//
//       for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k) {
//         struct aiVectorKey *key = &channel->mPositionKeys[k];
//
//         track->position.interpolation = interpolation;
//         track_resize_frame_in(&track->position, 3, k);
//         track_resize_frame_out(&track->position, 3, k);
//         track_resize_frame_value(&track->position, 3, k);
//
//         frame_s *frame = &track->position.frames[k];
//         frame->t = key->mTime / 1000.0f;
//         frame->value[0] = key->mValue.x;
//         frame->value[1] = key->mValue.y;
//         frame->value[2] = key->mValue.z;
//
//         // TODO: fix this
//         frame->in[0] = 0.0f;
//         frame->in[1] = 0.0f;
//         frame->in[2] = 0.0f;
//
//         frame->out[0] = 0.0f;
//         frame->out[1] = 0.0f;
//         frame->out[2] = 0.0f;
//       }
//
//       for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k) {
//         struct aiQuatKey *key = &channel->mRotationKeys[k];
//
//         track->rotation.interpolation = interpolation;
//
//         track_resize_frame_in(&track->rotation, 4, k);
//         track_resize_frame_out(&track->rotation, 4, k);
//         track_resize_frame_value(&track->rotation, 4, k);
//
//         frame_s *frame = &track->rotation.frames[k];
//         frame->t = key->mTime / 1000.0f;
//         frame->value[0] = key->mValue.x;
//         frame->value[1] = key->mValue.y;
//         frame->value[2] = key->mValue.z;
//         frame->value[3] = key->mValue.w;
//
//         // TODO: fix this
//         frame->in[0] = 0.0f;
//         frame->in[1] = 0.0f;
//         frame->in[2] = 0.0f;
//         frame->in[3] = 0.0f;
//
//         frame->out[0] = 0.0f;
//         frame->out[1] = 0.0f;
//         frame->out[2] = 0.0f;
//         frame->out[3] = 0.0f;
//       }
//
//       for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k) {
//         struct aiVectorKey *key = &channel->mScalingKeys[k];
//
//         track->scale.interpolation = interpolation;
//         track_resize_frame_in(&track->scale, 3, k);
//         track_resize_frame_out(&track->scale, 3, k);
//         track_resize_frame_value(&track->scale, 3, k);
//
//         frame_s *frame = &track->scale.frames[k];
//         frame->t = key->mTime / 1000.0f;
//         frame->value[0] = key->mValue.x;
//         frame->value[1] = key->mValue.y;
//         frame->value[2] = key->mValue.z;
//
//         // TODO: fix this
//         frame->in[0] = 0.0f;
//         frame->in[1] = 0.0f;
//         frame->in[2] = 0.0f;
//
//         frame->out[0] = 0.0f;
//         frame->out[1] = 0.0f;
//         frame->out[2] = 0.0f;
//       }
//     }
//     clip_recalculate_duration(clip);
//   }
//
//   for (size_t i = 0; i < SM_ARRAY_SIZE(model->clips); ++i) {
//     for (size_t j = 0; j < clip_get_size(model->clips[i]); ++j) {
//       int32_t joint = clip_get_id_at_index(model->clips[i], j);
//
//       transform_track_s *ttrack = clip_get_transform_track_from_joint(model->clips[i], joint);
//
//       track_index_look_up_table(&ttrack->position);
//       track_index_look_up_table(&ttrack->rotation);
//       track_index_look_up_table(&ttrack->scale);
//     }
//   }
//
//   aiReleaseImport(scene);
// }
