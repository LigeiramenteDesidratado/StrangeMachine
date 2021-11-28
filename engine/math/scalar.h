#ifndef SCALAR_H
#define SCALAR_H

#define PI 3.14159265358979323846264338327950288
#define TAU 2 * PI

#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

#define EPSILON 0.000001f
#define scalar_zero() (0.0f)

float scalar_lerp(float a, float b, float t);

#endif // SCALAR_H
