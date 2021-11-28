#ifndef TRACK_H
#define TRACK_H

#include "model/interpolation.h"
#include "util/common.h"

typedef unsigned char TRACK_KIND_EX2;
#define SCALAR_EX2 ((TRACK_KIND_EX2)0x01)
#define VECTOR_EX2 ((TRACK_KIND_EX2)0x02)
#define QUATERNION_EX2 ((TRACK_KIND_EX2)0x03)

// The frame_s struct is used to store keyframes in a animation track. An
// animation track is a collection of keyframes.
typedef struct {
  float *value;
  float *in;
  float *out;
  float t;

} frame_s;

#define frame_zero() ((frame_s){0})

typedef struct {

  frame_s *frames;

  uint32_t *sampled_frames;

  INTERPOLATION_EX3 interpolation;

} track_s;

#define track_new() ((track_s){0, .interpolation = LINEAR_EX3})

// Destructor
void track_dtor(track_s *track);
float track_get_start_time(const track_s *const track);
float track_get_end_time(const track_s *const track);

float track_sample_float(track_s *track, float time, bool looping);
vec3 track_sample_vec3(track_s *track, float time, bool looping);
quat track_sample_quat(track_s *track, float time, bool looping);

void track_resize_sampled_frames(track_s *track, size_t size);
void track_resize_frame(track_s *track, size_t size);
void track_resize_frame_in(track_s *track, size_t size, uint32_t frame_index);
void track_resize_frame_out(track_s *track, size_t size, uint32_t frame_index);
void track_resize_frame_value(track_s *track, size_t size, uint32_t frame_index);
void track_index_look_up_table(track_s *track);

int32_t track_frame_index(track_s *track, float time, bool looping);
#endif // TRACK_H
