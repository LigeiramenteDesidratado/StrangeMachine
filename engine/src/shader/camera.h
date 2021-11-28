#ifndef CAMERA_H
#define CAMERA_H

#include "shader/camera_p.h"
#include "util/common.h"

struct camera_s;

// Constructor
void camera_init(vec3 position, vec3 target, vec3 up, MODE_EX5 mode, PROJECTION_EX4 projection);

// Destructor
void camera_tear_down(void);

// Forward declaration

void camera_do(float dt);
mat4 camera_get_view();
void camera_set_target(vec3 target);
void camera_set_mode( MODE_EX5 mode);
void camera_set_projection(PROJECTION_EX4 projection);
mat4 camera_get_projection_matrix(float aspect_ratio);
float camera_get_fovy();
MODE_EX5 camera_get_mode();
PROJECTION_EX4 camera_get_projection();



#endif // CAMERA_H
