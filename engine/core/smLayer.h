#ifndef SM_LAYER_H
#define SM_LAYER_H

#include "core/smWindowPub.h"

typedef struct {

  char *name;

  void *user_data;
  event_callback_f on_event;

} layer_s;

#endif /* SM_LAYER_H */
