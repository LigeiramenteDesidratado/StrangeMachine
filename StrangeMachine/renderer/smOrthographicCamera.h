#ifndef SM_RENDERER_ORTHOGRAPHIC_CAMERA_H
#define SM_RENDERER_ORTHOGRAPHIC_CAMERA_H

#include "math/smMath.h"

typedef struct sm__orthographic_camera {

  sm_vec3 position;

  sm_mat4 projection;
  sm_mat4 view;
  sm_mat4 vp; /* view projection */

  float rotation;

} sm_orthographic_camera;

#endif /* SM_RENDERER_ORTHOGRAPHIC_CAMERA_H */
