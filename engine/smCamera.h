#ifndef SM_CAMERA_H
#define SM_CAMERA_H

#include "smCameraP.h"

#include "math/mat4.h"
#include "math/vec3.h"

struct camera_s;

// Constructor
void camera_init(vec3 position, vec3 target, vec3 up, cam_mode_e mode, cam_projection_e projection);

// Destructor
void camera_tear_down(void);

// Forward declaration

void camera_do(float dt);
mat4 camera_get_view();
void camera_set_target(vec3 target);
void camera_set_mode(cam_mode_e mode);
void camera_set_projection(cam_projection_e projection);
mat4 camera_get_projection_matrix(float aspect_ratio);
float camera_get_fovy();
cam_mode_e camera_get_mode();
cam_projection_e camera_get_projection();

#endif // SM_CAMERA_H
