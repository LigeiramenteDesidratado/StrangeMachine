#ifndef SCENE_P_H
#define SCENE_P_H

#include "scene_defs.h"
#include "math/vec3.h"


// Polymorphic behavior functions
typedef void (*do_f)(void *ptr, float dt);
/* typedef void (*do_late_t)(void *ptr); */
typedef void (*draw_f)(void *ptr);
typedef vec3 (*get_look_at_f)(void *ptr);

/*
   The scene_s "class" has three behavior functions. The draw_f, do_f and get_look_at_f functions
   are supposed to be polymorphic and can be overridden by the child classes.
*/
typedef struct {
  // id will works like a runtime type check
  SCENE_EX6 __id;

  do_f scene_do;
  /* do_late_t scene_do_late; */
  draw_f scene_draw;
  /* draw_late_t scene_draw_late; */
  get_look_at_f scene_get_look_at;

} scene_s;

#endif

