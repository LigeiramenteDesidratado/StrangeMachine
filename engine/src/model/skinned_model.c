#include "input/input.h"
#include "shader/shader.h"
#include "util/bitmask.h"
#include "util/common.h"

#include "model/GLTF_loader.h"
#include "model/clip.h"
#include "model/controller.h"
#include "model/rearrange_bones.h"
#include "model/skinned_mesh.h"
#include "shader_program.h"

#include "shader/texture.h"
#include "shader/uniform.h"

#include "stb_ds/stb_ds.h"

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
  skinned_model_s *skinned_model = calloc(1, sizeof(skinned_model_s));

  assert(skinned_model != NULL);

  return skinned_model;
}

status_v skinned_model_ctor(skinned_model_s *skinned_model,
                            const string gltf_path, const string texture_path) {

  assert(skinned_model != NULL);

  cgltf_data *data = gltf_loader_load_file(gltf_path);
  skinned_model->meshes = gltf_loader_load_meshes(data);
  skinned_model->skeleton = gltf_loader_load_skeleton(data);
  skinned_model->clips = gltf_loader_load_animation_clips(data);
  gltf_loader_free_data(data);

  bone_map_s *opt_maps = rearrange_skeleton(skinned_model->skeleton);
  for (size_t i = 0; i < arrlenu(skinned_model->meshes); ++i) {
    rearrange_mesh(&skinned_model->meshes[i], opt_maps);
  }

  for (size_t i = 0; i < arrlenu(skinned_model->clips); ++i) {
    rearrange_clip(skinned_model->clips[i], opt_maps);
  }
  hmfree(opt_maps);

  skinned_model->texture = texture_new();
  if (!texture_ctor(&skinned_model->texture, texture_path)) {
    log_error("failed to construct texture");
    return fail;
  }

  struct controller_s *control = controller_new();
  if (!controller_ctor(control, skinned_model->skeleton))
    return fail;
  skinned_model->fade_controller = control;

  controller_play(skinned_model->fade_controller, skinned_model->clips[0]);
  controller_do(skinned_model->fade_controller, 0.0f);
  pose_get_matrix_palette(
      controller_get_current_pose(skinned_model->fade_controller),
      &skinned_model->pose_palette);

  skinned_model->next_clip = 0;
  skinned_model->current_clip = 0;

  return ok;
}

void skinned_model_dtor(skinned_model_s *skinned_model) {

  assert(skinned_model != NULL);

  controller_dtor(skinned_model->fade_controller);

  arrfree(skinned_model->pose_palette);

  skeleton_dtor(skinned_model->skeleton);

  for (size_t i = 0; i < arrlenu(skinned_model->clips); ++i) {
    clip_dtor(skinned_model->clips[i]);
  }
  arrfree(skinned_model->clips);

  for (size_t i = 0; i < arrlenu(skinned_model->meshes); ++i) {
    skinned_mesh_dtor(&skinned_model->meshes[i]);
  }
  arrfree(skinned_model->meshes);

  free(skinned_model);
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

    controller_fade_to(skinned_model->fade_controller,
                       skinned_model->clips[skinned_model->current_clip], 0.5f);
  }

  pose_get_matrix_palette(
      controller_get_current_pose(skinned_model->fade_controller),
      &skinned_model->pose_palette);

  mat4 **inverse_bind_pose =
      skeleton_get_inverse_bind_pose(skinned_model->skeleton);
  for (size_t i = 0; i < arrlenu(skinned_model->pose_palette); ++i) {
    skinned_model->pose_palette[i] =
        mat4_mul(skinned_model->pose_palette[i], (*inverse_bind_pose)[i]);
  }
}

void skinned_model_draw(skinned_model_s *skinned_model) {
  assert(skinned_model != NULL);

  shader_bind(SHADERS[SKINNED_SHADER_EX7]);
  uniform_set_array(
      glGetUniformLocation(SHADERS[SKINNED_SHADER_EX7], "animated"),
      skinned_model->pose_palette, arrlenu(skinned_model->pose_palette));

  texture_set(&skinned_model->texture,
              glGetUniformLocation(SHADERS[SKINNED_SHADER_EX7], "tex0"), 0);

  uint8_t flags = 0;
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.position);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.tex_coord);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.normal);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.weight);
  MASK_SET(flags, 1 << skinned_mesh_attr_locs.joint);

  for (size_t i = 0; i < arrlenu(skinned_model->meshes); ++i) {
    skinned_mesh_bind(&skinned_model->meshes[i], flags);

    GLuint handle = skinned_model->meshes[i].index_buffer.ebo;
    uint32_t num_indices = skinned_model->meshes[i].index_buffer.count;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    skinned_mesh_unbind(&skinned_model->meshes[i], flags);
  }

  texture_unset(0);
  shader_unbind();
}

status_v skinned_model_set_animation(skinned_model_s *skinned_model, const string animation) {

  for (unsigned int i = 0; i < arrlenu(skinned_model->clips); ++i) {
    if (strcmp(clip_get_name(skinned_model->clips[i]), animation) == 0) {
      skinned_model->next_clip = i;
      return ok;
    }
  }

  return fail;
}

void next_animation(skinned_model_s *sample) {
  sample->next_clip++;
  if (sample->next_clip >= arrlenu(sample->clips))
    sample->next_clip = 0;
}
