#include "smpch.h"

#include "event/smEvent.h"

#include "core/smCore.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "EVENT"

event_type_e event_get_type(event_s *event) {

  SM_ASSERT(event);

  switch (event->category) {
  case SM_CATEGORY_WINDOW:
    return event->window.type;
    break;
  case SM_CATEGORY_KEYBOARD:
    return event->key.type;
    break;
  case SM_CATEGORY_MOUSE:
    return event->mouse.type;
    break;
  default:
    return SM_EVENT_NONE;
    break;
  }
}

static event_category_e EVENT_PRINT_MASK = 0;

bool event_dispatch(event_s *event, event_type_e t, const event_handl func, void *user_data) {

  SM_ASSERT(event);

  if (event_get_type(event) == t) {
    event->handled |= func(event, user_data);
    return true;
  }
  return false;
}

bool event_dispatch_categories(event_s *event, event_category_e t, const event_handl func, void *user_data) {

  SM_ASSERT(event);

  if (SM_MASK_CHK(event->category, t)) {
    event->handled |= func(event, user_data);
    return true;
  }
  return false;
}

void event_set_print_mask(event_category_e mask) {
  EVENT_PRINT_MASK = mask;
}

void event_print(event_s *event) {

  SM_ASSERT(event);

  if (EVENT_PRINT_MASK == 0)
    return;

  switch (event->category) {
  case SM_CATEGORY_WINDOW:
    if (!SM_MASK_CHK(EVENT_PRINT_MASK, SM_CATEGORY_WINDOW))
      return;
    switch (event->window.type) {
    case SM_EVENT_WINDOW_CLOSE:
      SM_LOG_DEBUG("window close");
      break;
    case SM_EVENT_WINDOW_RESIZE:
      SM_LOG_DEBUG("window resize %d %d", event->window.width, event->window.height);
      break;
    case SM_EVENT_WINDOW_FOCUS:
      SM_LOG_DEBUG("window focus");
      break;
    case SM_EVENT_WINDOW_UNFOCUS:
      SM_LOG_DEBUG("window unfocus");
      break;
    default:
      SM_LOG_DEBUG("window event");
      break;
    }
    break;
  case SM_CATEGORY_KEYBOARD:
    if (!SM_MASK_CHK(EVENT_PRINT_MASK, SM_CATEGORY_KEYBOARD))
      break;
    switch (event->key.type) {
    case SM_EVENT_KEY_DOWN:
      SM_LOG_DEBUG("key down %s", sm_key_to_str(event->key.key));
      break;
    case SM_EVENT_KEY_UP:
      SM_LOG_DEBUG("key up %s", sm_key_to_str(event->key.key));
      break;
    default:
      SM_LOG_DEBUG("key event %s", sm_key_to_str(event->key.key));
      break;
    }
    break;
  case SM_CATEGORY_MOUSE:
    if (!SM_MASK_CHK(EVENT_PRINT_MASK, SM_CATEGORY_MOUSE))
      break;
    switch (event->mouse.type) {
    case SM_EVENT_MOUSE_MOVE:
      SM_LOG_DEBUG("mouse move x:%d y:%d", event->mouse.x, event->mouse.y);
      break;
    case SM_EVENT_MOUSE_DOWN:
      SM_LOG_DEBUG("mouse down %d", event->mouse.button);
      break;
    case SM_EVENT_MOUSE_UP:
      SM_LOG_DEBUG("mouse up %d", event->mouse.button);
      break;
    case SM_EVENT_MOUSE_WHEEL:
      SM_LOG_DEBUG("mouse wheel %f", event->mouse.wheel);
      break;
    default:
      SM_LOG_DEBUG("mouse event");
      break;
    }
    break;
  default:
    SM_LOG_DEBUG("unknown event");
    break;
  }
}

#undef SM_MODULE_NAME
