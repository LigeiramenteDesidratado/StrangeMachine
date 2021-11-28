#ifndef CROSS_FADE_TARGET_H
#define CROSS_FADE_TARGET_H

#include "model/clip.h"
#include "model/pose.h"
#include "util/common.h"

typedef struct {
  struct clip_s *clip;
  pose_s *pose;
  float time;
  float duration;
  float elapsed;

} cross_fade_target_s;

#define cross_fade_target_new() ((cross_fade_target_s){0})

status_v cross_fade_target_ctor(cross_fade_target_s *cross_fade_target,
                                struct clip_s *target, pose_s *pose,
                                float duration);

#endif // CROSS_FADE_TARGET_H
