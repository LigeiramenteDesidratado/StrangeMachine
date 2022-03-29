#include "smpch.h"

#include "smAssert.h"
#include "smMem.h"
#include "smStackLayer.h"
#include "smWindow.h"

#include "smInput.h"

#include <SDL2/SDL.h>

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "APPLICATION"

#ifdef SM_DEBUG
static void sm_at_exit(void) {
  __smmem_print();
}
#endif

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
bool application_on_window_close(event_s *event, void *user_data);

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
  app->is_running = true;

  input_init();

  return true;
}

void application_on_event(event_s *event, void *user_data) {

  event_print(event);

  SM_CORE_ASSERT(event);
  SM_CORE_ASSERT(user_data);

  application_s *app = (application_s *)user_data;

  event_dispatch(event, SM_EVENT_WINDOW_CLOSE, application_on_window_close, app);
  event_dispatch_categories(event, SM_CATEGORY_KEYBOARD | SM_CATEGORY_MOUSE, input_on_event, NULL);

  size_t stack_size = stack_layer_get_size(app->stack);
  for (size_t i = stack_size; i > 0; i--) {
    layer_s *layer = stack_layer_get_layer(app->stack, i - 1);
    if (event->handled)
      break;
    if (layer->on_event) {
      layer->on_event(event, layer->user_data);
    }
  }
}

void application_do(application_s *app) {

  SM_CORE_ASSERT(app);

  while (app->is_running) {

    input_do();
    window_do(app->window);

    size_t stack_size = stack_layer_get_size(app->stack);
    for (size_t i = 0; i < stack_size; ++i) {
      layer_s *layer = stack_layer_get_layer(app->stack, i);
      if (layer->on_update) {
        layer->on_update(layer->user_data, app->delta);
      }
    }

    uint32_t current_tick = SDL_GetTicks();
    app->delta = (current_tick - app->last_tick) / 1000.0f;
    app->last_tick = current_tick;
  }

#ifdef SM_DEBUG

  SM_CORE_ASSERT(app->is_running == false);

#endif
}

bool application_on_window_close(event_s *event, void *user_data) {

  SM_CORE_ASSERT(event);
  SM_CORE_ASSERT(user_data);

  application_s *app = (application_s *)user_data;

  app->is_running = false;

  return true;
}

void application_dtor(application_s *app) {

  SM_CORE_ASSERT(app);

  input_tear_down();

  stack_layer_dtor(app->stack);
  window_dtor(app->window);

  SM_FREE(app);

#ifdef SM_DEBUG
  sm_at_exit();
#endif
}

void application_push_layer(application_s *app, layer_s *layer) {

  SM_CORE_ASSERT(app);
  SM_CORE_ASSERT(layer);

  stack_layer_push(app->stack, layer);
  layer->on_attach(layer->user_data);
}

void application_push_overlay(application_s *app, layer_s *layer) {

  SM_CORE_ASSERT(app);
  SM_CORE_ASSERT(layer);

  stack_layer_push_overlay(app->stack, layer);
  layer->on_attach(layer->user_data);
}
#undef SM_MODULE_NAME
