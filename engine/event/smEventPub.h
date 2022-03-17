#ifndef SM_EVENT_PUB_H
#define SM_EVENT_PUB_H

/* #include "math/smMath.h" */
/* #include "scene_defs.h" */
#include <stdbool.h>

typedef enum {
  MOUSE,
  KEYBOARD,
  JOYSTICK,
} event_e;

// Polymorphic behavior functions
typedef int (*get_category_flag)(void *ptr);
typedef event_e (*get_event_type)(void *ptr);

typedef struct {
  const char *name;
  bool handled;

  get_category_flag get_category_flag;
  get_event_type get_event_type;

} scene_s;

#endif /* SM_EVENT_PUB_H */
