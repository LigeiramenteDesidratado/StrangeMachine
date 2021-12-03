
#include "smCrossFadeTarget.h"
#include "util/common.h"

// Constructor
status_v cross_fade_target_ctor(cross_fade_target_s *cross_fade_target,
                                struct clip_s *target, pose_s *pose,
                                float duration) {

  assert(cross_fade_target != NULL);
  cross_fade_target->clip = target;
  cross_fade_target->time = clip_get_start_time(target);
  cross_fade_target->pose = pose;
  cross_fade_target->duration = duration;
  cross_fade_target->elapsed = 0.0f;

  return true;
}

struct clip_s *
cross_fade_target_get_clip(cross_fade_target_s *cross_fade_target) {
  assert(cross_fade_target != NULL);

  return cross_fade_target->clip;
}

float cross_fade_target_get_duration(cross_fade_target_s *cross_fade_target) {
  assert(cross_fade_target != NULL);

  return cross_fade_target->duration;
}
float cross_fade_target_get_elapsed(cross_fade_target_s *cross_fade_target) {
  assert(cross_fade_target != NULL);

  return cross_fade_target->elapsed;
}
float cross_fade_target_get_time(cross_fade_target_s *cross_fade_target) {
  assert(cross_fade_target != NULL);

  return cross_fade_target->time;
}
pose_s *cross_fade_target_get_pose(cross_fade_target_s *cross_fade_target) {
  assert(cross_fade_target != NULL);

  return cross_fade_target->pose;
}
