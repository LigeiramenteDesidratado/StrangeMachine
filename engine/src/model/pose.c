#include "util/common.h"
#include "stb_ds/stb_ds.h"
#include "model/pose.h"

// Destructor
void pose_dtor(pose_s *pose) {
  assert(pose != NULL);

  arrfree(pose->joints);
  arrfree(pose->parents);
}

void pose_resize(pose_s *pose, size_t size) {

  assert(pose != NULL);

  size_t joint_old_size = arrlenu(pose->joints);
  arrsetlen(pose->joints, size);
  size_t joint_new_size = arrlenu(pose->joints);
  for (size_t i = 0; i < (joint_new_size - joint_old_size); ++i) {
    pose->joints[joint_old_size + i] = transform_zero();
  }

  size_t parent_old_size = arrlenu(pose->parents);
  arrsetlen(pose->parents, size);
  size_t parent_new_size = arrlenu(pose->parents);
  for (size_t i = 0; i < (parent_new_size - parent_old_size); ++i) {
    pose->parents[parent_old_size + i] = 0;
  }
}

int32_t pose_get_parent(const pose_s *const pose, uint32_t index) {
  assert(pose != NULL);
  assert(index < arrlenu(pose->joints));
  return pose->parents[index];
}

void pose_set_parent(pose_s *pose, uint32_t index, int parent) {
  assert(pose != NULL);
  assert(index < arrlenu(pose->joints));
  pose->parents[index] = parent;
}

transform_s pose_get_local_transform(const pose_s *const pose, uint32_t index) {
  assert(pose != NULL);
  assert(index < arrlenu(pose->joints));
  return pose->joints[index];
}

void pose_set_local_transform(pose_s *pose, uint32_t index,
                              transform_s transform) {
  assert(pose != NULL);
  assert(index < arrlenu(pose->joints));

  pose->joints[index] = transform;
}

transform_s pose_get_global_transform(const pose_s *const pose, uint32_t index) {
  assert(pose != NULL);
  assert(index < arrlenu(pose->joints));

  transform_s result = pose->joints[index];
  for (int32_t p = pose->parents[index]; p >= 0; p = pose->parents[p]) {
    result = transform_combine(pose->joints[p], result);
  }
  return result;
}

#if 0
void pose_get_matrix_palette(pose_t *pose, mat4 **out, unsigned int length) {

  unsigned int size = arrlenu(pose->joints);
  if (length != size) {
    arrsetlen(*out, size);
  }

  for (unsigned int i = 0; i < size; ++i) {
    transform_t t = pose_get_global_transform(pose, i);
    (*out)[i] = transform_to_mat4(t);
  }
}
#else
void pose_get_matrix_palette(const pose_s *const pose, mat4 **out) {
  int32_t size = (int32_t)arrlenu(pose->joints);
  int32_t length = arrlenu((*out));

  if (length != size) {
    arrsetlen(*out, size);
    for (size_t i = 0; i < (arrlenu((*out)) - length); ++i) {
      (*out)[length + i] = mat4_identity();
    }
  }

  int32_t i = 0;
  for (; i < size; ++i) {
    int32_t parent = pose->parents[i];
    // this breaks ascending order, so the previous calculated results cannot be
    // used
    if (parent > i) {
      break;
    }

    mat4 global = transform_to_mat4(pose->joints[i]);
    if (parent >= 0) {
      global = mat4_mul((*out)[parent], global);
    }
    (*out)[i] = global;
  }

  for (; i < size; ++i) {
    transform_s t = pose_get_global_transform(pose, i);
    (*out)[i] = transform_to_mat4(t);
  }
}
#endif

bool pose_is_equal(const pose_s *const a, const pose_s *const b) {

  if (a == NULL || b == NULL)
    return false;

  if (arrlenu(a->joints) != arrlenu(b->joints)) {
    return false;
  }

  size_t size = arrlenu(a->joints);
  for (size_t i = 0; i < size; ++i) {
    transform_s a_local = a->joints[i];
    transform_s b_local = b->joints[i];

    int32_t a_parent = a->parents[i];
    int32_t b_parent = b->parents[i];

    if (a_parent != b_parent)
      return false;

    if (vec3_not_equal(a_local.position, b_local.position))
      return false;

    if (quat_not_equal(a_local.rotation, b_local.rotation))
      return false;

    if (vec3_not_equal(a_local.scale, b_local.scale))
      return false;
  }

  return true;
}

bool pose_not_equal(const pose_s *const a, const pose_s *const b) {
  return !pose_is_equal(a, b);
}

void pose_copy(pose_s *dest, const pose_s *const src) {

  assert((src != dest) || pose_not_equal(src, dest));

  size_t size = arrlenu(src->joints);
  pose_resize(dest, size);
  // arrsetlen(to->parents, arrlenu(from->parents));
  // arrsetlen(to->joints, arrlenu(from->joints));

  if (size != 0) {
    memcpy(dest->parents, src->parents, sizeof(int32_t) * size);
    memcpy(dest->joints, src->joints, sizeof(transform_s) * size);
  }
}

// returns true if the search node is a descendant of the given root node
bool pose_is_in_hierarchy(const pose_s *const pose, uint32_t root,
                          uint32_t search) {
  if (search == root)
    return true;

  int32_t p = pose_get_parent(pose, search);
  while (p >= 0) {
    if (p == (int32_t)root) {
      return true;
    }
    p = pose_get_parent(pose, p);
  }

  return false;
}

void pose_blend(pose_s *output, const pose_s *const a, const pose_s *const b,
                float t, int root) {
  size_t num_joints = arrlenu(output->joints);
  for (size_t i = 0; i < num_joints; ++i) {
    if (root >= 0) {
      // don't blend if they aren't within the same hierarchy
      if (!pose_is_in_hierarchy(output, root, i)) {
        continue;
      }
    }
    transform_s mix = transform_mix(pose_get_local_transform(a, i),
                                    pose_get_local_transform(b, i), t);
    pose_set_local_transform(output, i, mix);
    // output.SetLocalTransform(i,
    // mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
  }
}
