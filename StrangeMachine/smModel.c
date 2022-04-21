#include "util/bitmask.h"
#include "util/common.h"

#include "data/array.h"

#include "smMem.h"
#include "smMesh.h"
#include "smOBJLoader.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smTexture.h"
#include "smUniform.h"

#include "smClip.h"
#include "smController.h"
#include "smSkeleton.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

typedef struct {

  mesh_s *meshes;
  // TODO: materials?
  texture_s texture;
  transform_s transform;

} model_s;

typedef struct {

  struct clip_s **clips;
  struct skeleton_s *skeleton;

  mat4 *pose_palette;

  struct controller_s *fade_controller;
  unsigned char current_clip, next_clip;

} animation_data_s;

typedef struct {

  mesh2_s *meshes;
  transform_s transform;

  animation_data_s *animation;

} model2_s;

static bool string_suffix(const char *str, const char *suffix);
typedef void (*caller)(model2_s *model, const struct aiMesh *ai_mesh);
void process_node(model2_s *model, struct aiNode *node, const struct aiScene *scene, caller caller);
void load_animations(model2_s *model, const struct aiScene *scene);
void load_skeleton(model2_s *model, const struct aiMesh *ai_mesh);
void load_mesh(model2_s *model, const struct aiMesh *ai_mesh);
bool model_has_animation(model2_s *model);

model_s *model_new(void) {
  model_s *model = SM_CALLOC(1, sizeof(model_s));

  SM_ASSERT(model != NULL);

  return model;
}

model2_s *model2_new(void) {
  model2_s *model = SM_CALLOC(1, sizeof(model2_s));

  SM_ASSERT(model != NULL);

  return model;
}

bool model_ctor(model_s *model, const char *obj_path, const char *texture_path) {

  SM_ASSERT(model != NULL);
  SM_ASSERT(obj_path != NULL);
  SM_ASSERT(texture_path != NULL);

  if (string_suffix(obj_path, ".obj")) {
    if (!obj_loader_load(&model->meshes, obj_path)) {
      SM_LOG_ERROR("[%s] failed to load and parse object");
      return false;
    }
  } else {
    SM_LOG_ERROR("file format not supported");
    return false;
  }

  for (uint32_t i = 0; i < SM_ARRAY_SIZE(model->meshes); ++i) {
    if (!mesh_ctor(&model->meshes[i])) {
      SM_LOG_ERROR("failed to construct model");
      return false;
    }

    mesh_update_gl_buffers(&model->meshes[i]);
  }

  model->transform = transform_zero();

  model->texture = texture_new();
  if (!texture_ctor(&model->texture, texture_path)) {
    SM_LOG_ERROR("failed to construct texture");
    return false;
  }

  return true;
}

bool model2_ctor(model2_s *model, const char *filename) {

  SM_ASSERT(model != NULL);
  SM_ASSERT(filename != NULL);

  const struct aiScene *scene =
      aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_PopulateArmatureData);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    SM_LOG_ERROR("ASSIMP error: %s\n", aiGetErrorString());
    exit(1);
  }

  process_node(model, scene->mRootNode, scene, load_mesh);

  if (scene->mNumAnimations > 0) {
    model->animation = SM_CALLOC(1, sizeof(animation_data_s));
    process_node(model, scene->mRootNode, scene, load_skeleton);
    load_animations(model, scene);
  }
  aiReleaseImport(scene);

  /* mesh2_update_gl_buffers(&model->meshes[i]); */

  model->transform = transform_zero();
  return true;
}

void model_dtor(model_s *model) {
  SM_ASSERT(model != NULL);

  for (size_t i = 0; i < SM_ARRAY_SIZE(model->meshes); ++i) {
    mesh_dtor(&model->meshes[i]);
  }
  SM_ARRAY_DTOR(model->meshes);

  texture_dtor(&model->texture);

  SM_FREE(model);
  model = NULL;
}

void model2_dtor(model2_s *model) {

  SM_ASSERT(model != NULL);

  if (model->animation != NULL) {

    controller_dtor(model->animation->fade_controller);
    SM_ARRAY_DTOR(model->animation->pose_palette);
    skeleton_dtor(model->animation->skeleton);

    for (size_t i = 0; i < SM_ARRAY_SIZE(model->animation->clips); ++i) {
      clip_dtor(model->animation->clips[i]);
    }
    SM_ARRAY_DTOR(model->animation->clips);
    SM_FREE(model->animation);
  }

  for (size_t i = 0; i < SM_ARRAY_SIZE(model->meshes); ++i) {
    /* mesh_dtor(&model->meshes[i]); */
  }
  SM_ARRAY_DTOR(model->meshes);

  SM_FREE(model);
  model = NULL;
}

void model_do(model2_s *model, float dt) {

  SM_ASSERT(model != NULL);

  if (1) {
    return;
  }

  if (model->animation) {
    controller_do(model->animation->fade_controller, dt);

    if (model->animation->current_clip != model->animation->next_clip) {
      // sample->fade_timer = 3.0;
      model->animation->current_clip = model->animation->next_clip;

      controller_fade_to(model->animation->fade_controller, model->animation->clips[model->animation->current_clip],
                         0.5f);
    }

    pose_get_matrix_palette(controller_get_current_pose(model->animation->fade_controller),
                            &model->animation->pose_palette);

    mat4 **inverse_bind_pose = skeleton_get_inverse_bind_pose(model->animation->skeleton);
    for (size_t i = 0; i < SM_ARRAY_SIZE(model->animation->pose_palette); ++i) {
      /* model->animation->pose_palette[i]; */
      glm_mat4_mul(model->animation->pose_palette[i], (*inverse_bind_pose)[i], model->animation->pose_palette[i]);
    }
  }
}

void model_draw(const model_s *const model) {

  SM_ASSERT(model != NULL);

  shader_bind(SHADERS[STATIC_SHADER]);

  mat4 md;
  transform_to_mat4(model->transform, md);
  uniform_set_value(glGetUniformLocation(SHADERS[STATIC_SHADER], "model"), md);

  texture_set(&model->texture, glGetUniformLocation(SHADERS[STATIC_SHADER], "tex0"), 0);

  uint8_t flags = 0;
  MASK_SET(flags, 1 << mesh_attr_locs.position);
  MASK_SET(flags, 1 << mesh_attr_locs.tex_coord);
  MASK_SET(flags, 1 << mesh_attr_locs.normal);

  for (size_t i = 0; i < SM_ARRAY_SIZE(model->meshes); ++i) {
    mesh_bind(&model->meshes[i], flags);
    /* mesh_draw(model->meshes[i]); */
    glDrawArrays(GL_TRIANGLES, 0, SM_ARRAY_SIZE(model->meshes[i].vertex.positions));
    mesh_unbind(&model->meshes[i], flags);
  }
  texture_unset(0);

  shader_unbind();

  /* for (size_t i = 0; i < SM_ARRAY_SIZE(model->meshes); ++i) { */
  /* mesh_draw_debug(&model->meshes[i]); */
  /* }; */
}

static bool string_suffix(const char *str, const char *suffix) {
  if (!str || !suffix)
    return false;

  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);

  if (lensuffix > lenstr)
    return false;

  if (strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0)
    return true;

  return false;
}

mesh_s **model_get_meshes(model_s *model) {
  return &model->meshes;
}

int get_node_index(const struct aiNode *node, const struct aiBone **all_nodes, uint32_t num_nodes) {
  if (node == NULL) {
    return -1;
  }

  for (size_t i = 0; i < num_nodes; ++i) {
    if (strcmp(node->mName.data, all_nodes[i]->mNode->mName.data) == 0) {
      return i;
    }
  }

  return -1;
}

void load_mesh(model2_s *model, const struct aiMesh *ai_mesh) {

  mesh2_s mesh = (mesh2_s){0};
  /* if (!mesh_ctor(&mesh)) { */
  /* printf("error building mesh\n"); */
  /* } */

  if (ai_mesh->mVertices != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_SIZE(mesh.positions, ai_mesh->mNumVertices);
    memcpy(mesh.positions, ai_mesh->mVertices, ai_mesh->mNumVertices * sizeof(float) * 3);
  }

  if (ai_mesh->mNormals != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_SIZE(mesh.normals, ai_mesh->mNumVertices);
    memcpy(mesh.normals, ai_mesh->mNormals, ai_mesh->mNumVertices * sizeof(float) * 3);
  }

  if (ai_mesh->mTextureCoords[0] != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_SIZE(mesh.uvs, ai_mesh->mNumVertices);
    for (uint32_t uv = 0; uv < ai_mesh->mNumVertices; ++uv) {
      mesh.uvs[uv][0] = ai_mesh->mTextureCoords[0][uv].x;
      mesh.uvs[uv][1] = ai_mesh->mTextureCoords[0][uv].y;
    }
  } else {
    SM_ARRAY_SET_SIZE(mesh.uvs, ai_mesh->mNumVertices);
    for (uint32_t uv = 0; uv < ai_mesh->mNumVertices; ++uv) {
      mesh.uvs[uv][0] = 1.0f;
      mesh.uvs[uv][1] = 1.0f;
    }
  }

  // load indices
  if (ai_mesh->mFaces != NULL && ai_mesh->mNumFaces > 0) {
    for (uint32_t i = 0; i < ai_mesh->mNumFaces; ++i) {
      struct aiFace face = ai_mesh->mFaces[i];
      for (uint32_t j = 0; j < face.mNumIndices; ++j) {
        SM_ARRAY_PUSH(mesh.indices, face.mIndices[j]);
      }
    }
  }

  if (ai_mesh->mNumBones > 0) {

    SM_ARRAY_SET_SIZE(mesh.weights, ai_mesh->mNumVertices);
    memset(mesh.weights, 0, sizeof(vec4) * ai_mesh->mNumVertices);

    SM_ARRAY_SET_SIZE(mesh.influences, ai_mesh->mNumVertices);
    memset(mesh.influences, -1, sizeof(ivec4) * ai_mesh->mNumVertices);

    for (uint32_t bone_index = 0; bone_index < ai_mesh->mNumBones; ++bone_index) {
      int boneID = -1;
      boneID = bone_index;
      SM_ASSERT(boneID != -1);
      struct aiVertexWeight *weights = ai_mesh->mBones[bone_index]->mWeights;
      int num_weights = ai_mesh->mBones[bone_index]->mNumWeights;
      for (int weight_index = 0; weight_index < num_weights; ++weight_index) {
        int vertex_id = weights[weight_index].mVertexId;
        float weight = weights[weight_index].mWeight;

        for (int vi = 0; vi < 4; ++vi) {
          if (mesh.influences[vertex_id][vi] == -1) {
            mesh.influences[vertex_id][vi] = boneID;
            mesh.weights[vertex_id][vi] = weight;
            break;
          }
        }
      }
    }
  }

  SM_ARRAY_PUSH(model->meshes, mesh);
}

void load_skeleton(model2_s *model, const struct aiMesh *ai_mesh) {

  static unsigned int n_b = 0;
  if (model->animation != NULL) {
    if (n_b == 0) {
      n_b = ai_mesh->mNumBones;
    } else {
      SM_ASSERT(n_b == ai_mesh->mNumBones);
    }
    return;
  }

  pose_s rest_pose = pose_new();
  pose_resize(&rest_pose, ai_mesh->mNumBones);

  pose_s bind_pose = pose_new();
  pose_resize(&bind_pose, ai_mesh->mNumBones);

  // equivalent to gltf_loader_load_rest_pose
  for (uint32_t bone_index = 0; bone_index < ai_mesh->mNumBones; ++bone_index) {

    struct aiBone *b = ai_mesh->mBones[bone_index];

    // Get the bone local transform matrix and convert it to transform_s
    struct aiMatrix4x4 mat = b->mNode->mTransformation;
    mat4 m;
    m[0][0] = mat.a1;
    m[0][1] = mat.a2;
    m[0][2] = mat.a3;
    m[0][3] = mat.a4;
    m[1][0] = mat.b1;
    m[1][1] = mat.b2;
    m[1][2] = mat.b3;
    m[1][3] = mat.b4;
    m[2][0] = mat.c1;
    m[2][1] = mat.c2;
    m[2][2] = mat.c3;
    m[2][3] = mat.c4;
    m[3][0] = mat.d1;
    m[3][1] = mat.d2;
    m[3][2] = mat.d3;
    m[3][3] = mat.d4;
    /* glm_mat4_copy(mat4_new(mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3, mat.b4, mat.c1, mat.c2, mat.c3, */
    /* mat.c4, mat.d1, mat.d2, mat.d3, mat.d4), */
    /* m); */
    transform_s local_transform = transform_mat4_to_transform(m);

    // Get the bone parent index
    int parent_index = get_node_index(b->mNode->mParent, (const struct aiBone **)ai_mesh->mBones, ai_mesh->mNumBones);

    pose_set_parent(&rest_pose, bone_index, parent_index);
    pose_set_local_transform(&rest_pose, bone_index, local_transform);
    pose_set_name(&rest_pose, bone_index, b->mName.data);
  }

  size_t num_bones = SM_ARRAY_SIZE(rest_pose.nodes);
  transform_s *world_bind_pose = NULL;
  SM_ARRAY_SET_SIZE(world_bind_pose, num_bones);

  for (size_t i = 0; i < num_bones; ++i) {
    world_bind_pose[i] = pose_get_global_transform(&rest_pose, i);
  }

  for (size_t i = 0; i < num_bones; ++i) {
    struct aiBone *b = ai_mesh->mBones[i];

    struct aiMatrix4x4 ai_inverse_bind_pose = b->mOffsetMatrix;
    mat4 inverse_bind_pose_mtraix;

    inverse_bind_pose_mtraix[0][0] = ai_inverse_bind_pose.a1;
    inverse_bind_pose_mtraix[0][1] = ai_inverse_bind_pose.a2;
    inverse_bind_pose_mtraix[0][2] = ai_inverse_bind_pose.a3;
    inverse_bind_pose_mtraix[0][3] = ai_inverse_bind_pose.a4;
    inverse_bind_pose_mtraix[1][0] = ai_inverse_bind_pose.b1;
    inverse_bind_pose_mtraix[1][1] = ai_inverse_bind_pose.b2;
    inverse_bind_pose_mtraix[1][2] = ai_inverse_bind_pose.b3;
    inverse_bind_pose_mtraix[1][3] = ai_inverse_bind_pose.b4;
    inverse_bind_pose_mtraix[2][0] = ai_inverse_bind_pose.c1;
    inverse_bind_pose_mtraix[2][1] = ai_inverse_bind_pose.c2;
    inverse_bind_pose_mtraix[2][2] = ai_inverse_bind_pose.c3;
    inverse_bind_pose_mtraix[2][3] = ai_inverse_bind_pose.c4;
    inverse_bind_pose_mtraix[3][0] = ai_inverse_bind_pose.d1;
    inverse_bind_pose_mtraix[3][1] = ai_inverse_bind_pose.d2;
    inverse_bind_pose_mtraix[3][2] = ai_inverse_bind_pose.d3;
    inverse_bind_pose_mtraix[3][3] = ai_inverse_bind_pose.d4;
    glm_mat4_transpose(inverse_bind_pose_mtraix);

    /* mat4_transpose( */
    /* mat4_new(ai_inverse_bind_pose.a1, ai_inverse_bind_pose.a2, ai_inverse_bind_pose.a3, ai_inverse_bind_pose.a4, */
    /* ai_inverse_bind_pose.b1, ai_inverse_bind_pose.b2, ai_inverse_bind_pose.b3, ai_inverse_bind_pose.b4, */
    /* ai_inverse_bind_pose.c1, ai_inverse_bind_pose.c2, ai_inverse_bind_pose.c3, ai_inverse_bind_pose.c4, */
    /* ai_inverse_bind_pose.d1, ai_inverse_bind_pose.d2, ai_inverse_bind_pose.d3, ai_inverse_bind_pose.d4)); */

    mat4 bind_pose_matrix;
    glm_mat4_inv(inverse_bind_pose_mtraix, bind_pose_matrix);
    transform_s bind_pose_transform = transform_mat4_to_transform(bind_pose_matrix);

    int32_t joint_index = get_node_index(b->mNode, (const struct aiBone **)ai_mesh->mBones, num_bones);
    world_bind_pose[joint_index] = bind_pose_transform;
  }
  pose_copy(&bind_pose, &rest_pose);

  for (size_t i = 0; i < num_bones; ++i) {
    transform_s current = world_bind_pose[i];
    int32_t p = pose_get_parent(&bind_pose, i);

    if (p >= 0) { // bring into parent space
      transform_s parent = world_bind_pose[p];
      current = transform_combine(transform_inverse(parent), current);
    }

    pose_set_local_transform(&bind_pose, i, current);
  }
  SM_ARRAY_DTOR(world_bind_pose);

  const char **bone_names = NULL;
  SM_ARRAY_NEW(bone_names, ai_mesh->mNumBones);
  SM_ARRAY_SET_SIZE(bone_names, ai_mesh->mNumBones);

  for (uint32_t bone_index = 0; bone_index < ai_mesh->mNumBones; ++bone_index) {
    struct aiBone *b = ai_mesh->mBones[bone_index];
    if (b->mName.length > 0) {
      bone_names[bone_index] = strdup(b->mName.data);
    } else {
      bone_names[bone_index] = strdup("EMPTY NODE");
    }
  }

  model->animation->skeleton = skeleton_new();
  if (!skeleton_ctor(model->animation->skeleton, &rest_pose, &bind_pose, (const char **)bone_names))
    SM_LOG_ERROR("error building skeleton\n");

  SM_ARRAY_DTOR(bone_names);

  pose_dtor(&rest_pose);
  pose_dtor(&bind_pose);
}

void load_animations(model2_s *model, const struct aiScene *scene) {

  uint32_t anim_count = scene->mNumAnimations;

  SM_ARRAY_SET_SIZE(model->animation->clips, anim_count);

  for (uint32_t i = 0; i < anim_count; ++i) {
    struct aiAnimation *anim = scene->mAnimations[i];
    uint32_t num_channels = anim->mNumChannels;

    model->animation->clips[i] = clip_new();
    if (!clip_ctor(model->animation->clips[i], anim->mName.data))
      exit(1);

    struct clip_s *clip = model->animation->clips[i];

    clip_set_cap_tracks(clip, num_channels);
    for (uint32_t j = 0; j < num_channels; ++j) {
      struct aiNodeAnim *channel = anim->mChannels[j];
      int32_t node_index =
          pose_get_index_by_name(skeleton_get_rest_pose(model->animation->skeleton), channel->mNodeName.data);

      interpolation_e interpolation = CONSTANT_INTERP;
      if (channel->mPreState == aiAnimBehaviour_LINEAR)
        interpolation = LINEAR_INTERP;
      else if (channel->mPreState == aiAnimBehaviour_REPEAT)
        interpolation = CUBIC_INTERP;

      transform_track_s *track = clip_get_transform_track_from_joint(clip, node_index);
      track_resize_frame(&track->position, channel->mNumPositionKeys);
      track_resize_frame(&track->rotation, channel->mNumRotationKeys);
      track_resize_frame(&track->scale, channel->mNumScalingKeys);

      for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k) {
        struct aiVectorKey *key = &channel->mPositionKeys[k];

        track->position.interpolation = interpolation;
        track_resize_frame_in(&track->position, 3, k);
        track_resize_frame_out(&track->position, 3, k);
        track_resize_frame_value(&track->position, 3, k);

        frame_s *frame = &track->position.frames[k];
        frame->t = key->mTime / 1000.0f;
        frame->value[0] = key->mValue.x;
        frame->value[1] = key->mValue.y;
        frame->value[2] = key->mValue.z;

        // TODO: fix this
        frame->in[0] = 0.0f;
        frame->in[1] = 0.0f;
        frame->in[2] = 0.0f;

        frame->out[0] = 0.0f;
        frame->out[1] = 0.0f;
        frame->out[2] = 0.0f;
      }

      for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k) {
        struct aiQuatKey *key = &channel->mRotationKeys[k];

        track->rotation.interpolation = interpolation;

        track_resize_frame_in(&track->rotation, 4, k);
        track_resize_frame_out(&track->rotation, 4, k);
        track_resize_frame_value(&track->rotation, 4, k);

        frame_s *frame = &track->rotation.frames[k];
        frame->t = key->mTime / 1000.0f;
        frame->value[0] = key->mValue.x;
        frame->value[1] = key->mValue.y;
        frame->value[2] = key->mValue.z;
        frame->value[3] = key->mValue.w;

        // TODO: fix this
        frame->in[0] = 0.0f;
        frame->in[1] = 0.0f;
        frame->in[2] = 0.0f;
        frame->in[3] = 0.0f;

        frame->out[0] = 0.0f;
        frame->out[1] = 0.0f;
        frame->out[2] = 0.0f;
        frame->out[3] = 0.0f;
      }

      for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k) {
        struct aiVectorKey *key = &channel->mScalingKeys[k];

        track->scale.interpolation = interpolation;
        track_resize_frame_in(&track->scale, 3, k);
        track_resize_frame_out(&track->scale, 3, k);
        track_resize_frame_value(&track->scale, 3, k);

        frame_s *frame = &track->scale.frames[k];
        frame->t = key->mTime / 1000.0f;
        frame->value[0] = key->mValue.x;
        frame->value[1] = key->mValue.y;
        frame->value[2] = key->mValue.z;

        // TODO: fix this
        frame->in[0] = 0.0f;
        frame->in[1] = 0.0f;
        frame->in[2] = 0.0f;

        frame->out[0] = 0.0f;
        frame->out[1] = 0.0f;
        frame->out[2] = 0.0f;
      }
    }
    clip_recalculate_duration(clip);
  }

  for (size_t i = 0; i < SM_ARRAY_SIZE(model->animation->clips); ++i) {
    for (size_t j = 0; j < clip_get_size(model->animation->clips[i]); ++j) {
      int32_t joint = clip_get_id_at_index(model->animation->clips[i], j);

      transform_track_s *ttrack = clip_get_transform_track_from_joint(model->animation->clips[i], joint);

      track_index_look_up_table(&ttrack->position);
      track_index_look_up_table(&ttrack->rotation);
      track_index_look_up_table(&ttrack->scale);
    }
  }
}

void process_node(model2_s *model, struct aiNode *node, const struct aiScene *scene, caller caller) {
  // process each mesh located at the current node
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {

    struct aiMesh *ai_mesh = scene->mMeshes[node->mMeshes[i]];
    // the node object only contains indices to index the actual objects in the scene.
    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).

    /* load_mesh(&m, ai_mesh); */
    caller(model, ai_mesh);
  }
  // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    process_node(model, node->mChildren[i], scene, caller);
  }
}

bool model_has_animation(model2_s *model) {
  return model->animation != NULL;
}
