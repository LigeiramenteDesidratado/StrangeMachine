#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "math/transform.h"
#include "smCamera.h"
#include "smDebug.h"
#include "smInput.h"
#include "smPhysics.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smSkinnedModel.h"
#include "smText.h"
#include "smUniform.h"

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
  transform_s transform;
  state_e state;

} player_s;

player_s *player_new(void) {
  player_s *player = calloc(1, sizeof(player_s));

  assert(player != NULL);

  return player;
}

bool player_ctor(player_s *player) {

  struct skinned_model_s *model = skinned_model_new();
  if (!skinned_model_ctor(model, "goth.glb", "goth.png")) {
    return false;
  }
  player->model = model;

  player->transform = transform_zero();
  player->state = IDLE;

  struct physics_s *physics = physics_new();
  if (!physics_capsule_ctor(physics, vec3_new(0.0f, 9.0f, 0.0f), 0.9f, 5.3f)) {
    return false;
  }
  player->physics = physics;

  return true;
}

void player_dtor(player_s *player) {

  assert(player != NULL);

  skinned_model_dtor(player->model);
  physics_dtor(player->physics);

  free(player);
}

mat4 player_get_transformation_mat4(player_s *player) {
  assert(player != NULL);
  return transform_to_mat4(player->transform);
}

void player_do(player_s *player, float dt) {
  assert(player != NULL);

  skinned_model_do(player->model, dt);

  // Get the left/right/forward/backward.
  // They are mapped to the A,S,D,W keys.
  // The keyboard value will always be -1, 0 or 1
  vec3 input = vec3_new((float)(input_scan_key(SDL_SCANCODE_A) + -input_scan_key(SDL_SCANCODE_D)), 0.f,
                        (float)(input_scan_key(SDL_SCANCODE_W) + -input_scan_key(SDL_SCANCODE_S)));

  vec3 direction = vec3_norm(input);
  float dir_len = vec3_len(direction);
  if (dir_len != 0) {

    mat4 view = camera_get_view();
    vec3 fwd = vec3_new(view.forward.x, 0, -view.forward.z);
    vec3 right = vec3_new(-view.right.x, 0, view.right.z);

    direction = vec3_add(vec3_scale(fwd, direction.z), vec3_scale(right, direction.x));

    direction = vec3_norm(direction);
    float yaw = atan2f(direction.x, direction.z);

    quat rotation = quat_angle_axis(yaw, vec3_new(0, 1.0f, 0));
    player->transform.rotation = quat_slerp2(player->transform.rotation, rotation, 15 * dt);

    player->state = WALK;
  } else
    player->state = IDLE;

  float sprint = 1;
  if (input_scan_key(SDL_SCANCODE_LSHIFT) && dir_len != 0) {
    sprint = 1.9f;
    player->state = RUN;
  }

  physics_add_force(player->physics, vec3_scale(direction, 1.1f * sprint));

  physics_apply_force(player->physics);
  /* vec3 velocity = vec3_scale(direction, 4 * sprint); */
  /* vec3 moveamount = vec3_scale(velocity, dt); */
  physics_step();
  player->transform.position = physics_get_pos(player->physics);

  /* player->transform.position = vec3_add(player->transform.position,
   * moveamount); */

  if (!skinned_model_set_animation(player->model, state_str[player->state]))
    printf("invalid animation name: %s", state_str[player->state]);

  /* text_draw(vec2_new(10, 50), 800- 10, vec3_new(6.0f, 0.9f, 0.0f),
   * "Player\ndirection: %.2f\nvelocity: %.2f\nlocation: %.2f, %.2f, %.2f",
   * vec3_len(direction), vec3_len(velocity), player->transform.position.x,
   * player->transform.position.y, player->transform.position.z); */
}

void player_draw(player_s *player) {
  assert(player != NULL);

  shader_bind(SHADERS[SKINNED_SHADER]);
  mat4 p_trans = transform_to_mat4(player->transform);
  uniform_set_value(glGetUniformLocation(SHADERS[SKINNED_SHADER], "model"), p_trans);
  skinned_model_draw(player->model);
  shader_unbind();
}

void player_draw_debug(player_s *player) {
  assert(player != NULL);

  capsule_s c = physics_get_capsule(player->physics);

  debug_draw_capsule(c);
}

vec3 player_get_position(player_s *player) {
  assert(player != NULL);

  return player->transform.position;
}
