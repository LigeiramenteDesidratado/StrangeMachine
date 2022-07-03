#include "vendor/gladGL21/glad.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "renderer/api/smTypes.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "GL21"

void GL21init(void) {
  gladLoadGL();
  SM_LOG_INFO("GL21 initialized");
}

void GL21enable(enable_flags_e flags) {
  glCall(glEnable(flags));
}

void GL21clear_color(float r, float g, float b, float a) {

  glCall(glClearDepth(1.0f));
  glCall(glClearColor(r, g, b, a));
}

void GL21clear(buffer_bit_e mask) {

  glCall(glClear(mask));
}

void GL21_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  glCall(glViewport(x, y, width, height));
}

void GL21draw_indexed(uint32_t index_count) {

  glCall(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0));
}
#undef SM_MODULE_NAME
