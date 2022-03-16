#include "util/bitmask.h"
#include "util/common.h"

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

  assert(skinned_model != NULL);

  return skinned_model;
}

bool skinned_model_ctor(skinned_model_s *skinned_model, const char *gltf_path, const char *texture_path) {

  assert(skinned_model != NULL);

  cgltf_data *data = gltf_loader_load_file(gltf_path);
  skinned_model->meshes = gltf_loader_load_meshes(data);
  skinned_model->skeleton = gltf_loader_load_skeleton(data);
  skinned_model->clips = gltf_loader_load_animation_clips(data);
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
    log_error("failed to construct texture");
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

  assert(skinned_model != NULL);

  controller_dtor(skinned_model->fade_controller);

  SM_ARRAY_DTOR(skinned_model->pose_palette);

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

  if (input_scan_key_lock(SDL_SCANCODE_N)) {
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
  assert(skinned_model != NULL);

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

void next_animation(skinned_model_s *sample) {
  sample->next_clip++;
  if (sample->next_clip >= SM_ARRAY_SIZE(sample->clips))
    sample->next_clip = 0;
}
