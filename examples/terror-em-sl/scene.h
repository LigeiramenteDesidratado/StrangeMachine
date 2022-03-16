#ifndef SCENE_H
#define SCENE_H

#include "scene_defs.h"
#include "math/smMath.h"

// Foward declaration
struct scene_s;

// Allocate memory
struct scene_s *scene_new();

// Constructor
void scene_ctor(struct scene_s *scene, scenes_e id);

// Destructor
void scene_dtor(struct scene_s *scene);

// Behavior
void scene_do(struct scene_s *scene, float dt);
/* void scene_do_late(struct scene_s *scene); */
void scene_draw(struct scene_s *scene);
void scene_get_look_at(struct scene_s *scene, vec3 out);
/* void scene_draw_late(struct scene_t *scene); */
scenes_e scene_get_id(struct scene_s *scene);

#endif
