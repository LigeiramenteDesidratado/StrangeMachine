#ifndef SM_CAMERA_P_H
#define SM_CAMERA_P_H

#include <stdint.h>

typedef enum { PERSPECTIVE = 0x01, ORTHOGONAL = 0x02 } cam_projection_e;
typedef enum { FREE = 0x01, THIRD_PERSON = 0x02 } cam_mode_e;

#endif // SM_CAMERA_P_H
