#include "shader_program.h"
#include "shader/shader.h"
#include "util/common.h"
#include "model/skinned_mesh.h"
#include "model/mesh.h"
#include "text/text.h"

GLuint SHADERS[4];

void shaders_init(void) {

  GLuint static_shader = 0;
  if (!shader_ctor(&static_shader, "engine/src/shader/glsl/static.vs",
                   "engine/src/shader/glsl/static.fs")) {
    log_error("failed to create a new static shader");
    exit(1);
  }
  SHADERS[STATIC_SHADER_EX7] = static_shader;

  shader_bind_attrib_loc(SHADERS[STATIC_SHADER_EX7], mesh_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[STATIC_SHADER_EX7], mesh_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[STATIC_SHADER_EX7], mesh_attr_locs.normal, "normal");
  if (!shader_relink_program(SHADERS[STATIC_SHADER_EX7]))
    exit(1);

  GLuint skinned_shader = 0;
  if (!shader_ctor(&skinned_shader, "engine/src/shader/glsl/skinned.vs",
                   "engine/src/shader/glsl/skinned.fs")) {
    log_error("failed to create a new skinned shader");
    exit(1);
  }
  SHADERS[SKINNED_SHADER_EX7] = skinned_shader;

  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER_EX7], skinned_mesh_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER_EX7], skinned_mesh_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER_EX7], skinned_mesh_attr_locs.normal, "normal");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER_EX7], skinned_mesh_attr_locs.weight, "weight");
  shader_bind_attrib_loc(SHADERS[SKINNED_SHADER_EX7], skinned_mesh_attr_locs.joint, "joint");
  if (!shader_relink_program(SHADERS[SKINNED_SHADER_EX7]))
    exit(1);

  GLuint text_shader = 0;
  if (!shader_ctor(&text_shader, "engine/src/shader/glsl/text.vs", "engine/src/shader/glsl/text.fs")) {
    log_error("failed to create a new text shader");
    exit(1);
  }
  SHADERS[TEXT_SHADER_EX7] = text_shader;
  
  shader_bind_attrib_loc(SHADERS[TEXT_SHADER_EX7], text_attr_locs.position, "position");
  shader_bind_attrib_loc(SHADERS[TEXT_SHADER_EX7], text_attr_locs.tex_coord, "tex_coord");
  shader_bind_attrib_loc(SHADERS[TEXT_SHADER_EX7], text_attr_locs.color, "color");
  if (!shader_relink_program(SHADERS[TEXT_SHADER_EX7]))
    exit(1);

  GLuint debug_shader = 0;
  if (!shader_ctor(&debug_shader, "engine/src/shader/glsl/debug.vs", "engine/src/shader/glsl/debug.fs")) {
    log_error("failed to create a new debug shader");
    exit(1);
  }
  SHADERS[DEBUG_SHADER_EX7] = debug_shader;

  shader_bind_attrib_loc(SHADERS[DEBUG_SHADER_EX7], 1, "position");
  shader_bind_attrib_loc(SHADERS[DEBUG_SHADER_EX7], 2, "color");
  if (!shader_relink_program(SHADERS[DEBUG_SHADER_EX7]))
    exit(1);

}

void shaders_tear_down(void) {

  shader_dtor(SHADERS[STATIC_SHADER_EX7]);
  shader_dtor(SHADERS[SKINNED_SHADER_EX7]);
  shader_dtor(SHADERS[TEXT_SHADER_EX7]);
  shader_dtor(SHADERS[DEBUG_SHADER_EX7]);
}
