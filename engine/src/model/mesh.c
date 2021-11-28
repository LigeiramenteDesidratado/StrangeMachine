#include "model/mesh.h"
#include "shader/attribute.h"
#include "shader/index_buffer.h"
#include "util/bitmask.h"
#include "debug/debug.h"
#include "physics/shapes.h"
#include "util/common.h"
#include "stb_ds/stb_ds.h"

const struct mesh_attrs mesh_attr_locs = {
    .position = 1, .tex_coord = 2, .normal = 3};

// Constructor
status_v mesh_ctor(mesh_s *mesh) {

  assert(mesh != NULL);

  mesh->vertex.position_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.position_attr, VEC3_EX1))
    return fail;

  mesh->vertex.uv_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.uv_attr, VEC2_EX1))
    return fail;

  mesh->vertex.normal_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.normal_attr, VEC3_EX1))
    return fail;

  mesh->debug_attr = attribute_new();
  if (!attribute_ctor(&mesh->debug_attr, VEC3_EX1))
    return fail;

  mesh->debug_color_attr = attribute_new();
  if (!attribute_ctor(&mesh->debug_color_attr, VEC3_EX1))
    return fail;

  /* mesh->index_buffer = index_buffer_new(); */
  /* if (!index_buffer_ctor(&mesh->index_buffer)) */
  /* return fail; */

  return ok;
}

// Destructor
void mesh_dtor(mesh_s *mesh) {
  assert(mesh != NULL);

  /* index_buffer_dtor(&mesh->index_buffer); */
  /* if (mesh->indices != NULL) */
  /* free(mesh->indices); */

  attribute_dtor(&mesh->vertex.normal_attr);
  arrfree(mesh->vertex.normals);

  attribute_dtor(&mesh->vertex.uv_attr);
  arrfree(mesh->vertex.tex_coords);

  attribute_dtor(&mesh->vertex.position_attr);
  arrfree(mesh->vertex.positions);
}

void mesh_do(mesh_s *mesh) {
  assert(mesh != NULL);
}

void mesh_draw(mesh_s *mesh) {
  assert(mesh != NULL);
  /* draw_vertex(arrlenu(mesh->position), Triangles); */
}

void mesh_update_gl_buffers(mesh_s *mesh) {

  assert(mesh != NULL);

  if (arrlenu(mesh->vertex.positions)> 0)
    attribute_set(&mesh->vertex.position_attr, mesh->vertex.positions, arrlenu(mesh->vertex.positions),
                  GL_STATIC_DRAW);

  if (arrlenu(mesh->vertex.normals) > 0)
    attribute_set(&mesh->vertex.normal_attr, mesh->vertex.normals, arrlenu(mesh->vertex.normals),
                  GL_STATIC_DRAW);

  if (arrlenu(mesh->vertex.tex_coords) > 0)
    attribute_set(&mesh->vertex.uv_attr, mesh->vertex.tex_coords, arrlenu(mesh->vertex.tex_coords),
                  GL_STATIC_DRAW);

  if (arrlenu(mesh->indices) > 0)
    index_buffer_set(&mesh->index_buffer, mesh->indices, arrlenu(mesh->indices));
}

void mesh_bind(const mesh_s *const mesh, uint8_t flag) {

  assert(mesh != NULL);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.position)))
    attribute_bind_to(&mesh->vertex.position_attr, mesh_attr_locs.position);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.tex_coord)))
    attribute_bind_to(&mesh->vertex.uv_attr, mesh_attr_locs.tex_coord);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.normal)))
    attribute_bind_to(&mesh->vertex.normal_attr, mesh_attr_locs.normal);
}

void mesh_unbind(const mesh_s *const mesh, uint8_t flag) {

  assert(mesh != NULL);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.position)))
    attribute_unbind_from(&mesh->vertex.position_attr, mesh_attr_locs.position);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.tex_coord)))
    attribute_unbind_from(&mesh->vertex.uv_attr, mesh_attr_locs.tex_coord);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.normal)))
    attribute_unbind_from(&mesh->vertex.normal_attr, mesh_attr_locs.normal);
}

bounding_box_s mesh_get_bounding_box(mesh_s *mesh) {

  // Get min and max vertex to construct bounds (AABB)
  vec3 minVertex = {0};
  vec3 maxVertex = {0};

  if (mesh->vertex.positions != NULL) {
    minVertex = mesh->vertex.positions[0];
    maxVertex = mesh->vertex.positions[0];

    for (size_t i = 1; i < arrlenu(mesh->vertex.positions); i++) {
      minVertex = vec3_min(minVertex, mesh->vertex.positions[i]);
      maxVertex = vec3_max(maxVertex, mesh->vertex.positions[i]);
    }
  }

  // Create the bounding box
  bounding_box_s box;
  box.min = minVertex;
  box.max = maxVertex;

  return box;
}

void mesh_draw_debug(mesh_s *mesh) {

   bounding_box_s aabb = mesh_get_bounding_box(mesh); 

   debug_draw_aabb(aabb, vec3_new(1.0f, 0.4f, 0.3f));
}
