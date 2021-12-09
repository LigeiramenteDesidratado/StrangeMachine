#include "util/bitmask.h"
#include "util/common.h"

#include "smCollision.h"
#include "smMesh.h"
#include "smText.h"
#include "smShapes.h"

#define GRAVITY  9.8f
#define GROUND_FRICTION  0.8f
#define AIR_FRICTION  0.9f

void physics_init(void) {
  // TODO: implement
}

// ENUM X ID
typedef unsigned char EX0;
// TODO: more shapes?
#define EMPTY_EX0   ((EX0)(0 << 0)) // 0b00000000 0
#define SPHERE_EX0  ((EX0)(1 << 0)) // 0b00000001 1
#define CAPSULE_EX0 ((EX0)(1 << 1)) // 0b00000010 2
#define MESH_EX0    ((EX0)(1 << 2)) // 0b00000100 4

typedef struct {

  int8_t __id;
  EX0 entity_type;

  vec3 position;
  vec3 force;
  vec3 velocity;
  vec3 gravity;

  union {
    capsule_s capsule;
    sphere_s sphere;
    mesh_s *mesh_ref;

  } entity_body;

} physics_s;

// Forward declaration
static void __handle_capsule_mesh(physics_s *physics, physics_s *against);
static void __handle_sphere_mesh(physics_s *physics, physics_s *against);
static void __handle_default(physics_s *physics, physics_s *against);

static void (*handler[SPHERE_EX0 | CAPSULE_EX0 | MESH_EX0])(
    physics_s *physics, physics_s *against) = {
    [SPHERE_EX0  | SPHERE_EX0] = __handle_default,   // TODO: The collision of sphere-to-sphere is ready, but I can't stop being lazy
    [SPHERE_EX0  | CAPSULE_EX0] = __handle_default,  // TODO: implement...
    [SPHERE_EX0  | MESH_EX0] = __handle_sphere_mesh,
    [CAPSULE_EX0 | CAPSULE_EX0] = __handle_default,  // TODO: implement...
    [CAPSULE_EX0 | MESH_EX0] = __handle_capsule_mesh,
    [MESH_EX0    | MESH_EX0] = __handle_default,     // TODO: implement...
};

// Globals
struct {
  int8_t count;
  physics_s *bodies[INT8_MAX];

} entity = {

    .count = -1,
    .bodies = {NULL},
};

void physics_step(void) {

  for (signed char i = 0; i <= entity.count; ++i) {
    for (signed char j = i + 1; j <= entity.count; ++j) {
      handler[entity.bodies[i]->entity_type | entity.bodies[j]->entity_type](
          entity.bodies[i], entity.bodies[j]);
    }
  }
}

// Allocate memory
physics_s *physics_new(void) {
  physics_s *physics = (physics_s *)calloc(1, sizeof(physics_s));

  assert(physics != NULL);

  return physics;
}

// Sphere Constructor
status_v physics_sphere_ctor(physics_s *physics, vec3 pos, float radius) {

  assert(physics != NULL);
  assert(radius > 0);

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return fail;
  }

  if (physics->entity_type != EMPTY_EX0 || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return fail;
  }
  physics->__id = entity.count;
  physics->entity_type = SPHERE_EX0;

  physics->entity_body.sphere.center = pos;
  physics->entity_body.sphere.radius = radius;

  entity.bodies[entity.count] = physics;

  return ok;
}

status_v physics_capsule_ctor(physics_s *physics, vec3 pos, float radius,
                              float height) {

  assert(physics != NULL);
  assert(radius > 0.0f && "negative radius");
  assert(height > 0.0f && "negative height");

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return fail;
  }

  if (physics->entity_type != EMPTY_EX0 || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return fail;
  }
  physics->__id = entity.count;
  physics->entity_type = CAPSULE_EX0;

  physics->position = pos;

  sphere_s s = {pos, radius};
  physics->entity_body.capsule = shapes_capsule_new(s, height);

  entity.bodies[entity.count] = physics;

  return ok;
}

status_v physics_terrain_ctor(physics_s *physics, vec3 pos,
                              mesh_s *terrain_model) {

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return fail;
  }

  if (physics->entity_type != EMPTY_EX0 || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return fail;
  }
  physics->__id = entity.count;
  physics->entity_type = MESH_EX0;

  physics->entity_body.mesh_ref = terrain_model;
  physics->position = pos;

  entity.bodies[entity.count] = physics;

  return ok;
}

// Destructor
void physics_dtor(physics_s *physics) {
  assert(physics != NULL);

  free(physics);
}

void physics_do_late(physics_s *physics) {
  assert(physics != NULL);

  /* physics->velocity = Vector3Zero(); */
}

void physics_add_force(physics_s *physics, vec3 force) {
  assert(physics != NULL);
  physics->force = vec3_add(physics->force, force);
}

void physics_scale_force(physics_s *physics, float by) {
  assert(physics != NULL);
  physics->force = vec3_scale(physics->force, by);
}

void physics_apply_force(physics_s *physics) {
  assert(physics != NULL);
  physics->velocity = vec3_add(physics->velocity, physics->force);
  physics->force = vec3_zero();
}

void physics_add_gravity(physics_s *physics, vec3 gravity) {
  assert(physics != NULL);
  physics->gravity = vec3_add(physics->gravity, gravity);
}

static void __handle_capsule_mesh(physics_s *physics, physics_s *against) {
  assert(physics != NULL);
  assert(against != NULL);

  mesh_s *mesh = against->entity_body.mesh_ref;

  vec3 original_capsulepos = physics->position;
  vec3 capsulepos = original_capsulepos;
  float height = vec3_len(vec3_sub(physics->entity_body.capsule.tip, physics->entity_body.capsule.base));

  float radius = physics->entity_body.capsule.radius;
  bool ground_intersection = false;
  uint8_t const ccd_max = 5;
 
  // colision information, if any
  intersect_result_s result = {0};
  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step = vec3_scale(physics->velocity, 1.0f / ccd_max * 0.016f);
    capsulepos = vec3_add(capsulepos, step);
    sphere_s s = {.center = capsulepos, .radius = radius};
    physics->entity_body.capsule = shapes_capsule_new(s, height);

    collision_check_capsule_mesh(physics->entity_body.capsule, mesh, &result);

    if (result.valid) {

      vec3 rNorm = result.normal;

      float velocityLen = vec3_len(physics->velocity);
      vec3 velocityNorm = vec3_norm(physics->velocity);
      vec3 undesiredMotion = vec3_scale(rNorm, vec3_dot(velocityNorm, rNorm));

      vec3 desiredMotion = vec3_sub(velocityNorm, undesiredMotion);
      physics->velocity = vec3_scale(desiredMotion, velocityLen);

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration)
      capsulepos =
          vec3_add(capsulepos, vec3_scale(rNorm, result.depth + 0.0001f));
    }
  }

  result = (intersect_result_s){0};

  // Gravity collision is separate:
  //	This is to avoid sliding down slopes and easier traversing of
  // slopes/stairs 	Unlike normal character motion collision, surface
  // sliding is not computed
  physics->gravity = vec3_add(physics->gravity, vec3_new(0, -GRAVITY * 0.3f, 0));

  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step = vec3_scale(physics->gravity, 1.0f / ccd_max * 0.016f);
    capsulepos = vec3_add(capsulepos, step);
    sphere_s s = {.center = capsulepos, .radius = radius};
    physics->entity_body.capsule = shapes_capsule_new(s, height);

    
    collision_check_capsule_mesh(physics->entity_body.capsule, mesh, &result);

    if (result.valid) {

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration):
      capsulepos = vec3_add(capsulepos,
                            vec3_scale(result.normal, result.depth + 0.0001f));

      // Check whether it is intersecting the ground (ground normal is
      // upwards)
      if (vec3_dot(result.normal, vec3_new(0.0f, 1.0f, 0.0f)) > 0.3f) {
        ground_intersection = true;
        physics->gravity = vec3_scale(physics->gravity, 0);
        break;
      }
    }
  }

  if (ground_intersection)
    physics->velocity = vec3_scale(physics->velocity, GROUND_FRICTION);
  else
    physics->velocity = vec3_scale(physics->velocity, AIR_FRICTION);

  physics->position =
      vec3_add(physics->position, vec3_sub(capsulepos, original_capsulepos));

  if (result.valid) {
    debug_draw_line(result.position, vec3_add(result.position, result.normal), vec3_new(1, 0, 0));
  }

  float vel = vec3_len(physics->velocity);
  static float max_vel = 0;
  if (vel > max_vel)
    max_vel = vel;

  text_draw(vec2_new(10, 10), 800- 10, vec3_new(0.2f, 0.7f, 0.9f), "Grounded: %s\nVelocity: %.2f\nPedro", ground_intersection ? "True" : "False", vec3_len(physics->velocity));
  
}

vec3 physics_get_pos(physics_s *physics) {
  assert(physics != NULL);
  vec3 pos = physics->position;

  if (MASK_CHK(physics->entity_type, CAPSULE_EX0) > 0)
    pos.y -= physics->entity_body.capsule.radius;

  return pos;
}

static
void __handle_sphere_mesh(physics_s *physics, physics_s *against) {

  assert(physics != NULL);
  assert(against != NULL);

  mesh_s *mesh = against->entity_body.mesh_ref;

  vec3 original_pos = physics->position;
  vec3 sphere_pos = original_pos;
  float radius = physics->entity_body.sphere.radius;
  /* bool ground_intersection = false; */
  uint8_t const ccd_max = 5;

  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step = vec3_scale(physics->velocity, 1.0f / ccd_max * 0.016f);
    sphere_pos = vec3_add(sphere_pos, step);
    sphere_s s = {.center = sphere_pos, .radius = radius};
    physics->entity_body.sphere = s;
    intersect_result_s result = {0};

    collision_check_sphere_mesh(physics->entity_body.sphere, mesh, &result);

    if (result.valid) {

      vec3 rNorm = result.normal;

      float velocityLen = vec3_len(physics->velocity);
      vec3 velocityNorm = vec3_norm(physics->velocity);
      vec3 undesiredMotion = vec3_scale(rNorm, vec3_dot(velocityNorm, rNorm));

      vec3 desiredMotion = vec3_sub(velocityNorm, undesiredMotion);
      physics->velocity = vec3_scale(desiredMotion, velocityLen);

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration)
      sphere_pos =
          vec3_add(sphere_pos, vec3_scale(rNorm, result.depth + 0.0001f));
    }
  }
}

capsule_s physics_get_capsule(physics_s *physics) {

  if (physics->entity_type != CAPSULE_EX0) {
    log_error("not a capsule body");
    return shapes_capsule_new((sphere_s){.center = vec3_zero(), .radius = 1.0f}, 2.0f);
  }

  return physics->entity_body.capsule;
}

vec3 physics_get_force(physics_s *physics) { return physics->force; }

vec3 physics_get_velocity(physics_s *physics) { return physics->velocity; }

void physics_draw(physics_s *physics) {
  assert(physics != NULL);

  /* if (physics->physics_entity_type == CAPSULE) */
  /*   DrawCapsule(physics->physics_entity_body.capsule); */
}

static void __handle_default(physics_s *physics, physics_s *against) {

  (void)physics;
  (void)against;
  log_warn("NOT IMPLEMENTED DEFAULT\n");
}

