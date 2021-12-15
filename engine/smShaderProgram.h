#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include <stdint.h>
#include "glad/glad.h"

typedef uint8_t SHADERS_EX7;
#define STATIC_SHADER_EX7 ((SHADERS_EX7)0x00)
#define SKINNED_SHADER_EX7 ((SHADERS_EX7)0x01)
#define TEXT_SHADER_EX7 ((SHADERS_EX7)0x02)
#define DEBUG_SHADER_EX7 ((SHADERS_EX7)0x03)

extern GLuint SHADERS[4];

void shaders_init(void);
void shaders_tear_down(void);

#endif // SHADER_PROGRAM
