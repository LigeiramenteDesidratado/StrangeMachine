#ifndef SM_POSE_H
#define SM_POSE_H

#include "math/transform.h"
#include <stdint.h>

typedef struct {
  transform_s *joints;
  int32_t *parents;

} pose_s;

#define pose_new() ((pose_s){0})

// Destructor
void pose_dtor(pose_s *pose);

void pose_resize(pose_s *pose, size_t size);
void pose_copy(pose_s *dest, const pose_s *const src);
int32_t pose_get_parent(const pose_s *const pose, size_t index);
void pose_set_parent(pose_s *pose, uint32_t index, int parent);
transform_s pose_get_local_transform(const pose_s *const pose, uint32_t index);
void pose_set_local_transform(pose_s *pose, uint32_t index, transform_s transform);
transform_s pose_get_global_transform(const pose_s *const pose, uint32_t index);
void pose_get_matrix_palette(const pose_s *const pose, mat4 **out);
bool pose_is_equal(const pose_s *const a, const pose_s *const b);
bool pose_not_equal(const pose_s *const a, const pose_s *const b);
bool pose_is_in_hierarchy(const pose_s *const pose, uint32_t root, uint32_t search);
void pose_blend(pose_s *output, const pose_s *const a, const pose_s *const b, float t, int root);

#endif // SM_POSE_H
