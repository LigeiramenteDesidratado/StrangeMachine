#ifndef SM_LAYER_H
#define SM_LAYER_H

#include "core/smWindowPub.h"

typedef void (*layer_on_attach_f)(void *user_data);
typedef void (*layer_on_detach_f)(void *user_data);
typedef void (*layer_on_update_f)(void *user_data, float dt);
typedef void (*layer_on_gui_f)(void *user_data);

typedef struct {

  char *name;

  void *user_data;

  layer_on_attach_f on_attach;
  layer_on_detach_f on_detach;
  layer_on_update_f on_update;
  layer_on_gui_f on_gui;

  event_callback_f on_event;

} layer_s;

#endif /* SM_LAYER_H */
