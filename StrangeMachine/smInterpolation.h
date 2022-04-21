#ifndef SM_INTERPOLATION_H
#define SM_INTERPOLATION_H

typedef enum {

  /* A constant curve keeps its value the same until the next keyframe. Sometimes, this type of curve is called a step
     curve. */
  CONSTANT_INTERP = 0x01,
  /* A linear curve interpolates between two frames in a linear fashion (that is, in a straight line). */
  LINEAR_INTERP = 0x02,
  /* A cubic curve lets you define a curve in terms of values and tangents. */
  CUBIC_INTERP = 0x03,

} interpolation_e;

#endif // SM_INTERPOLATION_H
