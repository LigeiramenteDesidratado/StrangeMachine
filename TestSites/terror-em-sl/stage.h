#ifndef STAGE_H
#define STAGE_H

#include "scene.h"

#include "math/smMath.h"

struct stage_s;

// Allocate memory
struct stage_s *stage_new(void);

// Constructor
int stage_ctor(struct stage_s *stage);

// Destructor
void stage_dtor(struct stage_s *stage);

// Behavior
void stage_draw(struct stage_s *stage, float aspect_ratio);
void stage_do(struct stage_s *stage, float dt);
void stage_get_camera_view(struct stage_s* stage, mat4 out);
/* void stage_set_scene(struct stage_t *stage, scenes_id id); */

#endif // STAGE_H
