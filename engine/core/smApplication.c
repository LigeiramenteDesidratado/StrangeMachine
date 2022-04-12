#include "smpch.h"

#include "core/smAssert.h"
#include "core/smMem.h"
#include "core/smStackLayer.h"
#include "core/smWindow.h"

#include "core/util/smBitMask.h"

#include "smCamera.h"

#include "cimgui/smCimgui.h"

#include "resource/smResource.h"

#include "smInput.h"

#include <SDL2/SDL.h>

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "APPLICATION"

#ifdef SM_DEBUG
static void sm_at_exit(void) {
  sm__mem_print();
}
#endif

typedef struct application_s {

  struct window_s *window;
  struct stack_layer_s *stack;

  cimgui_s *cimgui;

  bool is_running;

  double delta;
  double elapsed;

} application_s;

application_s *application_new(void) {

  static bool instanced = false;
  SM_CORE_ASSERT(!instanced && "application already instanced");

  application_s *app = SM_CALLOC(1, sizeof(application_s));
  SM_CORE_ASSERT(app);

  instanced = true;

  return app;
}

bool application_on_event(event_s *event, void *user_data);
bool application_on_window_close(event_s *event, void *user_data);
void application_push_overlay(application_s *app, struct layer_s *layer);

bool application_ctor(application_s *app, const char *name) {

  SM_CORE_ASSERT(app);

  struct window_s *window = window_new();
  if (!window_ctor(window, name, 800, 600)) {
    SM_CORE_LOG_ERROR("failed to initialize window");
    return false;
  }
  app->window = window;

  app->delta = 0.0;

  window_set_callback(window, application_on_event, app);

  app->cimgui = cimgui_new();
  if (!cimgui_ctor(app->cimgui, app->window)) {
    SM_CORE_LOG_ERROR("failed to initialize cimgui");
    return false;
  }

  input_init();
  resource_init("assets/");

  struct stack_layer_s *stack = stack_layer_new();
  if (!stack_layer_ctor(stack)) {
    SM_CORE_LOG_ERROR("failed to initialize stack layer");
    return false;
  }
  app->stack = stack;

  application_push_overlay(app, app->cimgui);

  event_category_e mask = SM_CATEGORY_WINDOW;
  event_set_print_mask(mask);

  app->delta = 0.0;
  app->is_running = true;

  return true;
}

void application_dtor(application_s *app) {

  SM_CORE_ASSERT(app);

  stack_layer_dtor(app->stack);

  cimgui_dtor(app->cimgui);

  resource_teardown();

  input_tear_down();

  device_teardown();

  window_dtor(app->window);

  SM_FREE(app);

#ifdef SM_DEBUG
  sm_at_exit();
#endif
}

bool application_on_event(event_s *event, void *user_data) {

  event_print(event);

  SM_CORE_ASSERT(event);
  SM_CORE_ASSERT(user_data);

  application_s *app = (application_s *)user_data;

  event_dispatch(event, SM_EVENT_WINDOW_CLOSE, application_on_window_close, app);

  /* TODO: move input handling to input layer */
  /* this is a hack to get the input working */

  /* event_dispatch_categories(event, SM_CATEGORY_KEYBOARD | SM_CATEGORY_MOUSE, app->cimgui.on_event, */
  /*                           app->cimgui.user_data); */

  if (SM_MASK_CHK(event->category, SM_CATEGORY_KEYBOARD | SM_CATEGORY_MOUSE)) {
    event->handled |= layer_event(app->cimgui, event);
  }

  if (!event->handled)
    event_dispatch_categories(event, SM_CATEGORY_KEYBOARD | SM_CATEGORY_MOUSE, input_on_event, NULL);

  size_t stack_size = stack_layer_get_size(app->stack);
  for (size_t i = stack_size; i > 0; i--) {
    struct layer_s *layer = stack_layer_get_layer(app->stack, i - 1);
    if (event->handled)
      break;
    layer_event(layer, event);
  }

  return event->handled;
}

void sm__application_cap_frame_rate(long *then, float *remainder) {
  long wait, frameTime;
  wait = (long int)(16 + *remainder);

  *remainder -= (int)*remainder;

  frameTime = SDL_GetTicks() - *then;

  wait -= frameTime;
  if (wait < 1) {
    wait = 1;
  }
  SDL_Delay(wait);

  *remainder += 0.667f;

  *then = SDL_GetTicks();
}

void application_do(application_s *app) {

  SM_CORE_ASSERT(app);

  long then = SDL_GetTicks();
  float remainder = 0;
  int f = 12;

  while (app->is_running) {

    Uint64 start = SDL_GetPerformanceCounter();

    input_do();
    window_do(app->window);
    camera_do(app->delta);

    size_t stack_size = stack_layer_get_size(app->stack);
    for (size_t i = 0; i < stack_size; ++i) {
      struct layer_s *layer = stack_layer_get_layer(app->stack, i);
      layer_update(layer, app->delta);
    }

    cimgui_begin(app->cimgui);

    stack_size = stack_layer_get_size(app->stack);
    for (size_t i = 0; i < stack_size; ++i) {
      struct layer_s *layer = stack_layer_get_layer(app->stack, i);
      layer_gui(layer);
    }

    cimgui_end(app->cimgui);

    window_swap_buffers(app->window);

    sm__application_cap_frame_rate(&then, &remainder);

    Uint64 end = SDL_GetPerformanceCounter();

    app->delta = (end - (double)start) / SDL_GetPerformanceFrequency();

    if (--f <= 0) {
      app->elapsed = (1.0 / app->delta);
      f = 12;
    }
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

void application_push_layer(application_s *app, struct layer_s *layer) {

  SM_CORE_ASSERT(app);
  SM_CORE_ASSERT(layer);

  stack_layer_push(app->stack, layer);
  layer_attach(layer);
}

void application_push_overlay(application_s *app, struct layer_s *layer) {

  SM_CORE_ASSERT(app);
  SM_CORE_ASSERT(layer);

  stack_layer_push_overlay(app->stack, layer);
  layer_attach(layer);
}

#undef SM_MODULE_NAME
