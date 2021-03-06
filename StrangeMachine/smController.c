#include "util/common.h"

#include "data/array.h"

#include "smCrossFadeTarget.h"
#include "smMem.h"
#include "smSkeleton.h"

// Corss-fade controller
typedef struct {
  cross_fade_target_s *targets;
  struct clip_s *clip;
  float time;
  pose_s pose;
  struct skeleton_s *skeleton;
  bool was_skeleton_set;

} controller_s;

void controller_skeleton_set(controller_s *controller, struct skeleton_s *skeleton);

// Allocate memory
controller_s *controller_new(void) {
  controller_s *controller = (controller_s *)SM_CALLOC(1, sizeof(controller_s));
  SM_ASSERT(controller != NULL);

  return controller;
}

// Constructor
bool controller_ctor(controller_s *controller, struct skeleton_s *skeleton) {

  SM_ASSERT(controller != NULL);
  SM_ASSERT(skeleton != NULL);

  controller_skeleton_set(controller, skeleton);
  return true;
}

// Destructor
void controller_dtor(controller_s *controller) {

  SM_ASSERT(controller != NULL);

  SM_ARRAY_DTOR(controller->targets);
  controller->targets = NULL;

  pose_dtor(&controller->pose);

  SM_FREE(controller);
  controller = NULL;
}

void controller_do(controller_s *controller, float dt) {

  SM_ASSERT(controller != NULL);

  if (controller->clip == NULL || !controller->was_skeleton_set)
    return;

  size_t num_targets = SM_ARRAY_SIZE(controller->targets);

  for (size_t i = 0; i < num_targets; ++i) {
    float duration = controller->targets[i].duration;

    if (controller->targets[i].elapsed >= duration) {
      controller->clip = controller->targets[i].clip;

      controller->time = controller->targets[i].time;

      pose_copy(&controller->pose, controller->targets[i].pose);

      // TODO: arr delete like erase cpp
      // arrdel(controller->targets, i);
      SM_ARRAY_DEL(controller->targets, i, 1);
      break;
    }
  }

  num_targets = SM_ARRAY_SIZE(controller->targets);
  pose_copy(&controller->pose, skeleton_get_rest_pose(controller->skeleton));
  controller->time = clip_sample(controller->clip, &controller->pose, controller->time + dt);

  for (size_t i = 0; i < num_targets; ++i) {
    cross_fade_target_s *target = &controller->targets[i];
    target->time = clip_sample(target->clip, target->pose, target->time + dt);
    target->elapsed += dt;
    float t = target->elapsed / target->duration;
    if (t > 1.0f) {
      t = 1.0f;
    }
    pose_blend(&controller->pose, &controller->pose, target->pose, t, -1);
  }
}

void controller_skeleton_set(controller_s *controller, struct skeleton_s *skeleton) {

  SM_ASSERT(controller != NULL);

  controller->skeleton = skeleton;

  if (controller->pose.nodes == NULL) {
    controller->pose = pose_new();
  }

  pose_copy(&controller->pose, skeleton_get_rest_pose(skeleton));
  controller->was_skeleton_set = true;
}

void controller_play(controller_s *controller, struct clip_s *target) {

  SM_ASSERT(controller != NULL);
  SM_ARRAY_SET_SIZE(controller->targets, 0);

  controller->clip = target;

  pose_copy(&controller->pose, skeleton_get_rest_pose(controller->skeleton));

  controller->time = clip_get_start_time(target);
}

void controller_fade_to(controller_s *controller, struct clip_s *target, float fade_time) {
  SM_ASSERT(controller != NULL);

  if (controller->clip == NULL) {
    controller_play(controller, target);
    return;
  }

  size_t targets = SM_ARRAY_SIZE(controller->targets);
  if (targets >= 1) {
    struct clip_s *clip = controller->targets[targets - 1].clip;

    if (clip == controller->clip) {
      return;
    }

  } else {
    if (controller->clip == target) {
      return;
    }
  }

  cross_fade_target_s cross_targ = cross_fade_target_new();
  if (!cross_fade_target_ctor(&cross_targ, target, skeleton_get_rest_pose(controller->skeleton), fade_time))
    return;

  SM_ARRAY_PUSH(controller->targets, cross_targ);
}

pose_s *controller_get_current_pose(controller_s *controller) {
  return &controller->pose;
}

struct clip_s *controller_get_current_clip(controller_s *controller) {
  return controller->clip;
}
