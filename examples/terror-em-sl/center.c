#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "player.h"
#include "scene.h"
#include "scene_p.h"
#include "smModel.h"
#include "smPhysics.h"
#include "smShader.h"
#include "smShaderProgram.h"

static void __one_do(void *ptr, float dt);
static void __one_draw(void *ptr);
static vec3 __one_get_look_at(void *ptr);

typedef struct {

  // Is mandatory that scene_s type be the first item in the attribute
  // structure.
  scene_s scene;

  /* struct background_t *bg; */
  struct player_s *player;
  struct model_s *world;
  struct physics_s *terrain;
  /* struct world_t *world; */

} center_s;

center_s *center_new(void) {
  center_s *center = calloc(1, sizeof(center_s));
  assert(center != NULL);

  return center;
}

bool center_ctor(center_s *center, scenes_e id) {

  scene_ctor((struct scene_s *)center, id);
  center->scene.scene_do = __one_do;
  center->scene.scene_draw = __one_draw;
  center->scene.scene_get_look_at = __one_get_look_at;

  struct player_s *player = player_new();
  if (!player_ctor(player)) {
    return false;
  }
  center->player = player;

  struct model_s *model = model_new();
  if (!model_ctor(model, "plane.obj", "ps1.png")) {
    return false;
  }
  center->world = model;

  struct physics_s *terrain = physics_new();
  if (!physics_terrain_ctor(terrain, vec3_zero(), *model_get_meshes(center->world))) {
    return false;
  }
  center->terrain = terrain;
  return true;
}

void center_dtor(center_s *center) {
  assert(center != NULL);
  player_dtor(center->player);
  physics_dtor(center->terrain);
  model_dtor(center->world);

  free(center);
  center = NULL;
}

static void __one_do(void *ptr, float dt) {

  assert(ptr != NULL);

  center_s *center = (center_s *)ptr;

  player_do(center->player, dt);
  model_do(center->world);
}

static void __one_draw(void *ptr) {

  assert(ptr != NULL);
  center_s *center = (center_s *)ptr;

  player_draw(center->player);
  model_draw(center->world);

  player_draw_debug(center->player);
}

static void __one_get_look_at(void *ptr, vec3 out) {

  assert(ptr != NULL);
  center_s *center = (center_s *)ptr;

  /* glm_vec3_zero(out); */
  player_get_position(center->player, out);
}
