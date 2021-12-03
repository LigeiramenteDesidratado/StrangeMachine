#ifndef SM_SKINNED_MESH_H
#define SM_SKINNED_MESH_H

#include "util/common.h"
#include "smVertex.h"
#include "smIndexBuffer.h"

// NOTE: in case of change make sure to update the src/shader/glsl/static.vs as
// well
struct skinned_mesh_attrs {
  const uint8_t position;
  const uint8_t tex_coord;
  const uint8_t normal;
  const uint8_t weight;
  const uint8_t joint;
};
extern const struct skinned_mesh_attrs skinned_mesh_attr_locs;

// Start declaring the Mesh class. It should maintain a copy of the mesh data on
// the CPU as well as on the GPU. Store vectors for the position, normal, tex
// coordinates, weights, and influences that define each vertex. Include an
// optional vector for indices
typedef struct {
  // vertex information
  vertex_s vertex;

  // extra vertex information
  vec4 *weights;

  ivec4 *influences;

  uint32_t *indices;

  // Each list in the preceding code also needs to be set appropriate
  // attributes.
  attribute_s weight_attr;    // VEC4_K
  attribute_s influence_attr; // IVEC4_K
  index_buffer_s index_buffer;

  // Add an additional copy of the pose and normal data, as well as a matrix
  // palette to use for CPU skinning.
  vec3 *skinned_position;
  vec3 *skinned_normal;
  mat4 *pose_palette;

} skinned_mesh_s;

#define skinned_mesh_new() ((skinned_mesh_s) {0})

// Constructor
status_v skinned_mesh_ctor(skinned_mesh_s *mesh);

// Destructor
void skinned_mesh_dtor(skinned_mesh_s *mesh);


void skinned_mesh_copy(skinned_mesh_s *dest, const skinned_mesh_s * const src);
void skinned_mesh_update_gl_buffers(skinned_mesh_s *mesh);
void skinned_mesh_bind(const skinned_mesh_s * const mesh, uint8_t flag);
void skinned_mesh_unbind(const skinned_mesh_s * const mesh, uint8_t flag);
void skinned_mesh_CPU_skin_matrix_opt(skinned_mesh_s *mesh, mat4 *animated_pose);


#endif // SM_SKINNED_MESH_H

