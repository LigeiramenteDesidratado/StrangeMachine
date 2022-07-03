#ifndef SM_GL_UTIL_H
#define SM_GL_UTIL_H

#include "smpch.h"

#include "core/smCore.h"
#include "renderer/api/smTypes.h"
#include "renderer/smDevicePub.h"
#include "vendor/gladGL21/glad.h"

b8 GL21loader(loadproc_f load);

#ifdef SM_DEBUG
b8 GL21log_call();
void GL21clear_error();
GLenum GL21map_sm_to_gl_type(types_e type);
types_e GL21map_gl_to_sm_type(GLenum type);

  #define glCall(CALL)                                                                                                 \
    do {                                                                                                               \
      GL21clear_error();                                                                                               \
      CALL;                                                                                                            \
      SM_ASSERT(GL21log_call() && #CALL);                                                                              \
    } while (0)

#else

  #define glCall(CALL) CALL

#endif /* SM_DEBUG */

#endif /* SM_GL_UTIL_H */
