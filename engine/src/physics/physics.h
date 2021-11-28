#ifndef PHYSICS_H
#define PHYSICS_H

#include "util/common.h"
#include "model/mesh.h"

struct physics_s;

// Allocate memory
extern struct physics_s *physics_new(void);

// Constructor

extern status_v physics_sphere_ctor(struct physics_s *physics, vec3 pos, float radius);
extern status_v physics_capsule_ctor(struct physics_s *physics, vec3 pos, float radius, float height);
extern status_v physics_terrain_ctor(struct physics_s *physics, vec3 pos, mesh_s* mesh);

// Destructor
extern void physics_dtor(struct physics_s *physics);

// Forward declaration
extern void physics_step(void);
extern void physics_do_late(struct physics_s *physics);
extern void physics_draw(struct physics_s *physics);
extern void physics_add_force(struct physics_s *physics, vec3 force);
extern void physics_scale_force(struct physics_s *physics, float by);
extern void physics_apply_force(struct physics_s *physics);
extern vec3 physics_get_pos(struct physics_s *physics);
extern vec3 physics_get_force(struct physics_s *physics);
extern vec3 physics_get_velocity(struct physics_s *physics);
capsule_s physics_get_capsule(struct physics_s *physics);

#endif

