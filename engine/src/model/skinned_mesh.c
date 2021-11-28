#include "model/skinned_mesh.h"
#include "shader/index_buffer.h"
#include "stb_ds/stb_ds.h"
#include "util/bitmask.h"
#include "util/common.h"

const struct skinned_mesh_attrs skinned_mesh_attr_locs = {
    .position = 1, .tex_coord = 2, .normal = 3, .weight = 4, .joint = 5};

void skinned_mesh_update_gl_buffers(skinned_mesh_s *mesh);

// Constructor
status_v skinned_mesh_ctor(skinned_mesh_s *mesh) {

  assert(mesh != NULL);

  attribute_s position_attr = attribute_new();
  if (!attribute_ctor(&position_attr, VEC3_EX1))
    return fail;
  mesh->vertex.position_attr = position_attr;

  attribute_s normal_attr = attribute_new();
  if (!attribute_ctor(&normal_attr, VEC3_EX1))
    return fail;
  mesh->vertex.normal_attr = normal_attr;

  attribute_s uv_attr = attribute_new();
  if (!attribute_ctor(&uv_attr, VEC2_EX1))
    return fail;
  mesh->vertex.uv_attr = uv_attr;

  attribute_s weight_attr = attribute_new();
  if (!attribute_ctor(&weight_attr, VEC4_EX1))
    return fail;
  mesh->weight_attr = weight_attr;

  attribute_s influence_attr = attribute_new();
  if (!attribute_ctor(&influence_attr, IVEC4_EX1))
    return fail;
  mesh->influence_attr = influence_attr;

  index_buffer_s index_buffer = index_buffer_new();
  if (!index_buffer_ctor(&index_buffer))
    return fail;
  mesh->index_buffer = index_buffer;

  return ok;
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

  arrfree(mesh->vertex.positions);
  arrfree(mesh->vertex.tex_coords);
  arrfree(mesh->vertex.normals);
  arrfree(mesh->weights);
  arrfree(mesh->influences);
  arrfree(mesh->indices);
  arrfree(mesh->skinned_position);
  arrfree(mesh->skinned_normal);
  arrfree(mesh->pose_palette);
}

// mesh_copy which copy out the CPU-side members
// (all of the vectors) and then call the UpdateOpenGLBuffers function to upload
// the attribute data to the GPU
void skinned_mesh_copy(skinned_mesh_s *dest, const skinned_mesh_s *const src) {

  assert(dest != NULL);
  assert(src != NULL);
  if (src == dest)
    return;

  arrsetlen(dest->vertex.positions, arrlenu(src->vertex.positions));
  memcpy(dest->vertex.positions, src->vertex.positions, sizeof(vec3) * arrlenu(src->vertex.positions));

  arrsetlen(dest->vertex.normals, arrlenu(src->vertex.normals));
  memcpy(dest->vertex.normals, src->vertex.normals, sizeof(vec3) * arrlenu(src->vertex.normals));

  arrsetlen(dest->vertex.tex_coords, arrlenu(src->vertex.tex_coords));
  memcpy(dest->vertex.tex_coords, src->vertex.tex_coords, sizeof(vec2) * arrlenu(src->vertex.tex_coords));

  arrsetlen(dest->weights, arrlenu(src->weights));
  memcpy(dest->weights, src->weights, sizeof(vec4) * arrlenu(src->weights));

  arrsetlen(dest->influences, arrlenu(src->influences));
  memcpy(dest->influences, src->influences, sizeof(ivec4) * arrlenu(src->influences));

  arrsetlen(dest->indices, arrlenu(src->indices));
  memcpy(dest->indices, src->indices, sizeof(uint32_t) * arrlenu(src->indices));

  skinned_mesh_update_gl_buffers(dest);
}

void skinned_mesh_update_gl_buffers(skinned_mesh_s *mesh) {
  assert(mesh != NULL);

  if (arrlenu(mesh->vertex.positions) > 0)
    attribute_set(&mesh->vertex.position_attr, mesh->vertex.positions, arrlenu(mesh->vertex.positions), GL_STREAM_DRAW);

  if (arrlenu(mesh->vertex.normals) > 0)
    attribute_set(&mesh->vertex.normal_attr, mesh->vertex.normals, arrlenu(mesh->vertex.normals), GL_STREAM_DRAW);

  if (arrlenu(mesh->vertex.tex_coords) > 0)
    attribute_set(&mesh->vertex.uv_attr, mesh->vertex.tex_coords, arrlenu(mesh->vertex.tex_coords), GL_STREAM_DRAW);

  if (arrlenu(mesh->weights) > 0)
    attribute_set(&mesh->weight_attr, mesh->weights, arrlenu(mesh->weights), GL_STREAM_DRAW);

  if (arrlenu(mesh->influences) > 0)
    attribute_set(&mesh->influence_attr, mesh->influences, arrlenu(mesh->influences), GL_STREAM_DRAW);

  if (arrlenu(mesh->indices) > 0)
    index_buffer_set(&mesh->index_buffer, &mesh->indices[0], arrlenu(mesh->indices));
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
//   if (arrlenu(mesh->indices) > 0) {
//     draw_index_buffer(mesh->index_buffer, Triangles);
//   } else {
//     draw_vertex(arrlenu(mesh->position), Triangles);
//   }
// }
//
// void skinned_mesh_draw_instanced(skinned_mesh_s *mesh, uint32_t
// num_instances) {
//   assert(mesh != NULL);
//
//   if (arrlenu(mesh->indices) > 0) {
//     // TODO: waiting for draw_index_buffer_instanced impl...
//     log_warn("not implemented..\n");
//   } else {
//     draw_vertex_instanced(arrlenu(mesh->position), Triangles, num_instances);
//   }
// }

void skinned_mesh_CPU_skin_matrix_opt(skinned_mesh_s *mesh, mat4 *animated_pose) {

  assert(mesh != NULL);

  size_t num_verts = arrlenu(mesh->vertex.positions);
  assert(num_verts != 0);

  arrsetlen(mesh->skinned_position, num_verts);
  arrsetlen(mesh->skinned_normal, num_verts);

  for (size_t i = 0; i < num_verts; ++i) {

    ivec4 j = mesh->influences[i];
    vec4 w = mesh->weights[i];

    // TODO: animated_pose missing size??
    vec3 p0 = mat4_transform_point(animated_pose[j.x], mesh->vertex.positions[i]);
    vec3 p1 = mat4_transform_point(animated_pose[j.y], mesh->vertex.positions[i]);
    vec3 p2 = mat4_transform_point(animated_pose[j.z], mesh->vertex.positions[i]);
    vec3 p3 = mat4_transform_point(animated_pose[j.w], mesh->vertex.positions[i]);

    mesh->skinned_position[i] =
        vec3_add(vec3_add(vec3_scale(p0, w.x), vec3_scale(p1, w.y)),
                 vec3_add(vec3_scale(p2, w.z), vec3_scale(p3, w.w)));

    vec3 n0 = mat4_transform_vec3(animated_pose[j.x], mesh->vertex.normals[i]);
    vec3 n1 = mat4_transform_vec3(animated_pose[j.y], mesh->vertex.normals[i]);
    vec3 n2 = mat4_transform_vec3(animated_pose[j.z], mesh->vertex.normals[i]);
    vec3 n3 = mat4_transform_vec3(animated_pose[j.w], mesh->vertex.normals[i]);
    mesh->skinned_normal[i] =
        vec3_add(vec3_add(vec3_scale(n0, w.x), vec3_scale(n1, w.y)),
                 vec3_add(vec3_scale(n2, w.z), vec3_scale(n3, w.w)));
  }

  attribute_set(&mesh->vertex.position_attr, mesh->skinned_position, arrlenu(mesh->skinned_position), GL_STREAM_DRAW);
  attribute_set(&mesh->vertex.normal_attr, mesh->skinned_normal, arrlenu(mesh->skinned_normal), GL_STREAM_DRAW);
}
