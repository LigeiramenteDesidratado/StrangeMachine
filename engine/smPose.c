#include "smPose.h"
#include "smMem.h"
#include "util/common.h"

// Destructor
void pose_dtor(pose_s *pose) {
  assert(pose != NULL);

  SM_ARRAY_DTOR(pose->nodes);
}

void pose_resize(pose_s *pose, size_t size) {

  assert(pose != NULL);

  size_t nodes_old_size = SM_ARRAY_SIZE(pose->nodes);
  SM_ARRAY_SET_SIZE(pose->nodes, size);
  size_t nodes_new_size = SM_ARRAY_SIZE(pose->nodes);

  for (size_t i = 0; i < (nodes_new_size - nodes_old_size); ++i) {
    pose->nodes[nodes_old_size + i].joint = transform_zero();
    pose->nodes[nodes_old_size + i].parent = 0;
  }
}

int32_t pose_get_parent(const pose_s *const pose, size_t index) {
  assert(pose != NULL);

  assert(index < SM_ARRAY_SIZE(pose->nodes));
  return pose->nodes[index].parent;
}

void pose_set_parent(pose_s *pose, uint32_t index, int parent) {
  assert(pose != NULL);
  assert(index < SM_ARRAY_SIZE(pose->nodes));

  pose->nodes[index].parent = parent;
}

transform_s pose_get_local_transform(const pose_s *const pose, uint32_t index) {
  assert(pose != NULL);
  assert(index < SM_ARRAY_SIZE(pose->nodes));
  return pose->nodes[index].joint;
}

void pose_set_local_transform(pose_s *pose, uint32_t index, transform_s transform) {
  assert(pose != NULL);
  assert(index < SM_ARRAY_SIZE(pose->nodes));

  pose->nodes[index].joint = transform;
}

transform_s pose_get_global_transform(const pose_s *const pose, uint32_t index) {
  assert(pose != NULL);
  assert(index < SM_ARRAY_SIZE(pose->nodes));

  transform_s result = pose->nodes[index].joint;
  for (int32_t p = pose->nodes[index].parent; p >= 0; p = pose->nodes[p].parent) {
    result = transform_combine(pose->nodes[p].joint, result);
  }
  return result;
}

#if 0
void pose_get_matrix_palette(pose_t *pose, mat4 **out, unsigned int length) {

  unsigned int size = SM_ARRAY_SIZE(pose->joints);
  if (length != size) {
    SM_ARRAY_SET_SIZE(*out, size);
  }

  for (unsigned int i = 0; i < size; ++i) {
    transform_t t = pose_get_global_transform(pose, i);
    (*out)[i] = transform_to_mat4(t);
  }
}
#else

void pose_get_matrix_palette(const pose_s *const pose, mat4 **out) {
  int32_t size = (int32_t)SM_ARRAY_SIZE(pose->nodes);
  int32_t length = (int32_t)SM_ARRAY_SIZE((*out));

  if (length != size) {
    SM_ARRAY_SET_SIZE(*out, (size_t)size);
    for (size_t i = 0; i < (SM_ARRAY_SIZE((*out)) - length); ++i) {
      (*out)[length + i] = mat4_identity();
    }
  }

  int32_t i = 0;
  for (; i < size; ++i) {
    int32_t parent = pose->nodes[i].parent;
    // this breaks ascending order, so the previous calculated results cannot be
    // used
    if (parent > i) {
      break;
    }

    mat4 global = transform_to_mat4(pose->nodes[i].joint);
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

  if (SM_ARRAY_SIZE(a->nodes) != SM_ARRAY_SIZE(b->nodes)) {
    return false;
  }

  size_t size = SM_ARRAY_SIZE(a->nodes);
  for (size_t i = 0; i < size; ++i) {
    transform_s a_local = a->nodes[i].joint;
    transform_s b_local = b->nodes[i].joint;

    int32_t a_parent = a->nodes[i].parent;
    int32_t b_parent = b->nodes[i].parent;

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

  size_t size = SM_ARRAY_SIZE(src->nodes);
  pose_resize(dest, size);

  if (size != 0)
    memcpy(dest->nodes, src->nodes, sizeof(node_s) * size);
}

// returns true if the search node is a descendant of the given root node
bool pose_is_in_hierarchy(const pose_s *const pose, uint32_t root, uint32_t search) {
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

void pose_blend(pose_s *output, const pose_s *const a, const pose_s *const b, float t, int root) {
  size_t num_joints = SM_ARRAY_SIZE(output->nodes);
  for (size_t i = 0; i < num_joints; ++i) {
    if (root >= 0) {
      // don't blend if they aren't within the same hierarchy
      if (!pose_is_in_hierarchy(output, root, i)) {
        continue;
      }
    }
    transform_s mix = transform_mix(pose_get_local_transform(a, i), pose_get_local_transform(b, i), t);
    pose_set_local_transform(output, i, mix);
    // output.SetLocalTransform(i,
    // mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
  }
}
