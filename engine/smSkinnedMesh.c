#include "smSkinnedMesh.h"
#include "smMem.h"
#include "util/bitmask.h"
#include "util/common.h"

const struct skinned_mesh_attrs skinned_mesh_attr_locs = {
    .position = 1, .tex_coord = 2, .normal = 3, .weight = 4, .joint = 5};

void skinned_mesh_update_gl_buffers(skinned_mesh_s *mesh);

// Constructor
bool skinned_mesh_ctor(skinned_mesh_s *mesh) {

  assert(mesh != NULL);

  attribute_s position_attr = attribute_new();
  if (!attribute_ctor(&position_attr, VEC3_KIND))
    return false;
  mesh->vertex.position_attr = position_attr;

  attribute_s normal_attr = attribute_new();
  if (!attribute_ctor(&normal_attr, VEC3_KIND))
    return false;
  mesh->vertex.normal_attr = normal_attr;

  attribute_s uv_attr = attribute_new();
  if (!attribute_ctor(&uv_attr, VEC2_KIND))
    return false;
  mesh->vertex.uv_attr = uv_attr;

  attribute_s weight_attr = attribute_new();
  if (!attribute_ctor(&weight_attr, VEC4_KIND))
    return false;
  mesh->weight_attr = weight_attr;

  attribute_s influence_attr = attribute_new();
  if (!attribute_ctor(&influence_attr, IVEC4_KIND))
    return false;
  mesh->influence_attr = influence_attr;

  index_buffer_s index_buffer = index_buffer_new();
  if (!index_buffer_ctor(&index_buffer))
    return false;
  mesh->index_buffer = index_buffer;

  mesh->vertex.positions = (vec3 *)SM_ARRAY_NEW_EMPTY();
  mesh->vertex.tex_coords = (vec2 *)SM_ARRAY_NEW_EMPTY();
  mesh->vertex.normals = (vec3 *)SM_ARRAY_NEW_EMPTY();

  mesh->weights = (vec4 *)SM_ARRAY_NEW_EMPTY();
  mesh->influences = (ivec4 *)SM_ARRAY_NEW_EMPTY();
  mesh->indices = (uint32_t *)SM_ARRAY_NEW_EMPTY();

  mesh->skinned_position = (vec3 *)SM_ARRAY_NEW_EMPTY();
  mesh->skinned_normal = (vec3 *)SM_ARRAY_NEW_EMPTY();
  mesh->pose_palette = (mat4 *)SM_ARRAY_NEW_EMPTY();

  return true;
}

// Destructor
void skinned_mesh_dtor(skinned_mesh_s *mesh) {
  assert(mesh != NULL);

  index_buffer_dtor(&mesh->index_buffer);
  attribute_dtor(&mesh->influence_attr);
  attribute_dtor(&mesh->weight_attr);
  attribute_dtor(&mesh->vertex.uv_attr);
  attribute_dtor(&mesh->vertex.normal_attr);
  attribute_dtor(&mesh->vertex.position_attr);

  SM_ARRAY_DTOR(mesh->vertex.positions);
  SM_ARRAY_DTOR(mesh->vertex.tex_coords);
  SM_ARRAY_DTOR(mesh->vertex.normals);
  SM_ARRAY_DTOR(mesh->weights);
  SM_ARRAY_DTOR(mesh->influences);
  SM_ARRAY_DTOR(mesh->indices);
  SM_ARRAY_DTOR(mesh->skinned_position);
  SM_ARRAY_DTOR(mesh->skinned_normal);
  SM_ARRAY_DTOR(mesh->pose_palette);
}

// mesh_copy which copy out the CPU-side members
// (all of the vectors) and then call the UpdateOpenGLBuffers function to upload
// the attribute data to the GPU
void skinned_mesh_copy(skinned_mesh_s *dest, const skinned_mesh_s *const src) {

  assert(dest != NULL);
  assert(src != NULL);
  if (src == dest)
    return;

  SM_ARRAY_SET_SIZE(dest->vertex.positions, SM_ARRAY_SIZE(src->vertex.positions));
  memcpy(dest->vertex.positions, src->vertex.positions, sizeof(vec3) * SM_ARRAY_SIZE(src->vertex.positions));

  SM_ARRAY_SET_SIZE(dest->vertex.normals, SM_ARRAY_SIZE(src->vertex.normals));
  memcpy(dest->vertex.normals, src->vertex.normals, sizeof(vec3) * SM_ARRAY_SIZE(src->vertex.normals));

  SM_ARRAY_SET_SIZE(dest->vertex.tex_coords, SM_ARRAY_SIZE(src->vertex.tex_coords));
  memcpy(dest->vertex.tex_coords, src->vertex.tex_coords, sizeof(vec2) * SM_ARRAY_SIZE(src->vertex.tex_coords));

  SM_ARRAY_SET_SIZE(dest->weights, SM_ARRAY_SIZE(src->weights));
  memcpy(dest->weights, src->weights, sizeof(vec4) * SM_ARRAY_SIZE(src->weights));

  SM_ARRAY_SET_SIZE(dest->influences, SM_ARRAY_SIZE(src->influences));
  memcpy(dest->influences, src->influences, sizeof(ivec4) * SM_ARRAY_SIZE(src->influences));

  SM_ARRAY_SET_SIZE(dest->indices, SM_ARRAY_SIZE(src->indices));
  memcpy(dest->indices, src->indices, sizeof(uint32_t) * SM_ARRAY_SIZE(src->indices));

  skinned_mesh_update_gl_buffers(dest);
}

void skinned_mesh_update_gl_buffers(skinned_mesh_s *mesh) {
  assert(mesh != NULL);

  if (SM_ARRAY_SIZE(mesh->vertex.positions) > 0)
    attribute_set(&mesh->vertex.position_attr, mesh->vertex.positions, SM_ARRAY_SIZE(mesh->vertex.positions),
                  GL_STREAM_DRAW);

  if (SM_ARRAY_SIZE(mesh->vertex.normals) > 0)
    attribute_set(&mesh->vertex.normal_attr, mesh->vertex.normals, SM_ARRAY_SIZE(mesh->vertex.normals), GL_STREAM_DRAW);

  if (SM_ARRAY_SIZE(mesh->vertex.tex_coords) > 0)
    attribute_set(&mesh->vertex.uv_attr, mesh->vertex.tex_coords, SM_ARRAY_SIZE(mesh->vertex.tex_coords),
                  GL_STREAM_DRAW);

  if (SM_ARRAY_SIZE(mesh->weights) > 0)
    attribute_set(&mesh->weight_attr, mesh->weights, SM_ARRAY_SIZE(mesh->weights), GL_STREAM_DRAW);

  if (SM_ARRAY_SIZE(mesh->influences) > 0)
    attribute_set(&mesh->influence_attr, mesh->influences, SM_ARRAY_SIZE(mesh->influences), GL_STREAM_DRAW);

  if (SM_ARRAY_SIZE(mesh->indices) > 0)
    index_buffer_set(&mesh->index_buffer, &mesh->indices[0], SM_ARRAY_SIZE(mesh->indices));
}

void skinned_mesh_bind(const skinned_mesh_s *const mesh, uint8_t flag) {

  assert(mesh != NULL);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.position)))
    attribute_bind_to(&mesh->vertex.position_attr, skinned_mesh_attr_locs.position);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.normal)))
    attribute_bind_to(&mesh->vertex.normal_attr, skinned_mesh_attr_locs.normal);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.tex_coord)))
    attribute_bind_to(&mesh->vertex.uv_attr, skinned_mesh_attr_locs.tex_coord);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.weight)))
    attribute_bind_to(&mesh->weight_attr, skinned_mesh_attr_locs.weight);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.joint)))
    attribute_bind_to(&mesh->influence_attr, skinned_mesh_attr_locs.joint);
}

void skinned_mesh_unbind(const skinned_mesh_s *const mesh, uint8_t flag) {
  assert(mesh != NULL);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.position)))
    attribute_unbind_from(&mesh->vertex.position_attr, skinned_mesh_attr_locs.position);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.normal)))
    attribute_unbind_from(&mesh->vertex.normal_attr, skinned_mesh_attr_locs.normal);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.tex_coord)))
    attribute_unbind_from(&mesh->vertex.uv_attr, skinned_mesh_attr_locs.tex_coord);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.weight)))
    attribute_unbind_from(&mesh->weight_attr, skinned_mesh_attr_locs.weight);

  if (MASK_CHK(flag, (1 << skinned_mesh_attr_locs.joint)))
    attribute_unbind_from(&mesh->influence_attr, skinned_mesh_attr_locs.joint);
}

// void skinned_mesh_draw(skinned_mesh_s *mesh) {
//   assert(mesh != NULL);
//   if (SM_ARRAY_SIZE(mesh->indices) > 0) {
//     draw_index_buffer(mesh->index_buffer, Triangles);
//   } else {
//     draw_vertex(SM_ARRAY_SIZE(mesh->position), Triangles);
//   }
// }
//
// void skinned_mesh_draw_instanced(skinned_mesh_s *mesh, uint32_t
// num_instances) {
//   assert(mesh != NULL);
//
//   if (SM_ARRAY_SIZE(mesh->indices) > 0) {
//     // TODO: waiting for draw_index_buffer_instanced impl...
//     log_warn("not implemented..\n");
//   } else {
//     draw_vertex_instanced(SM_ARRAY_SIZE(mesh->position), Triangles, num_instances);
//   }
// }
