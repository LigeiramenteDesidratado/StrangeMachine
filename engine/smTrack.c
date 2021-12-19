#include "smTrack.h"
#include "util/common.h"

#define SCALAR_TRACK 1
#define VECTOR_TRACK 3
#define QUATERION_TRACK 4

static vec3 __track_sample_constant_vec3(track_s *track, float t, bool looping);
static vec3 __track_sample_cubic_vec3(track_s *track, float time, bool looping);
static vec3 __track_sample_linear_vec3(track_s *track, float time, bool looping);

static float __track_sample_constant_float(track_s *track, float t, bool looping);
static float __track_sample_cubic_float(track_s *track, float time, bool looping);
static float __track_sample_linear_float(track_s *track, float time, bool looping);

static quat __track_sample_constant_quat(track_s *track, float t, bool looping);
static quat __track_sample_cubic_quat(track_s *track, float time, bool looping);
static quat __track_sample_linear_quat(track_s *track, float time, bool looping);

static vec3 __track_hermite_vec3(float t, vec3 p1, vec3 s1, vec3 _p2, vec3 s2);
static float __track_hermite_float(float t, float p1, float s1, float _p2, float s2);
static quat __track_hermite_quat(float t, quat p1, quat s1, quat _p2, quat s2);

void __track_neighborhood(const quat *a, quat *b);
quat __track_adjust_hermite_result(quat q);
static quat quat_iterpolate(quat a, quat b, float t);
void track_index_look_up_table(track_s *track);

// Destructor
void track_dtor(track_s *track) {
  assert(track != NULL);

  for (size_t i = 0; i < arrlenu(track->frames); ++i) {
    arrfree(track->frames[i].value);
    arrfree(track->frames[i].in);
    arrfree(track->frames[i].out);
  }

  arrfree(track->frames);
  arrfree(track->sampled_frames);
}

float track_get_start_time(const track_s *const track) {

  assert(track != NULL);

  return track->frames[0].t;
}

float track_get_end_time(const track_s *const track) {

  assert(track != NULL);

  return track->frames[arrlenu(track->frames) - 1].t;
}

float track_sample_float(track_s *track, float time, bool looping) {
  switch (track->interpolation) {
  case CONSTANT_EX3:
    return __track_sample_constant_float(track, time, looping);
    break;
  case CUBIC_EX3:
    return __track_sample_cubic_float(track, time, looping);
    break;
  case LINEAR_EX3:
    return __track_sample_linear_float(track, time, looping);
    break;
  default:
    log_warn("unkown iterpolation");
  }
  return scalar_zero();
}

vec3 track_sample_vec3(track_s *track, float time, bool looping) {

  switch (track->interpolation) {
  case CONSTANT_EX3:
    return __track_sample_constant_vec3(track, time, looping);
    break;
  case CUBIC_EX3:
    return __track_sample_cubic_vec3(track, time, looping);
    break;
  case LINEAR_EX3:
    return __track_sample_linear_vec3(track, time, looping);
    break;
  default:
    log_warn("unkown iterpolation");
  }
  return vec3_zero();
}

quat track_sample_quat(track_s *track, float time, bool looping) {
  switch (track->interpolation) {
  case CONSTANT_EX3:
    return __track_sample_constant_quat(track, time, looping);
    break;
  case CUBIC_EX3:
    return __track_sample_cubic_quat(track, time, looping);
    break;
  case LINEAR_EX3:
    return __track_sample_linear_quat(track, time, looping);
    break;
  default:
    // TODO: Decent log system
    log_warn("unkown iterpolation");
  }
  return quat_identity();
}

void track_resize_sampled_frames(track_s *track, size_t size) {
  assert(track != NULL);

  size_t old_length = arrlenu(track->sampled_frames);
  arrsetlen(track->sampled_frames, size);
  size_t new_length = arrlenu(track->sampled_frames);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->sampled_frames[old_length + i] = 0;
  }
}

// changed from track_resize --> track_resize_frame;
void track_resize_frame(track_s *track, size_t size) {

  assert(track != NULL);

  size_t old_length = arrlenu(track->frames);
  arrsetlen(track->frames, size);
  size_t new_length = arrlenu(track->frames);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[old_length + i] = frame_zero();
  }
}

void track_resize_frame_in(track_s *track, size_t size, uint32_t frame_index) {

  assert(track != NULL);
  if (frame_index > arrlenu(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = arrlenu(track->frames[frame_index].in);
  arrsetlen(track->frames[frame_index].in, size);
  size_t new_length = arrlenu(track->frames[frame_index].in);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].in[old_length + i] = 0.0f;
  }
}

void track_resize_frame_out(track_s *track, size_t size, uint32_t frame_index) {

  assert(track != NULL);
  if (frame_index >= arrlenu(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = arrlenu(track->frames[frame_index].out);
  arrsetlen(track->frames[frame_index].out, size);
  size_t new_length = arrlenu(track->frames[frame_index].out);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].out[old_length + i] = 0.0f;
  }
}

void track_resize_frame_value(track_s *track, size_t size, uint32_t frame_index) {
  assert(track != NULL);
  if (frame_index > arrlenu(track->frames)) {
    log_warn("trying to access invalid index");
    return;
  }

  size_t old_length = arrlenu(track->frames[frame_index].value);
  arrsetlen(track->frames[frame_index].value, size);
  size_t new_length = arrlenu(track->frames[frame_index].value);

  for (size_t i = 0; i < (new_length - old_length); ++i) {
    track->frames[frame_index].value[old_length + i] = 0.0f;
  }
}

size_t track_get_frame_size(track_s *track) {

  assert(track != NULL);

  return arrlenu(track->frames);
}

INTERPOLATION_EX3 track_get_interpolation(track_s *track) {

  assert(track != NULL);

  return track->interpolation;
}

void track_set_interpolation(track_s *track, INTERPOLATION_EX3 interpolation) {
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
  unsigned int size = arrlenu(track->frames);
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

  size_t size = arrlenu(track->frames);
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

  if (index >= arrlenu(track->sampled_frames)) {
    /* log_debug("index (%d) is gt array length (%lu)", index, */
    /* arrlenu(track->sampled_frames)); */
    return (int32_t)track->sampled_frames[arrlenu(track->sampled_frames) - 1];
    // return -1;
  }

  return (int32_t)track->sampled_frames[index];
}

#endif

float track_adjust_time(track_s *track, float t, bool looping) {

  // If a track has less than one frame, the track is invalid. If an invalid
  // track is used, retun 0
  size_t size = arrlenu(track->frames);
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

float track_cast_float(float *value) { return value[0]; }

vec3 track_cast_vec3(float *value) { return vec3_new(value[0], value[1], value[2]); }

quat track_cast_quat(float *value) {

  quat r = quat_new(value[0], value[1], value[2], value[3]);
  return quat_norm(r);
}

// Constant sampling is often used for things such as visibility flags, where it
// makes sense for the value of a variable to change from one frame to the next
// without any real interpolation
static float __track_sample_constant_float(track_s *track, float t, bool looping) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= arrlen(track->frames)) {
    return 0.0f;
  }

  return track->frames[frame].value[0];
}

static float __track_sample_linear_float(track_s *track, float t, bool looping) {
  int32_t this_frame = track_frame_index(track, t, looping);
  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
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

  return scalar_lerp(start, end, time);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static float __track_sample_cubic_float(track_s *track, float time, bool looping) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
    return 0.0f;

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return scalar_zero();
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  float point1 = track->frames[this_frame].value[0];
  float slope1; // track_cast_float(&frame_get_out(track->frames[this_frame])) *
                // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, SCALAR_TRACK * flt_size);
  slope1 = slope1 * frame_delta;

  float point2 = track->frames[next_frame].value[0];
  float slope2; // track_cast_float(&frame_get_out(track->frames[next_frame])) *
                // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, SCALAR_TRACK * flt_size);
  slope2 = slope2 * frame_delta;

  return __track_hermite_float(t, point1, slope1, point2, slope2);
}

static vec3 __track_sample_constant_vec3(track_s *track, float t, bool looping) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= arrlen(track->frames))
    return vec3_zero();

  return track_cast_vec3(&track->frames[frame].value[0]);
}

static vec3 __track_sample_linear_vec3(track_s *track, float t, bool looping) {
  int32_t this_frame = track_frame_index(track, t, looping);

  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
    return vec3_zero();

  int next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, t, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return vec3_zero();
  }

  float time = (track_time - this_time) / frame_delta;

  vec3 start = track_cast_vec3(&track->frames[this_frame].value[0]);
  vec3 end = track_cast_vec3(&track->frames[next_frame].value[0]);

  return vec3_lerp(start, end, time);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static vec3 __track_sample_cubic_vec3(track_s *track, float time, bool looping) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
    return vec3_zero();

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return vec3_zero();
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  vec3 point1 = track_cast_vec3(&track->frames[this_frame].value[0]);
  vec3 slope1; // track_cast_vec3(&frame_get_out(track->frames[this_frame])) *
               // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, VECTOR_TRACK * flt_size);
  slope1 = vec3_scale(slope1, frame_delta);

  vec3 point2 = track_cast_vec3(&track->frames[next_frame].value[0]);
  vec3 slope2; // track_cast_vec3(&frame_get_out(track->frames[next_frame])) *
               // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, VECTOR_TRACK * flt_size);
  slope2 = vec3_scale(slope2, frame_delta);

  return __track_hermite_vec3(t, point1, slope1, point2, slope2);
}

static quat __track_sample_constant_quat(track_s *track, float t, bool looping) {

  // To do a constant (step) sample, find the frame based on the time with the
  // track_frame_index helper. Make sure the frame is valid, then cast the value
  // of that frame to the correct data type and return it
  int32_t frame = track_frame_index(track, t, looping);
  if (frame < 0 || frame >= arrlen(track->frames))
    return quat_identity();

  return track_cast_quat(&track->frames[frame].value[0]);
}

static quat __track_sample_linear_quat(track_s *track, float t, bool looping) {
  int32_t this_frame = track_frame_index(track, t, looping);
  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
    return quat_identity();

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, t, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return quat_identity();
  }

  float time = (track_time - this_time) / frame_delta;

  quat start = track_cast_quat(&track->frames[this_frame].value[0]);
  quat end = track_cast_quat(&track->frames[next_frame].value[0]);

  return quat_iterpolate(start, end, time);
}

// Cubic track sampling The final type of sampling, cubic sampling, finds the
// frames to sample and the interpolation time in the same way that linear
// sampling did. This function calls the Hermite helper function to do its
// interpolation.
static quat __track_sample_cubic_quat(track_s *track, float time, bool looping) {

  int32_t this_frame = track_frame_index(track, time, looping);
  if (this_frame < 0 || this_frame >= (arrlen(track->frames) - 1))
    return quat_identity();

  int32_t next_frame = this_frame + 1;

  float track_time = track_adjust_time(track, time, looping);
  float this_time = track->frames[this_frame].t;
  float frame_delta = track->frames[next_frame].t - this_time;

  if (frame_delta <= 0.0f) {
    return quat_identity();
  }

  float t = (track_time - this_time) / frame_delta;
  size_t flt_size = sizeof(float);

  quat point1 = track_cast_quat(&track->frames[this_frame].value[0]);
  quat slope1; // track_cast_vec3(&frame_get_out(track->frames[this_frame])) *
               // frame_delta;

  memcpy(&slope1, track->frames[this_frame].out, QUATERION_TRACK * flt_size);
  slope1 = quat_scale(slope1, frame_delta);

  quat point2 = track_cast_quat(&track->frames[next_frame].value[0]);
  quat slope2; // track_cast_quat(&frame_get_out(track->frames[next_frame])) *
               // frame_delta;

  memcpy(&slope2, track->frames[next_frame].in, QUATERION_TRACK * flt_size);
  slope2 = quat_scale(slope2, frame_delta);

  return __track_hermite_quat(t, point1, slope1, point2, slope2);
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

static vec3 __track_hermite_vec3(float t, vec3 p1, vec3 s1, vec3 _p2, vec3 s2) {
  float tt = t * t;
  float ttt = tt * t;

  vec3 p2 = _p2;

  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;

  vec3 result =
      vec3_add(vec3_add(vec3_add(vec3_scale(p1, h1), vec3_scale(p2, h2)), vec3_scale(s1, h3)), vec3_scale(s2, h4));

  return result;
}

static quat __track_hermite_quat(float t, quat p1, quat s1, quat _p2, quat s2) {
  float tt = t * t;
  float ttt = tt * t;

  quat p2 = _p2;
  __track_neighborhood(&p1, &p2);

  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;

  quat result =
      quat_add(quat_add(quat_add(quat_scale(p1, h1), quat_scale(p2, h2)), quat_scale(s1, h3)), quat_scale(s2, h4));

  return __track_adjust_hermite_result(result);
}

void __track_neighborhood(const quat *a, quat *b) {
  if (quat_dot(*a, *b) < 0) {
    *b = quat_negate(*b);
  }
}

quat __track_adjust_hermite_result(quat q) { return quat_norm(q); }

static quat quat_iterpolate(quat a, quat b, float t) {
  quat result = quat_mix(a, b, t);
  if (quat_dot(a, b) < 0) { // Neighborhood
    result = quat_mix(a, quat_negate(b), t);
  }
  return quat_norm(result); // NLerp, not slerp
}

// faster
void track_index_look_up_table(track_s *track) {

  assert(track != NULL);
  int32_t num_frames = arrlen(track->frames);
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
