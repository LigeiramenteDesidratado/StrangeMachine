#ifndef SM_CROSS_FADE_TARGET_H
#define SM_CROSS_FADE_TARGET_H

#include "smClip.h"
#include "smPose.h"

typedef struct {
  struct clip_s *clip;
  pose_s *pose;
  float time;
  float duration;
  float elapsed;

} cross_fade_target_s;

#define cross_fade_target_new() ((cross_fade_target_s){0})

bool cross_fade_target_ctor(cross_fade_target_s *cross_fade_target, struct clip_s *target, pose_s *pose, float duration);

#endif // SM_CROSS_FADE_TARGET_H
