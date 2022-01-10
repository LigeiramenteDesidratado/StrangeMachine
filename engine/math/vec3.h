#ifndef VEC3_H
#define VEC3_H

#include <stdbool.h>

#include "scalar.h"

typedef struct {
  union {
    struct {
      float x;
      float y;
      float z;
    };

    struct {
      float r;
      float g;
      float b;
    };

    struct {
      float pitch;
      float yaw;
      float roll;
    };

    float v[3];
  };

} vec3;

#define vec3_print(V) printf("%s\n\t%f, %f, %f\n", #V, V.x, V.y, V.z)

#define vec3_zero() ((vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f})
#define vec3_new(X, Y, Z) ((vec3){.x = X, .y = Y, .z = Z})

#define world_right vec3_new(1.0f, 0.0f, 0.0f)
#define world_up vec3_new(0.0f, 1.0f, 0.0f)
#define world_forward vec3_new(0.0f, 0.0f, 1.0f)

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_add_val(vec3 a, float b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_sub_val(vec3 a, float b);
vec3 vec3_scale(vec3 a, float by);
vec3 vec3_mul(vec3 a, vec3 b);
vec3 vec3_div_scalar(vec3 a, float by);
vec3 vec3_max(vec3 a, vec3 b);
vec3 vec3_min(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);
float vec3_len_sq(vec3 v);
float vec3_len(vec3 v);
vec3 vec3_norm(vec3 v);
float vec3_angle(vec3 a, vec3 b);
vec3 vec3_proj(vec3 a, vec3 b);
vec3 vec3_rej(vec3 a, vec3 b);
vec3 vec3_ref(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
vec3 vec3_lerp(vec3 a, vec3 b, float t);
vec3 vec3_slerp(vec3 a, vec3 b, float t);
vec3 vec3_nlerp(vec3 a, vec3 b, float t);
bool vec3_is_equal(vec3 a, vec3 b);
bool vec3_not_equal(vec3 a, vec3 b);
float rev_sqrt(float number);
void vec3_orthonorm(float left[3], float up[3], const float v[3]);

#endif // VEC3_H
