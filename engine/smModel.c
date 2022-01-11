#include "util/bitmask.h"
#include "util/common.h"

#include "smMesh.h"
#include "smOBJLoader.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smTexture.h"
#include "smUniform.h"
#include "smMem.h"

typedef struct {

  mesh_s *meshes;
  // TODO: materials?
  texture_s texture;
  transform_s transform;

} model_s;

static bool string_suffix(const char *str, const char *suffix);

model_s *model_new(void) {
  model_s *model = SM_CALLOC(1, sizeof(model_s));

  assert(model != NULL);

  return model;
}

bool model_ctor(model_s *model, const char *obj_path, const char *texture_path) {

  assert(model != NULL);
  assert(obj_path != NULL);
  assert(texture_path != NULL);

  if (string_suffix(obj_path, ".obj")) {
    if (!obj_loader_load(&model->meshes, obj_path)) {
      log_error("[%s] failed to load and parse object");
      return false;
    }
  } else {
    log_error("file format not supported");
    return false;
  }

  for (uint32_t i = 0; i < arrlenu(model->meshes); ++i) {
    if (!mesh_ctor(&model->meshes[i])) {
      log_error("failed to construct model");
      return false;
    }

    mesh_update_gl_buffers(&model->meshes[i]);
  }

  model->transform = transform_zero();

  model->texture = texture_new();
  if (!texture_ctor(&model->texture, texture_path)) {
    log_error("failed to construct texture");
    return false;
  }

  return true;
}

void model_dtor(model_s *model) {
  for (size_t i = 0; i < arrlenu(model->meshes); ++i) {
    mesh_dtor(&model->meshes[i]);
  }
  arrfree(model->meshes);

  texture_dtor(&model->texture);

  SM_FREE(model);
  model = NULL;
}

void model_do(model_s *model) {

  /* static int angle = 0; */
  /* if (angle++ >= 360) */
  /* angle = 0; */

  /* model->transform.rotation = */
  /* quat_angle_axis(angle * DEG2RAD, vec3_new(0.0f, 1.0f, 0.0f)); */
  /* model->transform.position.x += 0.5f; */
}

void model_draw(const model_s *const model) {

  shader_bind(SHADERS[STATIC_SHADER]);

  mat4 md = transform_to_mat4(model->transform);
  uniform_set_value(glGetUniformLocation(SHADERS[STATIC_SHADER], "model"), md);

  texture_set(&model->texture, glGetUniformLocation(SHADERS[STATIC_SHADER], "tex0"), 0);

  uint8_t flags = 0;
  MASK_SET(flags, 1 << mesh_attr_locs.position);
  MASK_SET(flags, 1 << mesh_attr_locs.tex_coord);
  MASK_SET(flags, 1 << mesh_attr_locs.normal);

  for (size_t i = 0; i < arrlenu(model->meshes); ++i) {
    mesh_bind(&model->meshes[i], flags);
    /* mesh_draw(model->meshes[i]); */
    glDrawArrays(GL_TRIANGLES, 0, arrlenu(model->meshes[i].vertex.positions));
    mesh_unbind(&model->meshes[i], flags);
  }
  texture_unset(0);

  shader_unbind();

  /* for (size_t i = 0; i < arrlenu(model->meshes); ++i) { */
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

mesh_s **model_get_meshes(model_s *model) { return &model->meshes; }
