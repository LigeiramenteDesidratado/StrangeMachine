/* An animation clip is a collection of animation tracks; each track describes
the motion of one joint over time and all of the tracks combined describe the
motion of the animated model over time. If you sample an animation clip, you get
a pose that describes the configuration of each joint in the animation clip at
the specified time. For a basic clip class, all you need is a vector of
transform tracks. Because transform tracks contain the ID of the joint that they
affect, you can have a minimal number of tracks per clip. The Clip class should
also keep track of metadata, such as the name of the clip, whether the clip is
looping, and information about the time or duration of the clip. */

#include "smPose.h"
#include "smTransformTrack.h"
#include "util/common.h"

typedef struct {
  //  The clip_t struct needs to maintain an array of transform tracks. This is
  //  the most important data that the clip contains. Other than the tracks, a
  //  clip has a name, a start time, and an end time, and the clip should know
  //  whether it's looping or not.
  transform_track_s *tracks;

  char *name;
  float start_time;
  float end_time;
  bool looping;

} clip_s;

static float __clip_adjust_time(const clip_s *const clip, float t);
static void __clip_set_name(clip_s *clip, const char *name);
float clip_get_duration(const clip_s *const clip);

// Allocate memory
clip_s *clip_new(void) {
  clip_s *clip = (clip_s *)calloc(1, sizeof(clip_s));

  assert(clip != NULL);

  clip->looping = true;

  return clip;
}

// Constructor
bool clip_ctor(clip_s *clip, const char *name) {

  assert(clip != NULL);
  __clip_set_name(clip, name);

  return true;
}

// Destructor
void clip_dtor(clip_s *clip) {

  assert(clip != NULL);

  for (size_t i = 0; i < arrlenu(clip->tracks); ++i) {
    transform_track_dtor(&clip->tracks[i]);
  }
  arrfree(clip->tracks);
  clip->tracks = NULL;

  if (clip->name != NULL) {
    free(clip->name);
    clip->name = NULL;
  }

  free(clip);
  clip = NULL;
}

static float __clip_adjust_time(const clip_s *const clip, float t) {
  assert(clip != NULL);

  if (clip->looping) {
    float duration = clip->end_time - clip->start_time;
    if (duration <= 0.0f)
      return scalar_zero();

    t = fmodf(t - clip->start_time, clip->end_time - clip->start_time);

    if (t < 0.0f) {
      t += clip->end_time - clip->start_time;
    }

    t = t + clip->start_time;
  } else {
    if (t < clip->start_time)
      t = clip->start_time;

    if (t > clip->end_time)
      t = clip->end_time;
  }

  return t;
}

// Provide a way to get the number of joints a clip contains, as well as the
// joint ID for a specific track index.
uint32_t clip_get_id_at_index(const clip_s *const clip, uint32_t index) {
  assert(clip != NULL);

  if (index >= arrlenu(clip->tracks))
    return 0;

  return clip->tracks[index].id;
}

void clip_set_id_at_index(clip_s *clip, uint32_t index, uint32_t id) {
  assert(clip != NULL);

  if (index < arrlenu(clip->tracks))
    clip->tracks[index].id = id;
}

size_t clip_get_size(const clip_s *const clip) {
  assert(clip != NULL);

  return arrlenu(clip->tracks);
}

void clip_set_cap_tracks(clip_s *clip, size_t value) {
  assert(clip != NULL);

  arrsetcap(clip->tracks, value);
}

// The Sample function takes a  posere ference and a time and returns a float
// value that is also a time. This function samples the animation clip at the
// provided time into the pose reference.
float clip_sample(const clip_s *const clip, pose_s *pose, float t) {
  assert(clip != NULL);
  assert(pose != NULL);

  if (clip_get_duration(clip) == 0.0f) {
    return 0.0f;
  }

  t = __clip_adjust_time(clip, t);

  size_t size = arrlenu(clip->tracks);
  for (size_t i = 0; i < size; ++i) {
    uint32_t j = clip->tracks[i].id; // joint
    transform_s local = pose_get_local_transform(pose, j);
    transform_s animated =
        transform_track_sample(&clip->tracks[i], &local, t, clip->looping);

    pose_set_local_transform(pose, j, animated);
  }

  return t;
}

// The RecalculateDuration function sets mStartTime and mEndTime to default
// values of 0. Next, these functions loop through every TransformTrack object
// in the animation clip. If the track is valid, the start and end times of the
// track are retrieved. The smallest start time and the largest end time are
// stored. The start time of a clip might not be 0; it's possible to have a clip
// that starts at an arbitrary point in time.
void clip_recalculate_duration(clip_s *clip) {
  assert(clip != NULL);

  clip->start_time = 0.0f;
  clip->end_time = 0.0f;

  bool start_set = false;
  bool end_set = false;

  size_t track_size = arrlen(clip->tracks);
  for (size_t i = 0; i < track_size; ++i) {
    if (transform_track_is_valid(&clip->tracks[i])) {
      float start_time = transform_track_get_start_time(&clip->tracks[i]);
      float end_time = transform_track_get_end_time(&clip->tracks[i]);

      if (start_time < clip->start_time || !start_set) {
        clip->start_time = start_time;
        start_set = true;
      }

      if (end_time > clip->end_time || !end_set) {
        clip->end_time = end_time;
        end_set = true;
      }
    }
  }
}

// clip_get_transform_track_from_joint is meant to retrieve the
// transform_track_t object for a specific joint in the clip. This function is
// mainly used by whatever code loads the animation clip from a file. The
// function performs a linear search through all of the tracks to see whether
// any of them targets the specified joint. If a qualifying track is found, a
// reference to it is returned. If no qualifying track is found, a new one is
// created and returned:
// similar [] operator
transform_track_s *clip_get_transform_track_from_joint(clip_s *clip,
                                                       uint32_t joint) {
  assert(clip != NULL);

  for (size_t i = 0, s = arrlenu(clip->tracks); i < s; ++i) {
    if (clip->tracks[i].id == joint) {
      return &clip->tracks[i];
    }
  }

  transform_track_s tr = transform_track_new();
  tr.id = joint;

  arrput(clip->tracks, tr);
  return &arrlast(clip->tracks);
}

void clip_resize_tracks(clip_s *clip, uint32_t value) {
  assert(clip != NULL);

  clip->tracks = realloc(clip->tracks, value);
}

char *clip_get_name(const clip_s *const clip) {
  assert(clip != NULL);

  return clip->name;
}

static void __clip_set_name(clip_s *clip, const char *name) {
  assert(clip != NULL);

  clip->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
  strcpy(clip->name, name);
}

float clip_get_duration(const clip_s *const clip) {
  assert(clip != NULL);

  return clip->end_time - clip->start_time;
}

float clip_get_start_time(const clip_s *const clip) {
  assert(clip != NULL);

  return clip->start_time;
}

float clip_get_end_time(const clip_s *const clip) {
  assert(clip != NULL);
  return clip->end_time;
}

bool clip_get_looping(const clip_s *const clip) {
  assert(clip != NULL);
  return clip->looping;
}

void clip_set_looping(clip_s *clip, bool looping) {

  assert(clip != NULL);
  clip->looping = looping;
}
