#ifndef SM_SHADER_H
#define SM_SHADER_H

#include "util/common.h"

// Constructor
status_v shader_ctor(GLuint *shader, const string vs, const string fs);

// Destructor
void shader_dtor(GLuint shader);

// Forward declaration
void shader_bind(GLuint shader);
void shader_unbind(void);
void shader_bind_attrib_loc(GLuint shader, uint32_t loc, const string name);
status_v shader_relink_program(GLuint shader);

#endif // SM_SHADER_H
