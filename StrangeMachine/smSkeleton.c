#include "util/common.h"

#include "data/array.h"

#include "smMem.h"
#include "smPose.h"

typedef struct {
  pose_s rest_pose;
  pose_s bind_pose;

  mat4 *inverse_bind_pose;

  char **joint_names;

} skeleton_s;

void skeleton_update_inverse_bind_pose(skeleton_s *skeleton);
void skeleton_set(skeleton_s *skeleton, pose_s *rest, pose_s *bind, const char **const names);

// Allocate memory
skeleton_s *skeleton_new(void) {
  skeleton_s *skeleton = (skeleton_s *)SM_CALLOC(1, sizeof(skeleton_s));

  SM_ASSERT(skeleton != NULL);

  return skeleton;
}

// Constructor
bool skeleton_ctor(skeleton_s *skeleton, pose_s *rest, pose_s *bind, const char **const names) {

  SM_ASSERT(skeleton != NULL);

  skeleton->inverse_bind_pose = NULL;
  skeleton->joint_names = (char **)SM_ARRAY_NEW_EMPTY();
  skeleton_set(skeleton, rest, bind, names);

  return true;
}

// Destructor
void skeleton_dtor(skeleton_s *skeleton) {
  SM_ASSERT(skeleton != NULL);

  SM_ARRAY_DTOR(skeleton->joint_names);
  SM_ALIGNED_ARRAY_DTOR(skeleton->inverse_bind_pose);

  pose_dtor(&skeleton->bind_pose);
  pose_dtor(&skeleton->rest_pose);

  SM_FREE(skeleton);
  skeleton = NULL;
}

void skeleton_set(skeleton_s *skeleton, pose_s *rest, pose_s *bind, const char **const names) {
  SM_ASSERT(skeleton != NULL);

  // TODO: investigate this
  if (skeleton->rest_pose.nodes == NULL)
    skeleton->rest_pose = pose_new();

  if (skeleton->bind_pose.nodes == NULL)
    skeleton->bind_pose = pose_new();

  pose_copy(&skeleton->rest_pose, rest);
  pose_copy(&skeleton->bind_pose, bind);

  size_t length = SM_ARRAY_SIZE(names);
  if (SM_ARRAY_SIZE(skeleton->joint_names) > 0)
    SM_ARRAY_SET_SIZE(skeleton->joint_names, 0);

  SM_ARRAY_SET_CAPACITY(skeleton->joint_names, length);
  for (size_t i = 0; i < length; ++i) {
    SM_ARRAY_PUSH(skeleton->joint_names, names[i]);
  }

  skeleton_update_inverse_bind_pose(skeleton);
}

void skeleton_update_inverse_bind_pose(skeleton_s *skeleton) {

  SM_ASSERT(skeleton != NULL);

  size_t size = SM_ARRAY_SIZE(skeleton->bind_pose.nodes);
  // is it safe to assume that the size will always be the same?
  // TODO: investigate this
  if (skeleton->inverse_bind_pose == NULL) {
    SM_ALIGNED_ARRAY_NEW(skeleton->inverse_bind_pose, 16, size);
  }
  SM_ASSERT(skeleton->inverse_bind_pose != NULL);

  for (size_t i = 0; i < size; ++i) {
    transform_s world = pose_get_global_transform(&skeleton->bind_pose, i);

    mat4 m;
    transform_to_mat4(world, m);
    glm_mat4_inv(m, skeleton->inverse_bind_pose[i]);
  }
}

pose_s *skeleton_get_bind_pose(skeleton_s *const skeleton) {
  SM_ASSERT(skeleton != NULL);

  return &skeleton->bind_pose;
}

pose_s *skeleton_get_rest_pose(skeleton_s *const skeleton) {
  SM_ASSERT(skeleton != NULL);

  return &skeleton->rest_pose;
}

mat4 **skeleton_get_inverse_bind_pose(skeleton_s *const skeleton) {
  SM_ASSERT(skeleton != NULL);

  return &skeleton->inverse_bind_pose;
}

char **skeleton_get_joint_names(const skeleton_s *const skeleton) {
  SM_ASSERT(skeleton != NULL);

  return skeleton->joint_names;
}

char *skeleton_get_joint_name(const skeleton_s *const skeleton, uint32_t index) {
  SM_ASSERT(skeleton != NULL);
  return skeleton->joint_names[index];
}
