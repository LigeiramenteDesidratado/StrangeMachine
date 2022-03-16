#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "util/common.h"

#include "smMem.h"

#include <SDL2/SDL_events.h>

#define MAX_KEYBOARD_KEYS 350

typedef struct {

  int32_t lock;
  bool keyboard[MAX_KEYBOARD_KEYS];

  // mouse
  int32_t x, y;           // x y mouse position
  int32_t x_last, y_last; // last x y mouse position
  int32_t x_rel, y_rel;   // relative x y mouse position

  float scroll;

} input_s;

static input_s *GINPUT = NULL;

void input_init(void) {
  SM_ASSERT(GINPUT == NULL && "input initialized twice");
  GINPUT = (input_s *)SM_CALLOC(1, sizeof(input_s));
  SM_ASSERT(GINPUT != NULL);
}

bool input_scan_key(int32_t key) {
  SM_ASSERT(GINPUT != NULL);
  SM_ASSERT(key < MAX_KEYBOARD_KEYS);

  return GINPUT->keyboard[key];
}

bool input_scan_key_lock(int32_t key) {
  SM_ASSERT(GINPUT != NULL);

  bool val = false;

  if (GINPUT->lock == 0 && key < MAX_KEYBOARD_KEYS) {
    val = GINPUT->keyboard[key];
    if (val) {
      GINPUT->lock = 42;
    }
  }

  return val;
}

float input_get_mouse_scroll() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->scroll;
}

float input_get_x_rel_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x_rel;
}

float input_get_y_rel_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y_rel;
}

float input_get_x_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x;
}

float input_get_y_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y;
}

float input_get_x_last_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->x_last;
}

float input_get_y_last_mouse() {
  SM_ASSERT(GINPUT != NULL);
  return GINPUT->y_last;
}

void input_before_do() {
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

void input_do(SDL_Event *e) {
  SM_ASSERT(GINPUT != NULL);

  if (e->type == SDL_KEYUP) {
    // check if the keyboard event was a result of  Keyboard repeat event
    if (e->key.repeat == 0 && e->key.keysym.scancode < MAX_KEYBOARD_KEYS) {
      GINPUT->keyboard[e->key.keysym.scancode] = false;
    }
  } else if (e->type == SDL_KEYDOWN) {
    // check if the keyboard event was a result of  Keyboard repeat event
    if (e->key.repeat == 0 && e->key.keysym.scancode < MAX_KEYBOARD_KEYS) {
      GINPUT->keyboard[e->key.keysym.scancode] = true;
    }
  } else if (e->type == SDL_MOUSEMOTION) {
    GINPUT->x_rel = e->motion.xrel;
    GINPUT->y_rel = e->motion.yrel;
    GINPUT->x = e->motion.x;
    GINPUT->y = e->motion.y;
  } else if (e->type == SDL_MOUSEWHEEL) {
    GINPUT->scroll = e->wheel.preciseY;
  }
}

void input_tear_down(void) {
  SM_ASSERT(GINPUT != NULL && "trying to finilize a not initialized input");
  SM_FREE(GINPUT);
  GINPUT = NULL;
}
