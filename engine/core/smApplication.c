#include "util/common.h"

#include "core/smStackLayer.h"
#include "core/smWindow.h"

#include "smMem.h"

#include <SDL2/SDL.h>

typedef struct {

  struct window_s *window;
  struct stack_layer_s *stack;

  bool is_running;

  uint32_t last_tick;
  float delta;

} application_s;

application_s *application_new(void) {

  static bool instanced = false;
  SM_CORE_ASSERT(!instanced && "application already instanced");

  application_s *app = SM_CALLOC(1, sizeof(application_s));
  SM_CORE_ASSERT(app);

  instanced = true;

  return app;
}

void application_on_event(event_s *event, void *user_data);

bool application_ctor(application_s *app, const char *name) {
  SM_CORE_ASSERT(app);

  struct window_s *window = window_new();
  if (!window_ctor(window, name, 800, 600)) {
    return false;
  }
  app->window = window;

  app->last_tick = SDL_GetTicks();
  app->delta = 0.0f;

  window_set_callback(window, application_on_event, app);

  struct stack_layer_s *stack = stack_layer_new();
  if (!stack_layer_ctor(stack)) {
    return false;
  }
  app->stack = stack;

  return true;
}

void application_on_event(event_s *event, void *user_data) {

  SM_CORE_ASSERT(event);
  SM_CORE_ASSERT(user_data);
  application_s *app = (application_s *)user_data;

  size_t stack_size = stack_layer_get_size(app->stack);
  for (size_t i = stack_size; i > 0; i--) {
    layer_s *layer = stack_layer_get_layer(app->stack, i - 1);
    if (event->handled)
      break;
    if (layer->on_event) {
      layer->on_event(event, layer->user_data);
    }
  }

  /* window_on_event(app->window, event); */
}

void application_do(application_s *app) {

  SM_CORE_ASSERT(app);

  while (app->is_running) {

    uint32_t current_tick = SDL_GetTicks();
    app->delta = (current_tick - app->last_tick) / 1000.0f;
    app->last_tick = current_tick;
  }
}

void application_dtor(application_s *app) {

  SM_CORE_ASSERT(app);

  window_dtor(app->window);
}