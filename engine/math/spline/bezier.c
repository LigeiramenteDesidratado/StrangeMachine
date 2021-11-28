
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "math/vec3.h"

typedef struct {
  vec3 p1, p2; // Points
  vec3 c1, c2; // Controls

} bezier_t;

// Allocate memory
bezier_t *bezier_new(void) {
  bezier_t *bezier = (bezier_t *)malloc(sizeof(bezier_t));

  assert(bezier != NULL);

  return bezier;
}

// Constructor
bool bezier_ctor(bezier_t *bezier, vec3 p1, vec3 p2, vec3 c1, vec3 c2) {

  assert(bezier != NULL);

  bezier->c1 = c1;
  bezier->c2 = c2;
  bezier->p1 = p1;
  bezier->p2 = p2;

  return true;
}

vec3 bezier_interpolate(bezier_t *bezier, float t) {

  vec3 A = vec3_lerp(bezier->p1, bezier->c1, t);
  vec3 B = vec3_lerp(bezier->c2, bezier->p2, t);
  vec3 C = vec3_lerp(bezier->c1, bezier->c2, t);

  vec3 D = vec3_lerp(A, C, t);
  vec3 E = vec3_lerp(C, B, t);

  vec3 R = vec3_lerp(D, E, t);

  return R;
}
