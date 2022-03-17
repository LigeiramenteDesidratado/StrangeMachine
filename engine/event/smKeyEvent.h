#ifndef SM_KEY_EVENT_H
#define SM_KEY_EVENT_H

#include "core/smKeyCode.h"
#include "smEvent.h"

typedef struct {
  event_type_s type;
  sm_key_code key_code;

} key_event_s;

typedef key_event_s key_pressed_event;
typedef key_event_s key_released_event;

#define key_pressed_event_new(key_code)                                                                                \
  (key_pressed_event) {                                                                                                \
    .type = SM_EVENT_KEY_DOWN, .key_code = key_code                                                                    \
  }

#define key_released_event_new(key_code)                                                                               \
  (key_released_event) {                                                                                               \
    .type = SM_EVENT_KEY_UP, .key_code = key_code                                                                      \
  }

#endif /* SM_KEY_EVENT_H */
