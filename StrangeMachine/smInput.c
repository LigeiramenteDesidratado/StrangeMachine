#include "smpch.h"

#include "core/smCore.h"

#define MAX_KEYBOARD_KEYS 350

typedef struct {

  i32 lock;
  b8 keyboard[MAX_KEYBOARD_KEYS];

  // mouse
  i32 x, y;           // x y mouse position
  i32 x_last, y_last; // last x y mouse position
  i32 x_rel, y_rel;   // relative x y mouse position

  f32 scroll;

} input_s;

static input_s *GINPUT = NULL;

void input_init(void) {
  SM_ASSERT(GINPUT == NULL && "input initialized twice");
  GINPUT = (input_s *)SM_CALLOC(1, sizeof(input_s));
  SM_ASSERT(GINPUT != NULL);
}

b8 input_scan_key(sm_key_code key) {
  SM_ASSERT(GINPUT != NULL);
  SM_ASSERT(key < MAX_KEYBOARD_KEYS);

  return GINPUT->keyboard[key];
}

b8 input_scan_key_lock(sm_key_code key) {
  SM_ASSERT(GINPUT != NULL);

  b8 val = false;

  if (GINPUT->lock == 0 && key < MAX_KEYBOARD_KEYS) {
    val = GINPUT->keyboard[key];
    if (val) {
      GINPUT->lock = 42;
    }
  }

  return val;
}

f32 input_get_mouse_scroll() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->scroll;
}

f32 input_get_x_rel_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x_rel;
}

f32 input_get_y_rel_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y_rel;
}

f32 input_get_x_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x;
}

f32 input_get_y_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y;
}

f32 input_get_x_last_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x_last;
}

f32 input_get_y_last_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y_last;
}

void input_do() {

  SM_ASSERT(GINPUT != NULL);

  GINPUT->y_rel = 0;
  GINPUT->x_rel = 0;
  GINPUT->x_last = GINPUT->x;
  GINPUT->y_last = GINPUT->y;
  /* GINPUT->x = 0; */
  /* GINPUT->y = 0; */
  GINPUT->scroll = 0;
  if (--GINPUT->lock < 0)
    GINPUT->lock = 0;
}

b8 input_on_event(event_s *event, void *user_data) {

  SM_ASSERT(GINPUT != NULL);
  SM_UNUSED(user_data);

  switch (event->category) {

  case SM_CATEGORY_KEYBOARD:

    switch (event->key.type) {
    case SM_EVENT_KEY_REPEAT: /* fallthrough */
    case SM_EVENT_KEY_DOWN:
      GINPUT->keyboard[event->key.key] = true;
      return true;
    case SM_EVENT_KEY_UP:
      GINPUT->keyboard[event->key.key] = false;
      return true;
    default:
      SM_LOG_WARN("unhandled keyboard event");
      return false;
    }
  case SM_CATEGORY_MOUSE:

    switch (event->mouse.type) {
    case SM_EVENT_MOUSE_MOVE:
      GINPUT->x = event->mouse.x;
      GINPUT->y = event->mouse.y;
      GINPUT->x_rel = event->mouse.x_delta;
      GINPUT->y_rel = event->mouse.y_delta;
      return true;
    case SM_EVENT_MOUSE_WHEEL:
      GINPUT->scroll = event->mouse.wheel;
      return true;
    case SM_EVENT_MOUSE_DOWN:
      GINPUT->keyboard[event->mouse.button] = true;
      return true;
    case SM_EVENT_MOUSE_UP:
      GINPUT->keyboard[event->mouse.button] = false;
      return true;
    default:
      SM_LOG_WARN("unhandled mouse event");
      return false;
    }
  default:
    SM_LOG_WARN("unhandled event");
    return false;
  }
}

void input_tear_down(void) {
  SM_ASSERT(GINPUT != NULL && "trying to finilize a not initialized input");
  SM_FREE(GINPUT);
  GINPUT = NULL;
}
