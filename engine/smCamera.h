#ifndef SM_CAMERA_H
#define SM_CAMERA_H

#include "smCameraP.h"

#include "math/smMath.h"

struct camera_s;

// Constructor
void camera_init(sm_vec3 position, sm_vec3 target, sm_vec3 up, cam_mode_e mode, cam_projection_e projection);

// Destructor
void camera_tear_down(void);

// Forward declaration

void camera_do(float dt);
void camera_get_view(mat4 out_view);
void camera_set_target(vec3 target);
void camera_set_mode(cam_mode_e mode);
void camera_set_projection(cam_projection_e projection);
void camera_get_projection_matrix(float aspect_ratio, mat4 out_projection);
float camera_get_fovy();
cam_mode_e camera_get_mode();
cam_projection_e camera_get_projection();

#endif // SM_CAMERA_H
