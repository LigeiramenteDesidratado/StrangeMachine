#include "util/common.h"

#include "smMesh.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smSkinnedMesh.h"
#include "smText.h"

GLuint SHADERS[MAX_SHADERS];

void shaders_init(void) {

  GLuint static_shader = 0;
  if (!shader_ctor(&static_shader, "engine/glsl/static.vs", "engine/glsl/static.fs")) {
    SM_LOG_ERROR("failed to create a new static shader");
    exit(1);
  }
  SHADERS[STATIC_SHADER] = static_shader;

  shader_bind_attrib_loc(SHADERS[STATIC_SHADER], mesh_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[STATIC_SHADER], mesh_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[STATIC_SHADER], mesh_attr_locs.normal, "normal");
  if (!shader_relink_program(SHADERS[STATIC_SHADER]))
    exit(1);

  GLuint skinned_shader = 0;
  if (!shader_ctor(&skinned_shader, "engine/glsl/skinned.vs", "engine/glsl/skinned.fs")) {
    SM_LOG_ERROR("failed to create a new skinned shader");
    exit(1);
  }
  SHADERS[SKINNED_SHADER] = skinned_shader;

  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER], skinned_mesh_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER], skinned_mesh_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER], skinned_mesh_attr_locs.normal, "normal");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER], skinned_mesh_attr_locs.weight, "weight");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER], skinned_mesh_attr_locs.joint, "joint");
  if (!shader_relink_program(SHADERS[SKINNED_SHADER]))
    exit(1);

  GLuint text_shader = 0;
  if (!shader_ctor(&text_shader, "engine/glsl/text.vs", "engine/glsl/text.fs")) {
    SM_LOG_ERROR("failed to create a new text shader");
    exit(1);
  }
  SHADERS[TEXT_SHADER] = text_shader;

  shader_bind_attrib_loc(SHADERS[TEXT_SHADER], text_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[TEXT_SHADER], text_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[TEXT_SHADER], text_attr_locs.color, "color");
  if (!shader_relink_program(SHADERS[TEXT_SHADER]))
    exit(1);

  GLuint debug_shader = 0;
  if (!shader_ctor(&debug_shader, "engine/glsl/debug.vs", "engine/glsl/debug.fs")) {
    SM_LOG_ERROR("failed to create a new debug shader");
    exit(1);
  }
  SHADERS[DEBUG_SHADER] = debug_shader;

  shader_bind_attrib_loc(SHADERS[DEBUG_SHADER], 1, "position");
  shader_bind_attrib_loc(SHADERS[DEBUG_SHADER], 2, "color");
  if (!shader_relink_program(SHADERS[DEBUG_SHADER]))
    exit(1);

  GLuint skybox_shader = 0;
  if (!shader_ctor(&skybox_shader, "engine/glsl/skybox.vs", "engine/glsl/skybox.fs")) {
    SM_LOG_ERROR("failed to create a new skybox shader");
    exit(1);
  }
  SHADERS[SKYBOX_SHADER] = skybox_shader;

  shader_bind_attrib_loc(SHADERS[SKYBOX_SHADER], 1, "position");
  if (!shader_relink_program(SHADERS[SKYBOX_SHADER]))
    exit(1);

  GLuint render_3d_shader = 0;
  if (!shader_ctor(&render_3d_shader, "engine/glsl/3Drender.vs", "engine/glsl/3Drender.fs")) {
    SM_LOG_ERROR("failed to create a new render 3D shader");
    exit(1);
  }
  SHADERS[RENDER_3D_SHADER] = render_3d_shader;

  shader_bind_attrib_loc(SHADERS[RENDER_3D_SHADER], 1, "position");
  shader_bind_attrib_loc(SHADERS[RENDER_3D_SHADER], 2, "color");
  shader_bind_attrib_loc(SHADERS[RENDER_3D_SHADER], 3, "normal");
  shader_bind_attrib_loc(SHADERS[RENDER_3D_SHADER], 4, "tex_coord");
  if (!shader_relink_program(SHADERS[RENDER_3D_SHADER]))
    exit(1);
}

void shaders_tear_down(void) {

  shader_dtor(SHADERS[STATIC_SHADER]);
  shader_dtor(SHADERS[SKINNED_SHADER]);
  shader_dtor(SHADERS[TEXT_SHADER]);
  shader_dtor(SHADERS[DEBUG_SHADER]);
}
