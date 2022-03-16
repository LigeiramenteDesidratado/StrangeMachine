#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cglm/quat.h"
#include "smCamera.h"
#include "smDebug.h"
#include "smInput.h"
#include "smPhysics.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smSkinnedModel.h"
#include "smText.h"
#include "smUniform.h"

#define MAX_BODIES 4

typedef enum {
  IDLE = 0x00,
  WALK = 0x01,
  RUN = 0x02,

} state_e;

char *state_str[3] = {
    "idle",
    "walk",
    "run.001",
};

typedef struct {
  struct skinned_model_s *model;
  struct physics_s *physics;
  /* struct physics_s **body; */
  struct physics_s *sphere;
  transform_s transform;
  transform_s btransform;
  state_e state;

} player_s;

player_s *player_new(void) {
  player_s *player = calloc(1, sizeof(player_s));

  assert(player != NULL);

  return player;
}

bool player_ctor(player_s *player) {

  struct skinned_model_s *model = skinned_model_new();
  if (!skinned_model_ctor2(model, "goth.glb", "goth.png")) {
    return false;
  }
  player->model = model;

  player->transform = transform_zero();
  player->btransform = transform_zero();
  player->state = IDLE;

  struct physics_s *physics = physics_new();
  if (!physics_capsule_ctor(physics, vec3_new(-1.0f, 9.0f, 0.0f), 0.9f, 5.3f)) {
    return false;
  }
  player->physics = physics;

  /* player->body = calloc(MAX_BODIES, sizeof(struct physics_s *)); */
  /* for (size_t i = 0; i < MAX_BODIES; ++i) { */
  /* struct physics_s *body = physics_new(); */
  /* if (!physics_capsule_ctor(body, vec3_new(i*1, 9.0f, 0.0f), 0.9f, 5.3f)) { */
  /* return false; */
  /* } */
  /* player->body[i] = body; */
  /* } */

  /* struct physics_s *sphere = physics_new(); */
  /* if (!physics_sphere_ctor(sphere, vec3_new(0.0f, 5.0f, 0.0f), 4.0f)) { */
  /* return false; */
  /* } */
  /* player->sphere = sphere; */

  return true;
}

void player_dtor(player_s *player) {

  assert(player != NULL);

  skinned_model_dtor(player->model);
  physics_dtor(player->physics);

  /* for (size_t i = 0; i < MAX_BODIES; ++i) { */
  /* physics_dtor(player->body[i]); */
  /* } */

  /* physics_dtor(player->sphere); */

  free(player);
}

void player_get_transformation_mat4(player_s *player, mat4 out) {
  assert(player != NULL);
  transform_to_mat4(player->transform, out);
}

void player_do(player_s *player, float dt) {
  assert(player != NULL);

  skinned_model_do(player->model, dt);

  // Get the left/right/forward/backward.
  // They are mapped to the A,S,D,W keys.
  // The keyboard value will always be -1, 0 or 1
  vec3 input;
  glm_vec3_copy(vec3_new((float)(input_scan_key(sm_key_a) + -input_scan_key(sm_key_d)), 0.f,
                         (float)(input_scan_key(sm_key_w) + -input_scan_key(sm_key_s))),
                input);

  vec3 direction;
  glm_vec3_normalize_to(input, direction);
  float dir_len = glm_vec3_norm(direction);
  if (dir_len != 0) {

    mat4 view;
    camera_get_view(view);
    vec3 fwd;
    fwd[0] = view[2][0];
    fwd[1] = 0.0f;
    fwd[2] = -view[2][2];
    vec3 right;
    right[0] = -view[0][0];
    right[1] = 0.0f;
    right[2] = view[0][2];

    glm_vec3_scale(fwd, direction[2], fwd);
    glm_vec3_scale(right, direction[0], right);

    glm_vec3_add(fwd, right, direction);

    glm_vec3_normalize(direction);
    float yaw = atan2f(direction[0], direction[2]);

    versor rotation;
    glm_quatv(rotation, yaw, vec3_new(0.0f, 1.0f, 0.0f));
    /* versor rotation = quat_angle_axis(yaw, vec3_new(0, 1.0f, 0)); */

    /* player->transform.rotation = quat_slerp2(player->transform.rotation, rotation, 15 * dt); */
    glm_quat_slerp(player->transform.rotation, rotation, 15 * dt, player->transform.rotation);

    player->state = WALK;
  } else
    player->state = IDLE;

  float sprint = 1;
  if (input_scan_key(sm_key_lshift) && dir_len != 0) {
    sprint = 1.9f;
    player->state = RUN;
  }
  vec3 force;
  glm_vec3_scale(direction, 1.1f * sprint, force);
  physics_add_force(player->physics, force);

  static bool can_jump = true;
  vec3 vel;
  physics_get_velocity(player->physics, vel);
  if (vel[1] < 0.1f) {
    can_jump = true;
  }

  if (input_scan_key(sm_key_space) && can_jump) {
    physics_add_force(player->physics, vec3_new(0.0f, 40.0f, 0.0f));
    can_jump = false;
  }

  physics_apply_force(player->physics);
  /* vec3 velocity = vec3_scale(direction, 4 * sprint); */
  /* vec3 moveamount = vec3_scale(velocity, dt); */
  physics_step();
  physics_get_pos(player->physics, player->transform.position);

  /* player->transform.position = vec3_add(player->transform.position,
   * moveamount); */

  if (!skinned_model_set_animation(player->model, state_str[player->state]))
    printf("invalid animation name: %s\n", state_str[player->state]);

  /* text_draw(vec2_new(10, 50), 800- 10, vec3_new(6.0f, 0.9f, 0.0f),
   * "Player\ndirection: %.2f\nvelocity: %.2f\nlocation: %.2f, %.2f, %.2f",
   * vec3_len(direction), vec3_len(velocity), player->transform.position.x,
   * player->transform.position.y, player->transform.position.z); */
}

void player_draw(player_s *player) {
  assert(player != NULL);

  shader_bind(SHADERS[SKINNED_SHADER]);
  /* player->transform.scale = vec3_new(1.0f, 1.0f, 1.0f); */
  mat4 p_trans;
  transform_to_mat4(player->transform, p_trans);
  uniform_set_value(glGetUniformLocation(SHADERS[SKINNED_SHADER], "model"), p_trans);
  skinned_model_draw(player->model);
  shader_unbind();
}

void player_draw_debug(player_s *player) {
  assert(player != NULL);

  capsule_s c = physics_get_capsule(player->physics);
  debug_draw_capsule(c);

  /* sphere_s s = physics_get_sphere(player->sphere); */
  /* debug_draw_sphere(s, vec3_new(0.3, 0.4, 0.5)); */

  /* for (size_t i = 0; i < MAX_BODIES; ++i) { */
  /* capsule_s body = physics_get_capsule(player->body[i]); */
  /* debug_draw_capsule(body); */
  /* } */
}

void player_get_position(player_s *player, vec3 out) {
  assert(player != NULL);

  glm_vec3_copy(player->transform.position, out);
}
