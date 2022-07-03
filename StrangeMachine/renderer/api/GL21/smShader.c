#include "smpch.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"

#include "resource/smShaderResource.h"

#include "vendor/gladGL21/glad.h"

#include "core/smCore.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

typedef struct sm__shader_active_s {
  GLint size;
  GLenum type;
  GLint location;

} sm_shader_active_s;

typedef struct {
  GLuint program;

  sm_hashmap_str_m *actives;

} shader_s;

static GLuint shader_compile_vert(sm_string vertex);
static GLuint shader_compile_frag(sm_string fragment);
static bool shader_link(GLuint shader, GLuint vertex, GLuint fragment);

shader_s *GL21shader_new(void) {

  shader_s *shader = SM_CALLOC(1, sizeof(shader_s));
  SM_ASSERT(shader);

  return shader;
}

b8 hashmap_dtor_cb(sm_string key, void *value, void *user_data) {
  SM_UNUSED(user_data);
  sm_shader_active_s *active = (sm_shader_active_s *)value;

  sm_string_dtor(key);
  SM_FREE(active);

  return true;
}

b8 GL21shader_ctor(shader_s *shader, sm_shader_resource_handler_s handler) {

  SM_ASSERT(shader);

  sm_string vs_source = sm_shader_resource_get_vertex_data(handler);
  sm_string fs_source = sm_shader_resource_get_fragment_data(handler);

  GLuint vert = shader_compile_vert(vs_source);
  GLuint frag = shader_compile_frag(fs_source);

  sm_string_dtor(vs_source);
  sm_string_dtor(fs_source);

  if (!vert || !frag)
    return false;

  shader->program = glCreateProgram();

  if (!shader_link(shader->program, vert, frag)) {
    SM_LOG_ERROR("Failed to link shader program");
    glCall(glDeleteProgram(shader->program));
    return false;
  }

  shader->actives = sm_hashmap_new_str();
  if (!sm_hashmap_ctor_str(shader->actives, 16, NULL, NULL)) {
    SM_LOG_ERROR("Failed to create shader actives map");
    glCall(glDeleteProgram(shader->program));
    return false;
  }

  GLint num_uniforms;
  glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &num_uniforms);
  GLchar uniform_name[256];
  GLsizei uniform_length;
  GLint uniform_size;
  GLenum uniform_type;
  GLint uniform_location;
  for (int i = 0; i < num_uniforms; i++) {
    glGetActiveUniform(shader->program, i, sizeof(uniform_name), &uniform_length, &uniform_size, &uniform_type,
                       uniform_name);

    glCall(uniform_location = glGetUniformLocation(shader->program, uniform_name));
    /* SM_LOG_INFO("Uniform: %s, size: %d, type: %X, location: %d", uniform_name, uniform_size, uniform_type, */
    /*             uniform_location); */

    sm_shader_active_s *active = SM_CALLOC(1, sizeof(sm_shader_active_s));
    active->size = uniform_size;
    active->type = uniform_type;
    active->location = uniform_location;

    sm_hashmap_put_str(shader->actives, sm_string_from(uniform_name), active);
  }

  GLint num_attributes;
  glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, &num_attributes);
  GLchar attribute_name[256];
  GLsizei attribute_length;
  GLint attribute_size;
  GLenum attribute_type;
  GLint attribute_location;

  for (int i = 0; i < num_attributes; i++) {
    glGetActiveAttrib(shader->program, i, sizeof(attribute_name), &attribute_length, &attribute_size, &attribute_type,
                      attribute_name);
    glCall(attribute_location = glGetAttribLocation(shader->program, attribute_name));
    /* SM_LOG_INFO("Attribute: %s, size: %d, type: %X, location: %d", attribute_name, attribute_size, attribute_type, */
    /*             attribute_location); */

    sm_shader_active_s *active = SM_MALLOC(sizeof(sm_shader_active_s));
    SM_ASSERT(active);

    active->size = attribute_size;
    active->type = attribute_type;
    active->location = attribute_location;

    sm_hashmap_put_str(shader->actives, sm_string_from(attribute_name), active);
  }

  return true;
}

void GL21shader_dtor(shader_s *shader) {

  SM_ASSERT(shader);

  glCall(glDeleteProgram(shader->program));

  sm_hashmap_for_each_str(shader->actives, hashmap_dtor_cb, NULL);
  sm_hashmap_dtor_str(shader->actives);

  SM_FREE(shader);
}

void GL21shader_bind(shader_s *shader) {

  SM_ASSERT(shader);

  glCall(glUseProgram(shader->program));
}

void GL21shader_unbind(shader_s *shader) {

  SM_ASSERT(shader);

  glCall(glUseProgram(0));
}

void GL21shader_set_uniform(shader_s *shader, sm_string name, void *value, types_e type) {

  SM_ASSERT(shader);
  SM_ASSERT(value);

  /* GLint location; */
  /* glCall(location = glGetUniformLocation(shader->program, name)); */
  /* glGetUniformLocation */

  sm_shader_active_s *active = sm_hashmap_get_str(shader->actives, name);
  if (!active) {
    SM_LOG_ERROR("Failed to find uniform %s", name.str);
    return;
  }

  /* SM_LOG_INFO("Uniform: %s, size: %d, type: 0x%X, location: %d", name, active->size, active->type, active->location);
   */
  /* SM_LOG_INFO("Type: %d, toGL: 0x%X", type, GL21map_sm_to_gl_type(type)); */

  if (active->type != GL21map_sm_to_gl_type(type)) {
    SM_LOG_ERROR("Uniform %s type mismatch", name.str);
    return;
  }

  switch (type) {

  case SM_SAMPLER2D:
    /* fallthrough */
  case SM_I32:
    glCall(glUniform1i(active->location, *(int *)value));
    break;
  case SM_F32:
    glCall(glUniform1f(active->location, *(f32 *)value));
    break;
  case SM_VEC2:
    glCall(glUniform2fv(active->location, 1, (f32 *)value));
    break;
  case SM_VEC3:
    glCall(glUniform3fv(active->location, 1, (f32 *)value));
    break;
  case SM_VEC4:
    glCall(glUniform4fv(active->location, 1, (f32 *)value));
    break;
  case SM_MAT4:
    glCall(glUniformMatrix4fv(active->location, 1, GL_FALSE, (f32 *)value));
    break;
  default:
    SM_LOG_ERROR("Unsupported type (%d) %s", type, SM_TYPE_TO_STR(type));
    break;
  }
}

void GL21shader_set_uniform_array(shader_s *shader, const char *name, void *value, u32 size, types_e type) {

  SM_ASSERT(shader);
  SM_ASSERT(name);
  SM_ASSERT(value);

  GLint location;
  glCall(location = glGetUniformLocation(shader->program, name));

  switch (type) {
  case SM_I32:
    glCall(glUniform1iv(location, (GLsizei)size, (int *)value));
    break;
  case SM_F32:
    glCall(glUniform1fv(location, (GLsizei)size, (f32 *)value));
    break;
  case SM_VEC2:
    glCall(glUniform2fv(location, (GLsizei)size, (f32 *)value));
    break;
  case SM_VEC3:
    glCall(glUniform3fv(location, (GLsizei)size, (f32 *)value));
    break;
  case SM_VEC4:
    glCall(glUniform4fv(location, (GLsizei)size, (f32 *)value));
    break;
  case SM_MAT4:
    glCall(glUniformMatrix4fv(location, (GLsizei)size, GL_FALSE, (f32 *)value));
    break;
  default:
    SM_LOG_WARN("Unsupported type (%d) %s", type, SM_TYPE_TO_STR(type));
    break;
  }
}

types_e GL21shader_get_type(shader_s *shader, sm_string name) {

  SM_ASSERT(shader);

  sm_shader_active_s *active = sm_hashmap_get_str(shader->actives, name);
  if (!active) {
    SM_LOG_ERROR("Failed to find uniform %s", name.str);
    return SM_F32;
  }

  return active->type;
}

i32 GL21shader_get_location(shader_s *shader, sm_string name) {

  SM_ASSERT(shader);

  sm_shader_active_s *active = sm_hashmap_get_str(shader->actives, name);
  if (!active) {
    SM_LOG_ERROR("Failed to find uniform %s", name.str);
    return -1;
  }

  return active->location;
}

static GLuint shader_compile_vert(sm_string vertex) {

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  const char *v_source = vertex.str;
  glCall(glShaderSource(v, 1, &v_source, NULL));
  glCall(glCompileShader(v));

  GLint success = 0;
  glCall(glGetShaderiv(v, GL_COMPILE_STATUS, &success));
  if (success != GL_TRUE) {
    char info_log[2 * 512];
    glGetShaderInfoLog(v, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("vertex compilation failed.\n\t%s", info_log);
    glCall(glDeleteShader(v));
    return 0;
  }

  return v;
}

static GLuint shader_compile_frag(sm_string fragment) {

  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  const char *f_source = fragment.str;
  glCall(glShaderSource(f, 1, &f_source, NULL));
  glCall(glCompileShader(f));

  GLint success = 0;
  glCall(glGetShaderiv(f, GL_COMPILE_STATUS, &success));
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(f, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("fragment compilation failed.\n\t%s", info_log);
    glCall(glDeleteShader(f));
    return 0;
  }

  return f;
}

static bool shader_link(GLuint shader, GLuint vertex, GLuint fragment) {

  glAttachShader(shader, vertex);
  glAttachShader(shader, fragment);
  glLinkProgram(shader);

  GLint success = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(shader, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("shader linking failed.\n\t%s", info_log);
    glCall(glDeleteShader(vertex));
    glCall(glDeleteShader(fragment));

    return false;
  }

  SM_LOG_INFO("compiled and linked shaders successfully");

  glCall(glDeleteShader(vertex));
  glCall(glDeleteShader(fragment));

  return true;
}

#undef SM_MODULE_NAME
