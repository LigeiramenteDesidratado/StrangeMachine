#ifndef TRANSFORM_TRACK_H
#define TRANSFORM_TRACK_H

#include "model/track.h"
#include "util/common.h"

typedef struct {
  uint32_t id;
  track_s position;
  track_s rotation;
  track_s scale;

} transform_track_s;

#define transform_track_new()                                                  \
  ((transform_track_s){.id = 0,                                                \
                       .position = track_new(),                                \
                       .rotation = track_new(),                                \
                       .scale = track_new()})

void transform_track_dtor(transform_track_s *transform_track);
bool transform_track_is_valid(const transform_track_s *const transform_track);
float transform_track_get_start_time(const transform_track_s * const transform_track);
float transform_track_get_end_time(const transform_track_s * const transform_track);
transform_s transform_track_sample(transform_track_s *transform_track, transform_s *transform_ref, float time, bool looping);


#endif // TRANSFORM_TRACK_H
