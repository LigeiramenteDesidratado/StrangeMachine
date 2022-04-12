#include "smpch.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"

#include "vendor/gladGL21/glad.h"

#include "core/smCore.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

typedef struct {

  GLuint program;

} shader_s;

static GLuint shader_compile_vert(char *vertex);
static GLuint shader_compile_frag(char *fragment);
static bool shader_link(GLuint shader, GLuint vertex, GLuint fragment);

shader_s *GL21shader_new(void) {

  shader_s *shader = SM_CALLOC(1, sizeof(shader_s));
  SM_ASSERT(shader);

  return shader;
}

bool GL21shader_ctor(shader_s *shader, const char *vertex_shader, const char *fragment_shader,
                     attribute_loc_desc_s *desc, size_t size) {

  SM_ASSERT(shader);
  SM_ASSERT(desc);

  char *vs_source = (char *)SM_FILE_READ(vertex_shader);
  if (!vs_source)
    return false;

  char *fs_source = (char *)SM_FILE_READ(fragment_shader);
  if (!fs_source) {
    SM_FREE(vs_source);
    return false;
  }

  GLuint vert = shader_compile_vert(vs_source);
  GLuint frag = shader_compile_frag(fs_source);

  SM_FREE(vs_source);
  SM_FREE(fs_source);

  if (!vert || !frag)
    return false;

  shader->program = glCreateProgram();

  for (uint8_t i = 0; i < size; ++i) {
    glCall(glBindAttribLocation(shader->program, desc[i].location, desc[i].name));
  }

  if (!shader_link(shader->program, vert, frag)) {
    SM_LOG_ERROR("Failed to link shader program");
    glCall(glDeleteProgram(shader->program));
    return false;
  }

  return true;
}

void GL21shader_dtor(shader_s *shader) {

  SM_ASSERT(shader);

  glCall(glDeleteProgram(shader->program));
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

void GL21shader_set_uniform(shader_s *shader, const char *name, void *value, types_e type) {

  SM_ASSERT(shader);
  SM_ASSERT(name);
  SM_ASSERT(value);

  GLint location;
  glCall(location = glGetUniformLocation(shader->program, name));

  SM_ASSERT(location != -1);

  switch (type) {
  case SM_INT:
    glCall(glUniform1i(location, *(int *)value));
    break;
  case SM_FLOAT:
    glCall(glUniform1f(location, *(float *)value));
    break;
  case SM_VEC2:
    glCall(glUniform2fv(location, 1, (float *)value));
    break;
  case SM_VEC3:
    glCall(glUniform3fv(location, 1, (float *)value));
    break;
  case SM_VEC4:
    glCall(glUniform4fv(location, 1, (float *)value));
    break;
  case SM_MAT4:
    glCall(glUniformMatrix4fv(location, 1, GL_FALSE, (float *)value));
    break;
  default:
    SM_LOG_ERROR("Unsupported type (%d) %s", type, SM_TYPE_TO_STR(type));
    break;
  }
}

void GL21shader_set_uniform_array(shader_s *shader, const char *name, void *value, uint32_t size, types_e type) {

  SM_ASSERT(shader);
  SM_ASSERT(name);
  SM_ASSERT(value);

  GLint location;
  glCall(location = glGetUniformLocation(shader->program, name));

  switch (type) {
  case SM_INT:
    glCall(glUniform1iv(location, size, (int *)value));
    break;
  case SM_FLOAT:
    glCall(glUniform1fv(location, size, (float *)value));
    break;
  case SM_VEC2:
    glCall(glUniform2fv(location, size, (float *)value));
    break;
  case SM_VEC3:
    glCall(glUniform3fv(location, size, (float *)value));
    break;
  case SM_VEC4:
    glCall(glUniform4fv(location, size, (float *)value));
    break;
  case SM_MAT4:
    glCall(glUniformMatrix4fv(location, size, GL_FALSE, (float *)value));
    break;
  default:
    SM_LOG_WARN("Unsupported type (%d) %s", type, SM_TYPE_TO_STR(type));
    break;
  }
}

// // private helper functions
// static GLuint shader_compile_vert(char *vertex);
// static GLuint shader_compile_frag(char *fragment);
// static bool shader_link(GLuint shader, GLuint vertex, GLuint fragment);
//
// // Constructor
// bool shader_ctor(GLuint *shader, const char *vs, const char *fs) {
//
//   SM_ASSERT(shader != NULL);
//   SM_ASSERT(vs != NULL);
//   SM_ASSERT(fs != NULL);
//
//   char *v_source = read_file(vs);
//   if (!v_source)
//     return false;
//
//   char *f_source = read_file(fs);
//   if (!f_source) {
//     SM_FREE(v_source);
//     return false;
//   }
//
//   GLuint vert = shader_compile_vert(v_source);
//   GLuint frag = shader_compile_frag(f_source);
//
//   SM_FREE(v_source);
//   SM_FREE(f_source);
//
//   if (!vert || !frag)
//     return false;
//
//   *shader = glCreateProgram();
//
//   if (!shader_link(*shader, vert, frag)) {
//     glDeleteProgram(*shader);
//     return false;
//   }
//
//   return true;
// }
//
// // Destructor
// void shader_dtor(GLuint shader) {
//   glDeleteProgram(shader);
// }
//
// void shader_bind(GLuint shader) {
//   glUseProgram(shader);
// }
//
// void shader_unbind() {
//   glUseProgram(0);
// }
//
// void shader_bind_attrib_loc(GLuint shader, uint32_t loc, const char *name) {
//   glBindAttribLocation(shader, loc, name);
// }
//

// bool shader_relink_program(GLuint shader) {
//
//   glLinkProgram(shader);
//
//   GLint success = 0;
//   glGetProgramiv(shader, GL_LINK_STATUS, &success);
//   if (!success) {
//     char info_log[2 * 512];
//     glGetShaderInfoLog(shader, 2 * 512, NULL, info_log);
//     SM_LOG_ERROR("shader relinking failed.\n\t%s", info_log);
//
//     return false;
//   }
//
//   SM_LOG_INFO("relinked shaders successfully");
//   return true;
// }

static GLuint shader_compile_vert(char *vertex) {

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  const char *v_source = vertex;
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

static GLuint shader_compile_frag(char *fragment) {

  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  const char *f_source = fragment;
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
