#ifndef SM_MESH_H
#define SM_MESH_H

#include "smAttribute.h"
#include "smIndexBuffer.h"
#include "smShapes.h"
#include "smVertex.h"

// NOTE: in case of change make sure to update the src/shader/glsl/static.vs as
// well
struct mesh_attrs {
  const uint8_t position;
  const uint8_t tex_coord;
  const uint8_t normal;
};
extern const struct mesh_attrs mesh_attr_locs;

typedef struct {

  // vertex information
  vertex_s vertex;

  uint32_t *indices;
  index_buffer_s index_buffer;

  vec3 aabb_debug[24];
  attribute_s debug_attr; // VEC3_EX1

  vec3 debug_color;
  attribute_s debug_color_attr; // VEC3_EX1

} mesh_s;

#define mesh_new() ((mesh_s){0})

bool mesh_ctor(mesh_s *mesh);
void mesh_dtor(mesh_s *mesh);
void mesh_draw(mesh_s *mesh);
void mesh_update_gl_buffers(mesh_s *mesh);
void mesh_bind(const mesh_s *const mesh, uint8_t flag);
void mesh_unbind(const mesh_s *const mesh, uint8_t flag);
void mesh_draw_debug(mesh_s *mesh);
bounding_box_s mesh_get_bounding_box(mesh_s *mesh);

#endif // SM_MESH_H
