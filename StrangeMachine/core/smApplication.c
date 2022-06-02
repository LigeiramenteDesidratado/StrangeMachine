#include "smpch.h"

#include "core/smBase.h"
#include "core/smLog.h"

#include "core/smAssert.h"
#include "core/smMem.h"
#include "core/smStackLayer.h"
#include "core/smTime.h"
#include "core/smTimer.h"
#include "core/smWindow.h"
#include "core/util/smBitMask.h"

#include "renderer/smDeviceDefs.h"
#include "renderer/smDevicePub.h"
#include "renderer/smRenderer3D.h"

#include "scene/smComponents.h"

#include "smCamera.h"

#include "cimgui/smCimgui.h"

/* #include "resource/smResource.h" */

#include "smInput.h"

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
  bool is_minimized;

  struct timer_s *timer;

  float thickness;
  double delta;
  double fps;

} application_s;

/* private functions */

SM_PRIVATE
void sm__application_cap_frame_rate(long *then, float *remainder);

SM_PRIVATE
void sm__application_status_gui(application_s *app);

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

#ifdef SM_DEBUG
  atexit(sm_at_exit);
#endif

  device_init(OPENGL21);

  app->window = window_new();
  if (!window_ctor(app->window, name, 800, 600)) {
    SM_CORE_LOG_ERROR("failed to initialize window");
    return false;
  }

  window_set_callback(app->window, application_on_event, app);

  renderer3D_init();

  app->cimgui = cimgui_new();
  if (!cimgui_ctor(app->cimgui, app->window)) {
    SM_CORE_LOG_ERROR("failed to initialize cimgui");
    return false;
  }

  input_init();
  /* resource_init("assets/"); */

  struct stack_layer_s *stack = stack_layer_new();
  if (!stack_layer_ctor(stack)) {
    SM_CORE_LOG_ERROR("failed to initialize stack layer");
    return false;
  }
  app->stack = stack;

  application_push_overlay(app, app->cimgui);

  event_category_e mask = SM_CATEGORY_WINDOW;
  event_set_print_mask(mask);

  component_init();

  app->timer = timer_new();
  timer_ctor(app->timer);

  app->delta = 0.0;
  app->is_running = true;
  app->thickness = 1.0;

  return true;
}

void application_dtor(application_s *app) {

  SM_CORE_ASSERT(app);

  timer_dtor(app->timer);

  component_teardown();

  stack_layer_dtor(app->stack);

  cimgui_dtor(app->cimgui);

  /* resource_teardown(); */

  input_tear_down();

  renderer3D_teardown();

  window_dtor(app->window);

  device_teardown();

  SM_FREE(app);
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

void application_do(application_s *app) {

  SM_CORE_ASSERT(app);

  long then = SM_GET_TICKS();
  float remainder = 0;
  int f = 12;

  while (app->is_running) {

    timer_reset(app->timer);

    input_do();
    window_do(app->window);
    camera_do((float)app->delta);

    size_t stack_size = stack_layer_get_size(app->stack);
    for (size_t i = 0; i < stack_size; ++i) {
      struct layer_s *layer = stack_layer_get_layer(app->stack, i);
      layer_update(layer, (float)app->delta * app->thickness);
    }

    static bool open = true;
    if (input_scan_key_lock(sm_key_space))
      open = !open;

    cimgui_begin(app->cimgui);

    stack_size = stack_layer_get_size(app->stack);
    for (size_t i = 0; i < stack_size; ++i) {
      struct layer_s *layer = stack_layer_get_layer(app->stack, i);
      layer_gui(layer);
    }

    if (open)
      sm__application_status_gui(app);

    /* igSliderFloat("dt", &app->thickness, -1.0, 1.0, "%.3f", 0); */

    cimgui_end(app->cimgui);

    window_swap_buffers(app->window);

    sm__application_cap_frame_rate(&then, &remainder);

    app->delta = timer_get_elapsed(app->timer);

    if (--f <= 0) {
      app->fps = (1.0 / app->delta);
      f = 12;
    }

    if (errno) {
      SM_CORE_LOG_ERROR("errno: %d", errno);
      errno = 0;
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

SM_PRIVATE
void sm__application_cap_frame_rate(long *then, float *remainder) {
  long wait, frameTime;
  wait = (long int)(16 + *remainder);

  *remainder -= fabsf(*remainder);

  frameTime = SM_GET_TICKS() - *then;

  wait -= frameTime;
  if (wait < 1) {
    wait = 1;
  }
  SM_DELAY((uint32_t)wait);

  *remainder += 0.667f;

  *then = SM_GET_TICKS();
}

SM_PRIVATE
void sm__application_status_gui(application_s *app) {
  int corner = (app != NULL) ? 1 : 0;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav;
  if (corner != -1) {
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  }

  igSetNextWindowBgAlpha(0.35f); // Transparent background
  if (igBegin("Application status overlay", NULL, window_flags)) {
    igText("average %.3f ms/frame (%.1f FPS)", 1000.0 / app->fps, app->fps);
    igText("Delta: %f", app->delta * (double)app->thickness);
    igText("Vsync: %s", window_is_vsync(app->window) ? "true" : "false");
  }
  igEnd();
}

#undef SM_MODULE_NAME
