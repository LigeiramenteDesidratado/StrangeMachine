#include "core/smBase.h"
#include "data/array.h"

#include "util/common.h"

#include "smTrack.h"

typedef enum { SCALAR_TRACK_KIND = 0x01, VECTOR_TRACK_KIND = 0x02, QUATERNION_TRACK_KIND = 0x04 } track_kind_e;

static void __track_sample_constant_vec3(track_s *track, float t, bool looping, vec3 out);
static void __track_sample_cubic_vec3(track_s *track, float time, bool looping, vec3 out);
static void __track_sample_linear_vec3(track_s *track, float time, bool looping, vec3 out);

static float __track_sample_constant_float(track_s *track, float t, bool looping);
static float __track_sample_cubic_float(track_s *track, float time, bool looping);
static float __track_sample_linear_float(track_s *track, float time, bool looping);

static void __track_sample_constant_quat(track_s *track, float t, bool looping, versor out);
static void __track_sample_cubic_quat(track_s *track, float time, bool looping, versor out);
static void __track_sample_linear_quat(track_s *track, float time, bool looping, versor out);

static void __track_hermite_vec3(float t, vec3 p1, vec3 s1, vec3 _p2, vec3 s2, vec3 out);
static float __track_hermite_float(float t, float p1, float s1, float _p2, float s2);
static void __track_hermite_quat(float t, versor p1, versor s1, versor _p2, versor s2, versor out);

void __track_neighborhood(versor a, versor b);
void __track_adjust_hermite_result(versor q, versor out);
static void quat_iterpolate(versor a, versor b, float t, versor out);
void track_index_look_up_table(track_s *track);

// Destructor
void track_dtor(track_s *track) {
  assert(track != NULL);

  for (size_t i = 0; i < SM_ARRAY_SIZE(track->frames); ++i) {
    SM_ARRAY_DTOR(track->frames[i].value);
    SM_ARRAY_DTOR(track->frames[i].in);
    SM_ARRAY_DTOR(track->frames[i].out);
  }

  SM_ARRAY_DTOR(track->frames);
  SM_ARRAY_DTOR(track->sampled_frames);
}

float track_get_start_time(const track_s *const track) {

  assert(track != NULL);

  return track->frames[0].t;
}

float track_get_end_time(const track_s *const track) {

  assert(track != NULL);

  return track->frames[SM_ARRAY_SIZE(track->frames) - 1].t;
}

float track_sample_float(track_s *track, float time, bool looping) {
  switch (track->interpolation) {
  case CONSTANT_INTERP:
    return __track_sample_constant_float(track, time, looping);
    break;
  case CUBIC_INTERP:
    return __track_sample_cubic_float(track, time, looping);
    break;
  case LINEAR_INTERP:
    return __track_sample_linear_float(track, time, looping);
    break;
  default:
    log_warn("unkown iterpolation");
  }
  return 0.0f;
}

void track_sample_vec3(track_s *track, float time, bool looping, vec3 out) {


  switch (track->interpolation) {
  case CONSTANT_INTERP:
    __track_sample_constant_vec3(track, time, looping, out);
    break;
  case CUBIC_INTERP:
    __track_sample_cubic_vec3(track, time, looping, out);
    break;
  case LINEAR_INTERP:
    __track_sample_linear_vec3(track, time, looping, out);
    break;
  default:
    SM_LOG_WARN("unkown iterpolation");
  }
}

void track_sample_quat(track_s *track, float time, bool looping, versor out) {
  switch (track->interpolation) {
  case CONSTANT_INTERP:
    __track_sample_constant_quat(track, time, looping, out);
    break;
  case CUBIC_INTERP:
    __track_sample_cubic_quat(track, time, looping, out);
    break;
  case LINEAR_INTERP:
    __track_sample_linear_quat(track, time, looping, out);
    break;
  default:
    SM_LOG_WARN("unkown iterpolation");
  }
}

void track_resize_sampled_frames(track_s *track, size_t size) {
  assert(track != NULL);

  size_t old_length = SM_ARRAY_SIZE(track->sampled_frames);
  SM_ARRAY_SET_SIZE(track->sampled_frames, size);
  size_t new_length = SM_ARRAY_SIZE(track->sampled_frames);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->sampled_frames[old_length + i] = 0;
  }
}

// changed from track_resize --> track_resize_frame;
void track_resize_frame(track_s *track, size_t size) {

  assert(track != NULL);

  size_t old_length = SM_ARRAY_SIZE(track->frames);
  SM_ARRAY_SET_SIZE(track->frames, size);
  size_t new_length = SM_ARRAY_SIZE(track->frames);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[old_length + i] = frame_zero();
  }
}

void track_resize_frame_in(track_s *track, size_t size, uint32_t frame_index) {

  assert(track != NULL);
  if (frame_index > SM_ARRAY_SIZE(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = SM_ARRAY_SIZE(track->frames[frame_index].in);
  SM_ARRAY_SET_SIZE(track->frames[frame_index].in, size);
  size_t new_length = SM_ARRAY_SIZE(track->frames[frame_index].in);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].in[old_length + i] = 0.0f;
  }
}

void track_resize_frame_out(track_s *track, size_t size, uint32_t frame_index) {

  assert(track != NULL);
  if (frame_index >= SM_ARRAY_SIZE(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = SM_ARRAY_SIZE(track->frames[frame_index].out);
  SM_ARRAY_SET_SIZE(track->frames[frame_index].out, size);
  size_t new_length = SM_ARRAY_SIZE(track->frames[frame_index].out);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].out[old_length + i] = 0.0f;
  }
}

void track_resize_frame_value(track_s *track, size_t size, uint32_t frame_index) {
  assert(track != NULL);
  if (frame_index > SM_ARRAY_SIZE(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = SM_ARRAY_SIZE(track->frames[frame_index].value);
  SM_ARRAY_SET_SIZE(track->frames[frame_index].value, size);
  size_t new_length = SM_ARRAY_SIZE(track->frames[frame_index].value);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].value[old_length + i] = 0.0f;
  }
}

size_t track_get_frame_size(track_s *track) {

  assert(track != NULL);

  return SM_ARRAY_SIZE(track->frames);
}

interpolation_e track_get_interpolation(track_s *track) {

  assert(track != NULL);

  return track->interpolation;
}

void track_set_interpolation(track_s *track, interpolation_e interpolation) {
  assert(track != NULL);

  track->interpolation = interpolation;
}

#if 0
// The track_frame_index function takes time as an argument; it should return
// the frame immediately before that time (on the left). This behavior changes
// depending on whether the track is intended to be sampled looping or not.
int track_frame_index(track_t *track, float t, bool looping) {
  // If the track has one frame or less, it is invalid. If an invalid track is
  // encountered,
  unsigned int size = SM_ARRAY_SIZE(track->frames);
  if (size < 1)
    return -1;

  float start_time = track->frames[0].t;
  float end_time = track->frames[size - 1].t;

  // If the track is sampled as looping, the input time needs to be adjusted so
  // that it falls between the start and end frames. This means you need to know
  // the time at the first frame of the track, the time at the frame of the
  // track, and the duration of the track:
  if (looping) {

    // Since the track is looping, time needs to be adjusted so that it is
    // within a valid range.
    t = fmodf(t - start_time, end_time - start_time);
    if (t < 0.0f)
      t += end_time - start_time;

    t = t + start_time;
  } else {

    // If the track doesn't loop, any time value that is less than the start
    // frame should clamp to 0 and any time value that is greater than the
    // second-to-last frame should clamp to the second-to-last frame's index.
    if (t <= start_time) {
      return 0;
    }
    if (t >= track->frames[size - 2].t)
      return size - 2;
  }

  // Now that the time is in a valid range, loop through every frame. The frame
  // that is closest to the time (but still less) is the frame whose index
  // should be returned
  for (int i = (int)size - 1; i >= 0; --i) {
    if (t >= track->frames[i].t)
      return i;
  }

  // Invalid code, we shoud not reach here11
  return -1;
}

#else

int32_t track_frame_index(track_s *track, float time, bool looping) {

  size_t size = SM_ARRAY_SIZE(track->frames);
  if (size <= 1) {
    log_debug("returning -1");
    return -1;
  }

  if (looping) {
    float start_time = track->frames[0].t;
    float end_time = track->frames[size - 1].t;
    float duration = end_time - start_time;

    time = fmodf(time - start_time, duration);
    if (time < 0.0f) {
      time += duration;
    }
    time = time + start_time;
  } else {
    if (time <= track->frames[0].t) {
      return 0;
    }
    if (time >= track->frames[size - 2].t) {
      return (int32_t)size - 2;
    }
  }

  float duration = track_get_end_time(track) - track_get_start_time(track);
  float t = time / duration;
  float num_samples = (duration * 60.0f);
  uint32_t index = (uint32_t)(t * num_samples);

  if (index >= SM_ARRAY_SIZE(track->sampled_frames)) {
    /* log_debug("index (%d) is gt array length (%lu)", index, */
    /* SM_ARRAY_SIZE(track->sampled_frames)); */
    return (int32_t)track->sampled_frames[SM_ARRAY_SIZE(track->sampled_frames) - 1];
    // return -1;
  }

  return (int32_t)track->sampled_frames[index];
}

#endif

float track_adjust_time(track_s *track, float t, bool looping) {

  // If a track has less than one frame, the track is invalid. If an invalid
  // track is used, retun 0
  size_t size = SM_ARRAY_SIZE(track->frames);
  if (size <= 1)
    return 0.0f;

  // Find the start time, end time, and duration of the track. The start time is
  // the time of the first frame, the end time is the time of the last frame,
  // and the duration is the difference between the two. If the track has a 0
  // duration, it is invalid—return 0
  float start_time = track->frames[0].t;
  float end_time = track->frames[size - 1].t;
  float duration = end_time - start_time;

  if (duration <= 0.0f) {
    return 0.0f;
  }

  if (looping) {
    // If the track loops, adjust the time by the duration of the track
    t = fmodf(t - start_time, end_time - start_time);
    if (t < 0.0f) {
      t += end_time - start_time;
    }
    t = t + start_time;
  } else {
    // If the track does not loop, clamp the time to the first or last frame.
    // Return the adjusted time
    if (t <= start_time)
      t = start_time;

    if (t >= end_time)
      t = end_time;
  }

  return t;
}

float track_cast_float(float *value) {
  return value[0];
}

void track_cast_vec3(float *value, vec3 out) {
  glm_vec3_copy(value, out);
}

void track_cast_quat(float *value, versor out) {

  /* glm_quat_copy(value, out); */
  out[0] = value[0];
  out[1] = value[1];
  out[2] = value[2];
  out[3] = value[3];

  glm_quat_normalize(out);
}

// Constant sampling is often used for things such as visibility flags, where it
// makes sense for the value of a variable to change from one frame to the next
// without any real interpolation
static float __track_sample_constant_float(track_s *track, float t, bool looping) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= (int32_t)SM_ARRAY_SIZE(track->frames)) {
    return 0.0f;
  }

  return track->frames[frame].value[0];
}

static float __track_sample_linear_float(track_s *track, float t, bool looping) {
  int32_t this_frame = track_frame_index(track, t, looping);
  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1))
    return 0.0f;

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, t, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return 0.0f;
  }

  float time = (track_time - this_time) / frame_delta;

  float start = track->frames[this_frame].value[0];
  float end = track->frames[next_frame].value[0];

  return glm_lerp(start, end, time);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static float __track_sample_cubic_float(track_s *track, float time, bool looping) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1))
    return 0.0f;

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return 0.0f;
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  float point1 = track->frames[this_frame].value[0];
  float slope1; // track_cast_float(&frame_get_out(track->frames[this_frame])) *
                // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, SCALAR_TRACK_KIND * flt_size);
  slope1 = slope1 * frame_delta;

  float point2 = track->frames[next_frame].value[0];
  float slope2; // track_cast_float(&frame_get_out(track->frames[next_frame])) *
                // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, SCALAR_TRACK_KIND * flt_size);
  slope2 = slope2 * frame_delta;

  return __track_hermite_float(t, point1, slope1, point2, slope2);
}

static void __track_sample_constant_vec3(track_s *track, float t, bool looping, vec3 out) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= (int32_t)SM_ARRAY_SIZE(track->frames)) {
    glm_vec3_zero(out);
    return;
  }

  track_cast_vec3(&track->frames[frame].value[0], out);
}

static void __track_sample_linear_vec3(track_s *track, float t, bool looping, vec3 out) {
  int32_t this_frame = track_frame_index(track, t, looping);

  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1)) {
    glm_vec3_zero(out);
    return;
  }

  int next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, t, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    glm_vec3_zero(out);
    return;
  }

  float time = (track_time - this_time) / frame_delta;

  vec3 start, end;
  track_cast_vec3(&track->frames[this_frame].value[0], start);
  track_cast_vec3(&track->frames[next_frame].value[0], end);

  glm_vec3_lerp(start, end, time, out);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static void __track_sample_cubic_vec3(track_s *track, float time, bool looping, vec3 out) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1)) {
    glm_vec3_zero(out);
    return;
  }

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    glm_vec3_zero(out);
    return;
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  vec3 point1;
  track_cast_vec3(&track->frames[this_frame].value[0], point1);
  vec3 slope1; // track_cast_vec3(&frame_get_out(track->frames[this_frame])) *
               // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, VECTOR_TRACK_KIND * flt_size);
  glm_vec3_scale(slope1, frame_delta, slope1);

  vec3 point2;
  track_cast_vec3(&track->frames[next_frame].value[0], point2);
  vec3 slope2; // track_cast_vec3(&frame_get_out(track->frames[next_frame])) *
               // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, VECTOR_TRACK_KIND * flt_size);
  glm_vec3_scale(slope2, frame_delta, slope2);

  __track_hermite_vec3(t, point1, slope1, point2, slope2, out);
}

static void __track_sample_constant_quat(track_s *track, float t, bool looping, versor out) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= (int32_t)SM_ARRAY_SIZE(track->frames)) {
    glm_quat_identity(out);
    return;
  }

  track_cast_quat(&track->frames[frame].value[0], out);
}

static void __track_sample_linear_quat(track_s *track, float t, bool looping, versor out) {
  int32_t this_frame = track_frame_index(track, t, looping);
  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1)) {
    glm_quat_identity(out);
    return;
  }

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, t, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    glm_quat_identity(out);
    return;
  }

  float time = (track_time - this_time) / frame_delta;

  versor start, end;
  track_cast_quat(&track->frames[this_frame].value[0], start);
  track_cast_quat(&track->frames[next_frame].value[0], end);

  quat_iterpolate(start, end, time, out);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static void __track_sample_cubic_quat(track_s *track, float time, bool looping, versor out) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (int32_t)(SM_ARRAY_SIZE(track->frames) - 1)) {
    glm_quat_identity(out);
    return;
  }

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    glm_quat_identity(out);
    return;
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  versor point1;
  track_cast_quat(&track->frames[this_frame].value[0], point1);
  versor slope1; // track_cast_vec3(&frame_get_out(track->frames[this_frame])) *
                 // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, QUATERNION_TRACK_KIND * flt_size);
  slope1[0] *= frame_delta;
  slope1[1] *= frame_delta;
  slope1[2] *= frame_delta;
  slope1[3] *= frame_delta;

  versor point2;
  track_cast_quat(&track->frames[next_frame].value[0], point2);
  versor slope2; // track_cast_quat(&frame_get_out(track->frames[next_frame])) *
                 // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, QUATERNION_TRACK_KIND * flt_size);
  slope2[0] *= frame_delta;
  slope2[1] *= frame_delta;
  slope2[2] *= frame_delta;
  slope2[3] *= frame_delta;

  __track_hermite_quat(t, point1, slope1, point2, slope2, out);
}

static float __track_hermite_float(float t, float p1, float s1, float _p2, float s2) {
  float tt = t * t;
  float ttt = tt * t;

  float p2 = _p2;

  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;

  float result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;

  return result;
}

static void __track_hermite_vec3(float t, vec3 p1, vec3 s1, vec3 _p2, vec3 s2, vec3 out) {
  float tt = t * t;
  float ttt = tt * t;

  vec3 p2;
  glm_vec3_copy(_p2, p2);

  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;

  /* vec3_add(vec3_add(vec3_add(vec3_scale(p1, h1), vec3_scale(p2, h2)), vec3_scale(s1, h3)), vec3_scale(s2, h4)); */
  vec3 tmp1, tmp2, tmp3, tmp4;
  glm_vec3_scale(p1, h1, tmp1);
  glm_vec3_scale(p2, h2, tmp2);
  glm_vec3_scale(s1, h3, tmp3);
  glm_vec3_scale(s2, h4, tmp4);
  glm_vec3_add(tmp1, tmp2, tmp1);
  glm_vec3_add(tmp1, tmp3, tmp2);
  glm_vec3_add(tmp2, tmp4, out);
}

static void __track_hermite_quat(float t, versor p1, versor s1, versor _p2, versor s2, versor out) {
  float tt = t * t;
  float ttt = tt * t;

  versor p2;
  glm_quat_copy(_p2, p2);
  __track_neighborhood(p1, p2);

  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;

  /* quat result = */
  /* quat_add(quat_add(quat_add(quat_scale(p1, h1), quat_scale(p2, h2)), quat_scale(s1, h3)), quat_scale(s2, h4)); */

  versor result;

  versor tmp1, tmp2, tmp3, tmp4;
  /* glm_quat_scale(p1, h1, tmp1); */
  tmp1[0] = p1[0] * h1;
  tmp1[1] = p1[1] * h1;
  tmp1[2] = p1[2] * h1;
  tmp1[3] = p1[3] * h1;

  /* glm_quat_scale(p2, h2, tmp2); */
  tmp2[0] = p2[0] * h2;
  tmp2[1] = p2[1] * h2;
  tmp2[2] = p2[2] * h2;
  tmp2[3] = p2[3] * h2;

  /* glm_quat_scale(s1, h3, tmp3); */
  tmp3[0] = s1[0] * h3;
  tmp3[1] = s1[1] * h3;
  tmp3[2] = s1[2] * h3;
  tmp3[3] = s1[3] * h3;

  /* glm_quat_scale(s2, h4, tmp4); */
  tmp4[0] = s2[0] * h4;
  tmp4[1] = s2[1] * h4;
  tmp4[2] = s2[2] * h4;
  tmp4[3] = s2[3] * h4;

  glm_quat_add(tmp1, tmp2, tmp1);
  glm_quat_add(tmp1, tmp3, tmp2);
  glm_quat_add(tmp2, tmp4, result);

  __track_adjust_hermite_result(result, out);
}

void __track_neighborhood(versor a, versor b) {
  if (glm_quat_dot(a, b) < 0) {
    b[0] = -b[0];
    b[1] = -b[1];
    b[2] = -b[2];
    b[3] = -b[3];
  }
}

void __track_adjust_hermite_result(versor q, versor out) {
  // TODO: INVESTIGATE
  glm_quat_normalize_to(q, out);
}

static void quat_iterpolate(versor a, versor b, float t, versor out) {
  versor result;
  if (glm_quat_dot(a, b) < 0) { // Neighborhood

    result[0] = a[0] * (1.0f - t) + -b[0] * t;
    result[1] = a[1] * (1.0f - t) + -b[1] * t;
    result[2] = a[2] * (1.0f - t) + -b[2] * t;
    result[3] = a[3] * (1.0f - t) + -b[3] * t;

  } else {
    result[0] = a[0] * (1.0f - t) + b[0] * t;
    result[1] = a[1] * (1.0f - t) + b[1] * t;
    result[2] = a[2] * (1.0f - t) + b[2] * t;
    result[3] = a[3] * (1.0f - t) + b[3] * t;
  }

  glm_quat_normalize_to(result, out); // NLerp, not slerp
}

// faster
void track_index_look_up_table(track_s *track) {

  assert(track != NULL);
  int32_t num_frames = SM_ARRAY_SIZE(track->frames);
  if (num_frames <= 1)
    return;

  float duration = track_get_end_time(track) - track_get_start_time(track);

  uint32_t num_samples = (uint32_t)(duration * 60.0f);
  track_resize_sampled_frames(track, num_samples);
  for (size_t i = 0; i < num_samples; ++i) {
    float t = (float)i / (float)(num_samples - 1);
    float time = t * duration + track_get_start_time(track);

    uint32_t frame_index = 0;
    for (int32_t j = num_frames - 1; j >= 0; --j) {
      if (time >= track->frames[j].t) {
        frame_index = (uint32_t)j;
        if ((int32_t)frame_index >= num_frames - 2) {
          frame_index = num_frames - 2;
        }
        break;
      }
    }

    track->sampled_frames[i] = frame_index;
  }
}
