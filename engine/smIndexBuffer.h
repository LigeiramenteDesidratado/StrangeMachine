#ifndef SM_INDEX_BUFFER_H
#define SM_INDEX_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include "glad/glad.h"

// An index buffer allows you to render a model using indexed geometry. Think of
// a human model; almost all triangles in the mesh will be connected. This means
// many triangles might share a single vertex. Instead of storing every single
// vertex, only unique vertices are stored. A buffer that indexes into the list
// of unique vertices, the index buffer, is used to create triangles out of the
// unique vertices.
typedef struct {
  GLuint ebo;
  size_t count;

} index_buffer_s;

#define index_buffer_new() ((index_buffer_s){.ebo = 0, .count = 0})

bool index_buffer_ctor(index_buffer_s *index_buffer);
void index_buffer_dtor(index_buffer_s *index_buffer);

void index_buffer_set(index_buffer_s *index_buffer, uint32_t *input_array, size_t length);

#endif // SM_INDEX_BUFFER_H
