#include "data/array.h"
#include "util/common.h"

#include "smTransformTrack.h"

// Destructor
void transform_track_dtor(transform_track_s *transform_track) {

  assert(transform_track != NULL);

  track_dtor(&transform_track->position);
  track_dtor(&transform_track->rotation);
  track_dtor(&transform_track->scale);
}

// The transform_track_is_valid helper function should only return true if at
// least one of the component tracks stored in the transform_track_s struct is
// valid. For a track to be valid, it needs to have two or more frames.
bool transform_track_is_valid(const transform_track_s *const transform_track) {
  assert(transform_track != NULL);
  return (SM_ARRAY_SIZE(transform_track->position.frames) > 1) ||
         (SM_ARRAY_SIZE(transform_track->rotation.frames) > 1) || (SM_ARRAY_SIZE(transform_track->scale.frames) > 1);
}

// The GetStartTime function should return the smallest start time of the three
// component tracks. If none of the components are valid (that is, they all have
// one or no frames), then transform_track isn't valid. In this case, just
// return 0.
float transform_track_get_start_time(const transform_track_s *const transform_track) {

  assert(transform_track != NULL);

  float result = 0.0f;
  bool is_set = false;

  if (SM_ARRAY_SIZE(transform_track->position.frames) > 1) {
    result = track_get_start_time(&transform_track->position);
    is_set = true;
  }

  if (SM_ARRAY_SIZE(transform_track->rotation.frames) > 1) {
    float rotation_start = track_get_start_time(&transform_track->rotation);
    if (rotation_start < result || !is_set) {
      result = rotation_start;
      is_set = true;
    }
  }

  if (SM_ARRAY_SIZE(transform_track->scale.frames) > 1) {

    float scale_start = track_get_start_time(&transform_track->scale);
    if (scale_start < result || !is_set) {
      result = scale_start;
      is_set = true;
    }
  }

  return result;
}

// The GetEndTime function is similar to the GetStartTime function. The only
// difference is that this function looks for the greatest trak end time.
float transform_track_get_end_time(const transform_track_s *const transform_track) {

  assert(transform_track != NULL);

  float result = 0.0f;
  bool is_set = false;

  if (SM_ARRAY_SIZE(transform_track->position.frames) > 1) {
    result = track_get_end_time(&transform_track->position);
    is_set = true;
  }

  if (SM_ARRAY_SIZE(transform_track->rotation.frames) > 1) {
    float rotation_end = track_get_end_time(&transform_track->rotation);
    if (rotation_end > result || !is_set) {
      result = rotation_end;
      is_set = true;
    }
  }

  if (SM_ARRAY_SIZE(transform_track->scale.frames) > 1) {

    float scale_end = track_get_end_time(&transform_track->scale);
    if (scale_end > result || !is_set) {
      result = scale_end;
      is_set = true;
    }
  }

  return result;
}

// The transform_track_sample function only samples one of its component tracks
// if that track has two or more frames. Since a TransformTrack class can
// animate only one component, such as the position, this function needs to take
// a reference transform  as an argument. If one of the transform components
// isn't animated by the transform track, the value of the reference transform
// is used.
transform_s transform_track_sample(transform_track_s *transform_track, transform_s *transform_ref, float time,
                                   bool looping) {
  assert(transform_track != NULL);

  transform_s result = *transform_ref; // Assign default values

  if (SM_ARRAY_SIZE(transform_track->position.frames) > 1) // only if valid
    track_sample_vec3(&transform_track->position, time, looping, result.position);

  if (SM_ARRAY_SIZE(transform_track->rotation.frames) > 1) // only if valid
    track_sample_quat(&transform_track->rotation, time, looping, result.rotation);

  if (SM_ARRAY_SIZE(transform_track->scale.frames) > 1) // only if valid
    track_sample_vec3(&transform_track->scale, time, looping, result.scale);

  return result;
}
