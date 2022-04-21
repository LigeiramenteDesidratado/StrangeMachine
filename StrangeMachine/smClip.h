#ifndef SM_CLIP_H
#define SM_CLIP_H

#include <stdbool.h>
#include <stdint.h>

#include "smPose.h"
#include "smTransformTrack.h"

struct clip_s;

// Allocate memory
struct clip_s *clip_new(void);

// Constructor
bool clip_ctor(struct clip_s *clip, const char *name);

// Destructor
void clip_dtor(struct clip_s *clip);

uint32_t clip_get_id_at_index(const struct clip_s *const clip, uint32_t index);
void clip_set_id_at_index(struct clip_s *clip, uint32_t index, uint32_t id);
size_t clip_get_size(const struct clip_s *const clip);
float clip_sample(const struct clip_s *const clip, pose_s *pose, float t);
void clip_recalculate_duration(struct clip_s *clip);
transform_track_s *clip_get_transform_track_from_joint(struct clip_s *clip, uint32_t joint);
char *clip_get_name(const struct clip_s *const clip);
void clip_resize_tracks(struct clip_s *clip, size_t size);
float clip_get_duration(const struct clip_s *const clip);
float clip_get_start_time(const struct clip_s *const clip);
float clip_get_end_time(const struct clip_s *const clip);
bool clip_get_looping(const struct clip_s *const clip);
void clip_set_looping(struct clip_s *clip, bool looping);
void clip_set_cap_tracks(struct clip_s *clip, size_t value);

#endif // SM_CLIP_H
