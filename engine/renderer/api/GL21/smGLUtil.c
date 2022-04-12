#include "smpch.h"

#include "vendor/gladGL21/glad.h"

#include "renderer/api/smTypes.h"

#include "core/smCore.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

SM_PRIVATE
const char *GL21error_to_string(GLenum error) {
  switch (error) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "UNKNOWN";
  }
}

bool GL21log_call() {
  GLenum err;
  while ((err = glGetError())) {
    SM_LOG_ERROR("[GL Error] (%d): %s", err, GL21error_to_string(err));
    return false;
  }
  return true;
}

void GL21clear_error() {
  while (glGetError() != GL_NO_ERROR) {}
}

GLint GL21map_sm_to_gl_type(types_e type) {

  switch (type) {
  case SM_FLOAT:
    return GL_FLOAT;
  case SM_INT:
    return GL_INT;
  case SM_IVEC2:
    return GL_INT_VEC2;
  case SM_IVEC3:
    return GL_INT_VEC3;
  case SM_IVEC4:
    return GL_INT_VEC4;
  case SM_VEC2:
    return GL_FLOAT_VEC2;
  case SM_VEC3:
    return GL_FLOAT_VEC3;
  case SM_VEC4:
    return GL_FLOAT_VEC4;
  case SM_MAT2:
    return GL_FLOAT_MAT2;
  case SM_MAT3:
    return GL_FLOAT_MAT3;
  case SM_MAT4:
    return GL_FLOAT_MAT4;
  default:
    SM_LOG_WARN("Unkown type (%d) %s", type, SM_TYPE_TO_STR(type));
    return SM_FLOAT;
  }
}
#undef SM_MODULE_NAME
