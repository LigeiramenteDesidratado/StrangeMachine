#include "glad/glad.h"
#include "util/common.h"
#include "util/file.h"

// private helper functions
static GLuint shader_compile_vert(string vertex);
static GLuint shader_compile_frag(string fragment);
static status_v shader_link(GLuint shader, GLuint vertex, GLuint fragment);

// Constructor
status_v shader_ctor(GLuint *shader, const string vs, const string fs) {

  assert(shader != NULL);
  assert(vs != NULL);
  assert(fs != NULL);

  string v_source = read_file(vs);
  if (!v_source)
    return fail;

  string f_source = read_file(fs);
  if (!f_source) {
    free(v_source);
    return fail;
  }

  GLuint vert = shader_compile_vert(v_source);
  GLuint frag = shader_compile_frag(f_source);

  free(v_source);
  free(f_source);

  if (!vert || !frag)
    return fail;

  *shader = glCreateProgram();

  if (!shader_link(*shader, vert, frag)) {
    glDeleteProgram(*shader);
    return fail;
  }

  return ok;
}

// Destructor
void shader_dtor(GLuint shader) { glDeleteProgram(shader); }

void shader_bind(GLuint shader) { glUseProgram(shader); }

void shader_unbind() { glUseProgram(0); }

void shader_bind_attrib_loc(GLuint shader, uint32_t loc, const string name) {

  glBindAttribLocation(shader, loc, name);
}

status_v shader_relink_program(GLuint shader) {

  glLinkProgram(shader);

  GLint success = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(shader, 2 * 512, NULL, info_log);
    log_error("shader relinking failed.\n\t%s", info_log);

    return fail;
  }

  log_info("relinked shaders successfully");
  return ok;
}

static GLuint shader_compile_vert(string vertex) {

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  const char *v_source = vertex;
  glShaderSource(v, 1, &v_source, NULL);
  glCompileShader(v);

  GLint success = 0;
  glGetShaderiv(v, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    char info_log[2 * 512];
    glGetShaderInfoLog(v, 2 * 512, NULL, info_log);
    log_error("vertex compilation failed.\n\t%s", info_log);
    glDeleteShader(v);
    return 0;
  }

  return v;
}

static GLuint shader_compile_frag(string fragment) {

  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  const char *f_source = fragment;
  glShaderSource(f, 1, &f_source, NULL);
  glCompileShader(f);

  GLint success = 0;
  glGetShaderiv(f, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(f, 2 * 512, NULL, info_log);
    log_error("fragment compilation failed.\n\t%s", info_log);
    glDeleteShader(f);
    return 0;
  }

  return f;
}

static status_v shader_link(GLuint shader, GLuint vertex, GLuint fragment) {

  glAttachShader(shader, vertex);
  glAttachShader(shader, fragment);
  glLinkProgram(shader);

  GLint success = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[2 * 512];
    glGetShaderInfoLog(shader, 2 * 512, NULL, info_log);
    log_error("shader linking failed.\n\t%s", info_log);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return fail;
  }

  log_info("compiled and linked shaders successfully");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return ok;
}
