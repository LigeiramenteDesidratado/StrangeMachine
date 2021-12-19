#ifndef SM_SKELETON_H
#define SM_SKELETON_H

#include <stdbool.h>

#include "smPose.h"

struct skeleton_s;

// Allocate memory
struct skeleton_s *skeleton_new(void);

// Constructor
bool skeleton_ctor(struct skeleton_s *skeleton, pose_s *rest, pose_s *bind, const char **const names);

// Destructor
void skeleton_dtor(struct skeleton_s *skeleton);

// Forward declaration

void skeleton_set(struct skeleton_s *skeleton, pose_s *rest, pose_s *bind, const char **const names);
void skeleton_update_inverse_bind_pose(struct skeleton_s *skeleton);
pose_s *skeleton_get_bind_pose(struct skeleton_s *const skeleton);
pose_s *skeleton_get_rest_pose(struct skeleton_s *const skeleton);
mat4 **skeleton_get_inverse_bind_pose(struct skeleton_s *const skeleton);
char **skeleton_get_joint_names(const struct skeleton_s *const skeleton);
char *skeleton_get_joint_name(const struct skeleton_s *const skeleton, uint32_t index);

#endif // SM_SKELETON_H
