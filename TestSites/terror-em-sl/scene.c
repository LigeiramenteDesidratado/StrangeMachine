#include "math/smMath.h"
#include "scene_p.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void __scene_do(void *this_ptr, float dt) {
  /* scene_t* scene = (scene_t*)this_ptr; */
  (void)this_ptr;
  (void)dt;

  printf("Do nothing. You should overwrite this function\n");
}

void __scene_draw(void *this_ptr) {
  /* scene_t* scene = (scene_t*)this_ptr; */
  (void)this_ptr;

  printf("Draw nothing. You should overwrite this function\n");
}

void __scene_get_look_at(void *this_ptr, vec3 out) {
  /* scene_t* scene = (scene_t*)this_ptr; */
  (void)this_ptr;

  printf("Return nothing. You should overwrite this function\n");
  glm_vec3_copy(out, (vec3){0.0f, 0.0f, 0.0f});
}

scene_s *scene_new(void) {
  scene_s *scene = calloc(1, sizeof(scene_s));
  assert(scene != NULL);

  return scene;
}

void scene_ctor(scene_s *scene, scenes_e id) {
  assert(scene != NULL);

  // Set the function pointer to point to the default definition
  scene->__id = id;
  scene->scene_do = __scene_do;
  scene->scene_draw = __scene_draw;
  scene->scene_get_look_at = __scene_get_look_at;
}

void scene_dtor(scene_s *scene) {}

void scene_do(scene_s *scene, float dt) {
  assert(scene != NULL);

  scene->scene_do(scene, dt);
}

void scene_draw(scene_s *scene) {
  assert(scene != NULL);

  scene->scene_draw(scene);
}

void scene_get_look_at(scene_s *scene, vec3 out) {
  assert(scene != NULL);

  scene->scene_get_look_at(scene, out);
}

scenes_e scene_get_id(scene_s *scene) {
  assert(scene != NULL);

  return scene->__id;
}
