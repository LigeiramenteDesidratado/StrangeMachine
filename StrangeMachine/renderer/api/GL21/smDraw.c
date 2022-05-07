#include "vendor/gladGL21/glad.h"

#include "renderer/api/GL21/smGLUtil.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

void GL21init(void) {
  gladLoadGL();
  SM_LOG_INFO("GL21 initialized");
}

void GL21clear_color(float r, float g, float b, float a) {

  glCall(glClearColor(r, g, b, a));
}

void GL21clear(uint32_t mask) {

  /* Depois de 4 dias de debugging, finalmente posso morrer em paz. */
  glCall(glClear(mask));
}

void GL21_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  glCall(glViewport(x, y, width, height));
}

void GL21draw_indexed(uint32_t index_count) {

  glCall(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0));
}
#undef SM_MODULE_NAME
