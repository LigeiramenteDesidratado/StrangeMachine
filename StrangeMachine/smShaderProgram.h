#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include "glad/glad.h"
#include <stdint.h>

typedef enum {
  STATIC_SHADER = 0x00,
  SKINNED_SHADER = 0x01,
  TEXT_SHADER = 0x02,
  DEBUG_SHADER = 0x03,
  SKYBOX_SHADER = 0x04,
  RENDER_3D_SHADER = 0x05,
  MAX_SHADERS

} shaders_e;

extern GLuint SHADERS[MAX_SHADERS];

void shaders_init(void);
void shaders_tear_down(void);

#endif // SHADER_PROGRAM
