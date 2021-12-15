#ifndef SM_CAMERA_P_H
#define SM_CAMERA_P_H

#include <stdint.h>

typedef uint8_t PROJECTION_EX4;
#define PERSPECTIVE_EX4 ((PROJECTION_EX4)0x01)
#define ORTHOGONAL_EX4 ((PROJECTION_EX4)0x02)

typedef uint8_t MODE_EX5;
#define FREE_EX5 ((MODE_EX5)0x01)
#define THIRD_PERSON_EX5 ((MODE_EX5)0x02)

#endif // SM_CAMERA_P_H
