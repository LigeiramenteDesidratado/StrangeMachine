#include "util/bitmask.h"
#include "util/common.h"

#include "smCollision.h"
#include "smMem.h"
#include "smMesh.h"
#include "smShapes.h"
#include "smText.h"

#define GRAVITY         3.1f
#define GROUND_FRICTION 0.8f
#define AIR_FRICTION    0.9f

void physics_init(void) {
  // TODO: implement
}

typedef enum {
  EMPTY = (0 << 0),   /* 0b00000000 0 */
  SPHERE = (1 << 0),  /* 0b00000001 1 */
  CAPSULE = (1 << 1), /* 0b00000010 2 */
  MESH = (1 << 2),    /* 0b00000100 4 */
} body_e;

typedef struct {

  int8_t __id;
  body_e entity_type;

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

static void (*handler[SPHERE | CAPSULE | MESH])(physics_s *physics, physics_s *against) = {
    [SPHERE | SPHERE] = __handle_default,  // TODO: implement
    [SPHERE | CAPSULE] = __handle_default, // TODO: implement...
    [SPHERE | MESH] = __handle_sphere_mesh,
    [CAPSULE | CAPSULE] = __handle_default, // TODO: implement...
    [CAPSULE | MESH] = __handle_capsule_mesh,
    [MESH | MESH] = __handle_default, // TODO: implement...
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
      handler[entity.bodies[i]->entity_type | entity.bodies[j]->entity_type](entity.bodies[i], entity.bodies[j]);
    }
  }
}

// Allocate memory
physics_s *physics_new(void) {
  physics_s *physics = (physics_s *)SM_CALLOC(1, sizeof(physics_s));

  assert(physics != NULL);

  return physics;
}

// Sphere Constructor
bool physics_sphere_ctor(physics_s *physics, vec3 pos, float radius) {

  assert(physics != NULL);
  assert(radius > 0);

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return false;
  }

  if (physics->entity_type != EMPTY || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return false;
  }
  physics->__id = entity.count;
  physics->entity_type = SPHERE;

  physics->entity_body.sphere.center = pos;
  physics->entity_body.sphere.radius = radius;

  entity.bodies[entity.count] = physics;

  return true;
}

bool physics_capsule_ctor(physics_s *physics, vec3 pos, float radius, float height) {

  assert(physics != NULL);
  assert(radius > 0.0f && "negative radius");
  assert(height > 0.0f && "negative height");

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return false;
  }

  if (physics->entity_type != EMPTY || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return false;
  }
  physics->__id = entity.count;
  physics->entity_type = CAPSULE;

  glm_vec3_copy(pos, physics->position);

  sphere_s s; /* {pos, radius}; */
  s.radius = radius;
  glm_vec3_copy(pos, s.center);
  physics->entity_body.capsule = shapes_capsule_new(s, height);

  entity.bodies[entity.count] = physics;

  return true;
}

bool physics_terrain_ctor(physics_s *physics, vec3 pos, mesh_s *terrain_model) {

  if (++entity.count == INT8_MAX) {
    --entity.count;
    return false;
  }

  if (physics->entity_type != EMPTY || physics->__id != 0) {
    --entity.count;
    log_warn("NON EMPTY PHYSICS BODY!\n");
    return false;
  }
  physics->__id = entity.count;
  physics->entity_type = MESH;

  physics->entity_body.mesh_ref = terrain_model;
  glm_vec3_copy(pos, physics->position);

  entity.bodies[entity.count] = physics;

  return true;
}

// Destructor
void physics_dtor(physics_s *physics) {
  assert(physics != NULL);

  SM_FREE(physics);
}

void physics_do_late(physics_s *physics) {
  assert(physics != NULL);

  /* physics->velocity = Vector3Zero(); */
}

void physics_add_force(physics_s *physics, vec3 force) {
  assert(physics != NULL);
}

void physics_scale_force(physics_s *physics, float by) {
  assert(physics != NULL);
  glm_vec3_scale(physics->force, by, physics->force);
}

void physics_apply_force(physics_s *physics) {
  assert(physics != NULL);
  /* physics->velocity = vec3_add(physics->velocity, physics->force); */
  glm_vec3_add(physics->velocity, physics->force, physics->velocity);
  glm_vec3_zero(physics->force);
  /* physics->force = vec3_zero(); */
}

void physics_add_gravity(physics_s *physics, vec3 gravity) {
  assert(physics != NULL);
  glm_vec3_add(physics->gravity, gravity, physics->gravity);
}

static void __handle_capsules(physics_s *physics, physics_s *against) {
  SM_ASSERT(physics != NULL);
  SM_ASSERT(against != NULL);

  intersect_result_s result = {0};

  // retrieve intersection result
  collision_check_capsules(physics->entity_body.capsule, against->entity_body.capsule, &result);
  // if collided then apply impulse to both bodies
  if (result.valid) {
    vec3 impulse;
    glm_vec3_scale(result.normal, result.depth, impulse);
    /* vec3 impulse = vec3_scale(result.normal, result.depth); */
    glm_vec3_add(physics->velocity, impulse, physics->velocity);
    glm_vec3_sub(against->velocity, impulse, against->velocity);
  }
}

// TODO: improve this
static void __handle_spheres(physics_s *physics, physics_s *against) {
  SM_ASSERT(physics != NULL);
  SM_ASSERT(against != NULL);

  intersect_result_s result = {0};

  // retrieve intersection result
  collision_check_spheres(physics->entity_body.sphere, against->entity_body.sphere, &result);
  // if collided then apply impulse to both bodies
  if (result.valid) {
    vec3 impulse;
    glm_vec3_scale(result.normal, result.depth, impulse);

    glm_vec3_add(physics->velocity, impulse, physics->velocity);
    glm_vec3_sub(against->velocity, impulse, against->velocity);
  }
}

static void __handle_capsule_mesh(physics_s *physics, physics_s *against) {
  assert(physics != NULL);
  assert(against != NULL);

  mesh_s *mesh = against->entity_body.mesh_ref;

  vec3 original_capsulepos;
  glm_vec3_copy(physics->position, original_capsulepos);
  vec3 capsulepos;
  glm_vec3_copy(original_capsulepos, capsulepos);

  vec3 c;
  glm_vec3_sub(physics->entity_body.capsule.tip, physics->entity_body.capsule.base, c);
  float height = glm_vec3_norm(c);

  float radius = physics->entity_body.capsule.radius;
  bool ground_intersection = false;
  uint8_t const ccd_max = 5;

  // colision information, if any
  intersect_result_s result = {0};
  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step;
    glm_vec3_scale(physics->velocity, 1.0f / ccd_max * 0.016f, step);
    glm_vec3_add(capsulepos, step, capsulepos);
    sphere_s s; /*{.center = capsulepos, .radius = radius}; */
    s.radius = radius;
    glm_vec3_copy(capsulepos, s.center);
    physics->entity_body.capsule = shapes_capsule_new(s, height);

    collision_check_capsule_mesh(physics->entity_body.capsule, mesh, &result);

    if (result.valid) {

      vec3 rNorm;
      glm_vec3_copy(result.normal, rNorm);

      float velocityLen = glm_vec3_norm(physics->velocity);

      vec3 velocityNorm;
      glm_vec3_normalize_to(physics->velocity, velocityNorm);

      vec3 undesiredMotion;
      glm_vec3_scale(rNorm, glm_vec3_dot(velocityNorm, rNorm), undesiredMotion);

      vec3 desiredMotion;
      glm_vec3_sub(velocityNorm, undesiredMotion, desiredMotion);

      glm_vec3_scale(desiredMotion, velocityLen, physics->velocity);

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration)
      vec3 penetration;
      glm_vec3_scale(rNorm, result.depth + 0.0001f, penetration);
      glm_vec3_add(capsulepos, penetration, capsulepos);
    }
  }

  result = (intersect_result_s){0};

  // Gravity collision is separate:
  //	This is to avoid sliding down slopes and easier traversing of
  // slopes/stairs 	Unlike normal character motion collision, surface
  // sliding is not computed
  physics->gravity = vec3_add(physics->gravity, vec3_new(0, -GRAVITY * 0.3f, 0));
  glm_vec3_add(physics->gravity, vec3_new(0, -GRAVITY * 0.3f, 0), physics->gravity);

  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step;
    glm_vec3_scale(physics->gravity, 1.0f / ccd_max * 0.016f, step);
    glm_vec3_add(capsulepos, step, capsulepos);
    /* sphere_s s = {.center = capsulepos, .radius = radius}; */
    sphere_s s;
    s.radius = radius;
    glm_vec3_copy(capsulepos, s.center);
    physics->entity_body.capsule = shapes_capsule_new(s, height);

    collision_check_capsule_mesh(physics->entity_body.capsule, mesh, &result);

    if (result.valid) {

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration):
      vec3 penetration;
      glm_vec3_scale(result.normal, result.depth + 0.0001f, penetration);
      glm_vec3_add(capsulepos, penetration, capsulepos);

      // Check whether it is intersecting the ground (ground normal is
      // upwards)
      if (glm_vec3_dot(result.normal, vec3_new(0.0f, 1.0f, 0.0f)) > 0.3f) {
        ground_intersection = true;
        glm_vec3_scale(physics->gravity, 0, physics->gravity);
        break;
      }
    }
  }

  if (ground_intersection)
    glm_vec3_scale(physics->velocity, GROUND_FRICTION, physics->velocity);
  else
    glm_vec3_scale(physics->velocity, AIR_FRICTION, physics->velocity);

  vec3 sub;
  glm_vec3_sub(capsulepos, original_capsulepos, sub);
  glm_vec3_add(physics->position, sub, physics->position);

  /* if (result.valid) { */
  /* debug_draw_line(result.position, vec3_add(result.position, result.normal), vec3_new(1, 0, 0)); */
  /* } */

  float vel = glm_vec3_norm(physics->velocity);
  static float max_vel = 0;
  if (vel > max_vel)
    max_vel = vel;

  text_draw(vec2_new(10, 10), 800 - 10, vec3_new(0.2f, 0.7f, 0.9f), "Grounded: %s\nVelocity: %.2f\n",
            ground_intersection ? "True" : "False", glm_vec3_norm(physics->velocity));
}

vec3 physics_get_pos(physics_s *physics) {
  assert(physics != NULL);
void physics_get_pos(physics_s *physics, vec3 out) {

  glm_vec3_copy(physics->position, out);

  return pos;
  if (MASK_CHK(physics->entity_type, CAPSULE) > 0)
    out[1] -= physics->entity_body.capsule.radius;
}

static void __handle_sphere_mesh(physics_s *physics, physics_s *against) {

  assert(physics != NULL);
  assert(against != NULL);

  mesh_s *mesh = against->entity_body.mesh_ref;

  vec3 original_pos;
  vec3 sphere_pos;

  glm_vec3_copy(physics->position, original_pos);
  glm_vec3_copy(original_pos, sphere_pos);

  float radius = physics->entity_body.sphere.radius;
  /* bool ground_intersection = false; */
  uint8_t const ccd_max = 5;

  for (uint8_t i = 0; i < ccd_max; ++i) {

    vec3 step;
    glm_vec3_scale(physics->velocity, 1.0f / ccd_max * 0.016f, step);
    glm_vec3_add(sphere_pos, step, sphere_pos);
    sphere_s s;
    /* sphere_s s = {.center = sphere_pos, .radius = radius}; */
    s.radius = radius;
    glm_vec3_copy(sphere_pos, s.center);
    physics->entity_body.sphere = s;
    intersect_result_s result = {0};

    collision_check_sphere_mesh(physics->entity_body.sphere, mesh, &result);

    if (result.valid) {

      vec3 rNorm;
      glm_vec3_copy(result.normal, rNorm);

      float velocityLen = glm_vec3_norm(physics->velocity);
      vec3 velocityNorm;
      glm_vec3_normalize_to(physics->velocity, velocityNorm);
      vec3 undesiredMotion;
      glm_vec3_scale(rNorm, glm_vec3_dot(velocityNorm, rNorm), undesiredMotion);

      vec3 desiredMotion;
      glm_vec3_sub(velocityNorm, undesiredMotion, desiredMotion);
      glm_vec3_scale(desiredMotion, velocityLen, physics->velocity);

      // Remove penetration (penetration epsilon added to handle infinitely
      // small penetration)
      vec3 scal;
      glm_vec3_scale(rNorm, result.depth + 0.0001f, scal);
      glm_vec3_add(sphere_pos, scal, sphere_pos);
    }
  }
}

capsule_s physics_get_capsule(physics_s *physics) {

  if (physics->entity_type != CAPSULE) {
    log_error("not a capsule body");
    sphere_s s;
    s.radius = 1.0f;
    glm_vec3_copy(s.center, vec3_new(0.0f, 0.0f, 0.0f));
    return shapes_capsule_new(s, 2.0f);
  }

  return physics->entity_body.capsule;
}

vec3 physics_get_force(physics_s *physics) {
  return physics->force;
sphere_s physics_get_sphere(physics_s *physics) {

  if (physics->entity_type != SPHERE) {
    SM_LOG_ERROR("not a sphere body");
    sphere_s s;
    s.radius = 1.0f;
    glm_vec3_copy(s.center, vec3_new(0.0f, 0.0f, 0.0f));
    return s;
  }

  return physics->entity_body.sphere;
}

vec3 physics_get_velocity(physics_s *physics) {
  return physics->velocity;
void physics_get_force(physics_s *physics, vec3 out) {
  glm_vec3_copy(physics->force, out);
}

void physics_get_velocity(physics_s *physics, vec3 out) {
  glm_vec3_copy(physics->velocity, out);
}

void physics_draw(physics_s *physics) {
  assert(physics != NULL);

  /* if (physics->physics_entity_type == CAPSULE) */
  /*   DrawCapsule(physics->physics_entity_body.capsule); */
}

static char const *__physics_type_to_str(body_e type) {
  switch (type) {
  case CAPSULE:
    return "capsule";
  case SPHERE:
    return "sphere";
  case MESH:
    return "mesh";
  default:
    return "unknown";
  }
}

static void __handle_default(physics_s *physics, physics_s *against) {

  (void)physics;
  (void)against;
  log_warn("NOT IMPLEMENTED DEFAULT\n");
}
