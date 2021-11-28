#include "model/clip.h"
#include "model/cross_fade_target.h"
#include "model/pose.h"
#include "model/skeleton.h"
#include "util/common.h"
#include "stb_ds/stb_ds.h"

// Corss-fade controller
typedef struct {
  cross_fade_target_s *targets;
  struct clip_s *clip;
  float time;
  pose_s pose;
  struct skeleton_s *skeleton;
  bool was_skeleton_set;

} controller_s;

void controller_skeleton_set(controller_s *controller,
                             struct skeleton_s *skeleton);

// Allocate memory
controller_s *controller_new(void) {
  controller_s *controller = (controller_s *)calloc(1, sizeof(controller_s));
  assert(controller != NULL);

  return controller;
}

// Constructor
status_v controller_ctor(controller_s *controller,
                         struct skeleton_s *skeleton) {

  assert(controller != NULL);
  assert(skeleton != NULL);

  controller_skeleton_set(controller, skeleton);
  return ok;
}

// Destructor
void controller_dtor(controller_s *controller) {

  assert(controller != NULL);

  arrfree(controller->targets);
  controller->targets = NULL;

  pose_dtor(&controller->pose);

  free(controller);
  controller = NULL;
}

void controller_do(controller_s *controller, float dt) {

  assert(controller != NULL);

  if (controller->clip == NULL || !controller->was_skeleton_set)
    return;

  size_t num_targets = arrlenu(controller->targets);

  for (size_t i = 0; i < num_targets; ++i) {
    float duration = controller->targets[i].duration;

    if (controller->targets[i].elapsed >= duration) {
      controller->clip = controller->targets[i].clip;

      controller->time = controller->targets[i].time;

      pose_copy(&controller->pose, controller->targets[i].pose);

      // TODO: arr delete like erase cpp
      // arrdel(controller->targets, i);
      arrdel(controller->targets, i);
      break;
    }
  }

  num_targets = arrlenu(controller->targets);
  pose_copy(&controller->pose, skeleton_get_rest_pose(controller->skeleton));
  controller->time =
      clip_sample(controller->clip, &controller->pose, controller->time + dt);

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

void controller_skeleton_set(controller_s *controller,
                             struct skeleton_s *skeleton) {

  assert(controller != NULL);

  controller->skeleton = skeleton;

  if (controller->pose.parents == NULL) {
    controller->pose = pose_new();
  }

  pose_copy(&controller->pose, skeleton_get_rest_pose(skeleton));
  controller->was_skeleton_set = true;
}

void controller_play(controller_s *controller, struct clip_s *target) {

  assert(controller != NULL);
  arrsetlen(controller->targets, 0);

  controller->clip = target;

  pose_copy(&controller->pose, skeleton_get_rest_pose(controller->skeleton));

  controller->time = clip_get_start_time(target);
}

void controller_fade_to(controller_s *controller, struct clip_s *target,
                        float fade_time) {
  assert(controller != NULL);

  if (controller->clip == NULL) {
    controller_play(controller, target);
    return;
  }

  size_t targets = arrlenu(controller->targets);
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
  if (!cross_fade_target_ctor(&cross_targ, target,
                              skeleton_get_rest_pose(controller->skeleton),
                              fade_time))
    return;

  arrput(controller->targets, cross_targ);
}

pose_s *controller_get_current_pose(controller_s *controller) {
  return &controller->pose;
}

struct clip_s *controller_get_current_clip(controller_s *controller) {
  return controller->clip;
}
