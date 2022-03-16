#ifndef SM_MESH_H
#define SM_MESH_H

#include "smAttribute.h"
#include "smIndexBuffer.h"
#include "smShapes.h"
#include "smTexture.h"
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
  attribute_s debug_attr; // VEC3_KIND

  vec3 debug_color;
  attribute_s debug_color_attr; // VEC3_KIND

} mesh_s;

typedef struct {

  // vertex information
  vec3 *positions;
  vec4 *colors;
  vec3 *normals;
  vec2 *uvs;
  uint32_t *indices;
  texture_s texture;

  // animation extra data
  vec4 *weights;
  ivec4 *influences;
  mat4 *pose_palette;

  bounding_box_s aabb;

} mesh2_s;

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
