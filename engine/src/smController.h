#ifndef SM_CONTROLLER_H
#define SM_CONTROLLER_H

#include "smClip.h"
#include "smSkeleton.h"

struct controller_s;

// Allocate memory
struct controller_s *controller_new(void);

// Constructor
status_v controller_ctor(struct controller_s *controller,
                         struct skeleton_s *skeleton);

// Destructor
void controller_dtor(struct controller_s *controller);

// Forward declaration
void controller_do(struct controller_s *controller, float dt);

void controller_skeleton_set(struct controller_s *controller,
                             struct skeleton_s *skeleton);

struct clip_s *controller_get_current_clip(struct controller_s *controller);

pose_s *controller_get_current_pose(struct controller_s *controller);

void controller_fade_to(struct controller_s *controller, struct clip_s *target,
                        float fade_time);

void controller_play(struct controller_s *controller, struct clip_s *target);

void controller_skeleton_set(struct controller_s *controller,
                             struct skeleton_s *skeleton);

#endif // SM_CONTROLLER_H
