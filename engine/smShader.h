#ifndef SM_SHADER_H
#define SM_SHADER_H

#include <stdbool.h>
#include <stdint.h>

#include "glad/glad.h"

// Constructor
bool shader_ctor(GLuint *shader, const char *vs, const char *fs);

// Destructor
void shader_dtor(GLuint shader);

// Forward declaration
void shader_bind(GLuint shader);
void shader_unbind(void);
void shader_bind_attrib_loc(GLuint shader, uint32_t loc, const char *name);
bool shader_relink_program(GLuint shader);

#endif // SM_SHADER_H
