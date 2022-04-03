#ifndef SM_EVENT_H
#define SM_EVENT_H

#include "smpch.h"

typedef enum {
  SM_EVENT_NONE = 0,

  /* key events */
  SM_EVENT_KEY_DOWN,
  SM_EVENT_KEY_UP,
  SM_EVENT_KEY_REPEAT,

  /* mouse events */
  SM_EVENT_MOUSE_DOWN,
  SM_EVENT_MOUSE_UP,
  SM_EVENT_MOUSE_MOVE,
  SM_EVENT_MOUSE_WHEEL,

  /* window events */
  SM_EVENT_WINDOW_RESIZE,
  SM_EVENT_WINDOW_CLOSE,
  SM_EVENT_WINDOW_FOCUS,
  SM_EVENT_WINDOW_UNFOCUS,

  SM_EVENT_MAX

} event_type_e;

typedef enum {

  SM_CATEGORY_NONE = 0,
  SM_CATEGORY_KEYBOARD = 1 << 0,
  SM_CATEGORY_MOUSE = 1 << 1,
  SM_CATEGORY_WINDOW = 1 << 2,
  SM_CATEGORY_ALL = SM_CATEGORY_KEYBOARD | SM_CATEGORY_MOUSE | SM_CATEGORY_WINDOW

} event_category_e;

#include "core/smKeyCode.h"
#include "core/smMouseCode.h"

typedef struct {

  event_type_e type;
  sm_key_code key; /* key pressed, key released, key repeat */

} event_key_s;

typedef struct {

  event_type_e type;
  sm_mouse_code button;
  int x;
  int y;
  int x_delta;
  int y_delta;
  float wheel;

} event_mouse_s;

typedef struct {

  event_type_e type;
  int width;
  int height;

} event_window_s;

typedef struct {

  event_category_e category;
  bool handled;

  union {
    event_key_s key;
    event_mouse_s mouse;
    event_window_s window;
  };

} event_s;

typedef bool (*event_handl)(event_s *event, void *user_data);

void event_print(event_s *event);
bool event_dispatch(event_s *event, event_type_e t, const event_handl func, void *user_data);
bool event_dispatch_categories(event_s *event, event_category_e t, const event_handl func, void *user_data);
void event_set_print_mask(event_category_e mask);

#define event_new_key(TYPE, KEY)                                                                                       \
  (event_s) {                                                                                                          \
    .category = SM_CATEGORY_KEYBOARD, .handled = false, .key = {.type = TYPE, .key = KEY }                             \
  }

#define event_new_mouse(TYPE, BUTTON, X, Y, X_DELTA, Y_DELTA, WHEEL)                                                   \
  (event_s) {                                                                                                          \
    .category = SM_CATEGORY_MOUSE, .handled = false, .mouse = {                                                        \
      .type = TYPE,                                                                                                    \
      .button = BUTTON,                                                                                                \
      .x = X,                                                                                                          \
      .y = Y,                                                                                                          \
      .x_delta = X_DELTA,                                                                                              \
      .y_delta = Y_DELTA,                                                                                              \
      .wheel = WHEEL                                                                                                   \
    }                                                                                                                  \
  }

#define event_new_window(TYPE, WIDTH, HEIGHT)                                                                          \
  (event_s) {                                                                                                          \
    .category = SM_CATEGORY_WINDOW, .handled = false, .window = {.type = TYPE, .width = WIDTH, .height = HEIGHT }      \
  }

#endif /* SM_EVENT_H */
