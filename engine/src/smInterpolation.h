#ifndef SM_INTERPOLATION_H
#define SM_INTERPOLATION_H

typedef unsigned char INTERPOLATION_EX3;
// A constant curve keeps its value the same until the next keyframe.
// Sometimes, this type of curve is called a step curve.
#define CONSTANT_EX3 ((INTERPOLATION_EX3)0x01)
// A linear curve interpolates between two frames in a linear fashion (that
// is, in a straight line).
#define LINEAR_EX3 ((INTERPOLATION_EX3)0x02)
// A cubic curve lets you define a curve in terms of values and tangents.
#define CUBIC_EX3 ((INTERPOLATION_EX3)0x03)

#endif // SM_INTERPOLATION_H
