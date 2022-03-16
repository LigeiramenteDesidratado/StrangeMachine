#include "util/common.h"
#include "util/file.h"

#include "glad/glad.h"

// private helper functions
static GLuint shader_compile_vert(char *vertex);
static GLuint shader_compile_frag(char *fragment);
static bool shader_link(GLuint shader, GLuint vertex, GLuint fragment);

// Constructor
bool shader_ctor(GLuint *shader, const char *vs, const char *fs) {

  SM_ASSERT(shader != NULL);
  SM_ASSERT(vs != NULL);
  SM_ASSERT(fs != NULL);

  char *v_source = read_file(vs);
  if (!v_source)
    return false;

  char *f_source = read_file(fs);
  if (!f_source) {
    SM_FREE(v_source);
    return false;
  }

  GLuint vert = shader_compile_vert(v_source);
  GLuint frag = shader_compile_frag(f_source);

  SM_FREE(v_source);
  SM_FREE(f_source);

  if (!vert || !frag)
    return false;

  *shader = glCreateProgram();

  if (!shader_link(*shader, vert, frag)) {
    glDeleteProgram(*shader);
    return false;
  }

  return true;
}

// Destructor
void shader_dtor(GLuint shader) {
  glDeleteProgram(shader);
}

void shader_bind(GLuint shader) {
  glUseProgram(shader);
}

void shader_unbind() {
  glUseProgram(0);
}

void shader_bind_attrib_loc(GLuint shader, uint32_t loc, const char *name) {
  glBindAttribLocation(shader, loc, name);
}

bool shader_relink_program(GLuint shader) {

  glLinkProgram(shader);

  GLint success = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(shader, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("shader relinking failed.\n\t%s", info_log);

    return false;
  }

  SM_LOG_INFO("relinked shaders successfully");
  return true;
}

static GLuint shader_compile_vert(char *vertex) {

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  const char *v_source = vertex;
  glShaderSource(v, 1, &v_source, NULL);
  glCompileShader(v);

  GLint success = 0;
  glGetShaderiv(v, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    char info_log[2 * 512];
    glGetShaderInfoLog(v, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("vertex compilation failed.\n\t%s", info_log);
    glDeleteShader(v);
    return 0;
  }

  return v;
}

static GLuint shader_compile_frag(char *fragment) {

  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  const char *f_source = fragment;
  glShaderSource(f, 1, &f_source, NULL);
  glCompileShader(f);

  GLint success = 0;
  glGetShaderiv(f, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(f, 2 * 512, NULL, info_log);
    SM_LOG_ERROR("fragment compilation failed.\n\t%s", info_log);
    glDeleteShader(f);
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
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return false;
  }

  SM_LOG_INFO("compiled and linked shaders successfully");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return true;
}
