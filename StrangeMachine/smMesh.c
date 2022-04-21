#include "data/array.h"
#include "util/bitmask.h"
#include "util/common.h"

#include "smDebug.h"
#include "smMem.h"
#include "smMesh.h"

const struct mesh_attrs mesh_attr_locs = {.position = 1, .tex_coord = 2, .normal = 3};

// Constructor
bool mesh_ctor(mesh_s *mesh) {

  SM_ASSERT(mesh != NULL);

  mesh->vertex.position_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.position_attr, VEC3_KIND))
    return false;

  mesh->vertex.uv_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.uv_attr, VEC2_KIND))
    return false;

  mesh->vertex.normal_attr = attribute_new();
  if (!attribute_ctor(&mesh->vertex.normal_attr, VEC3_KIND))
    return false;

  mesh->debug_attr = attribute_new();
  if (!attribute_ctor(&mesh->debug_attr, VEC3_KIND))
    return false;

  mesh->debug_color_attr = attribute_new();
  if (!attribute_ctor(&mesh->debug_color_attr, VEC3_KIND))
    return false;

  return true;
}

// Destructor
void mesh_dtor(mesh_s *mesh) {
  SM_ASSERT(mesh != NULL);

  attribute_dtor(&mesh->vertex.normal_attr);
  SM_ARRAY_DTOR(mesh->vertex.normals);

  attribute_dtor(&mesh->vertex.uv_attr);
  SM_ARRAY_DTOR(mesh->vertex.tex_coords);

  attribute_dtor(&mesh->vertex.position_attr);
  SM_ARRAY_DTOR(mesh->vertex.positions);
}

void mesh_do(mesh_s *mesh) {
  SM_ASSERT(mesh != NULL);
  SM_UNUSED(mesh);
}

void mesh_draw(mesh_s *mesh) {
  SM_ASSERT(mesh != NULL);
  SM_UNUSED(mesh);
  /* draw_vertex(SM_ARRAY_SIZE(mesh->position), Triangles); */
}

void mesh_update_gl_buffers(mesh_s *mesh) {

  SM_ASSERT(mesh != NULL);

  if (SM_ARRAY_SIZE(mesh->vertex.positions) > 0)
    attribute_set(&mesh->vertex.position_attr, mesh->vertex.positions, SM_ARRAY_SIZE(mesh->vertex.positions),
                  GL_STATIC_DRAW);

  if (SM_ARRAY_SIZE(mesh->vertex.normals) > 0)
    attribute_set(&mesh->vertex.normal_attr, mesh->vertex.normals, SM_ARRAY_SIZE(mesh->vertex.normals), GL_STATIC_DRAW);

  if (SM_ARRAY_SIZE(mesh->vertex.tex_coords) > 0)
    attribute_set(&mesh->vertex.uv_attr, mesh->vertex.tex_coords, SM_ARRAY_SIZE(mesh->vertex.tex_coords),
                  GL_STATIC_DRAW);
}

void mesh_bind(const mesh_s *const mesh, uint8_t flag) {

  SM_ASSERT(mesh != NULL);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.position)))
    attribute_bind_to(&mesh->vertex.position_attr, mesh_attr_locs.position);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.tex_coord)))
    attribute_bind_to(&mesh->vertex.uv_attr, mesh_attr_locs.tex_coord);

  if (MASK_CHK(flag, (1 << mesh_attr_locs.normal)))
    attribute_bind_to(&mesh->vertex.normal_attr, mesh_attr_locs.normal);
}

void mesh_unbind(const mesh_s *const mesh, uint8_t flag) {

  SM_ASSERT(mesh != NULL);

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
    glm_vec3_copy(mesh->vertex.positions[0], minVertex);
    glm_vec3_copy(mesh->vertex.positions[0], maxVertex);

    for (size_t i = 1; i < SM_ARRAY_SIZE(mesh->vertex.positions); ++i) {
      glm_vec3_minv(minVertex, mesh->vertex.positions[i], minVertex);
      glm_vec3_maxv(maxVertex, mesh->vertex.positions[i], maxVertex);
      /* minVertex = vec3_min(minVertex, mesh->vertex.positions[i]); */
      /* maxVertex = vec3_max(maxVertex, mesh->vertex.positions[i]); */
    }
  }

  // Create the bounding box
  bounding_box_s box;
  glm_vec3_copy(minVertex, box.min);
  glm_vec3_copy(maxVertex, box.max);
  /* box.min = minVertex; */
  /* box.max = maxVertex; */

  return box;
}

void mesh_draw_debug(mesh_s *mesh) {

  bounding_box_s aabb = mesh_get_bounding_box(mesh);

  debug_draw_aabb(aabb, vec3_new(1.0f, 0.4f, 0.3f));
}
