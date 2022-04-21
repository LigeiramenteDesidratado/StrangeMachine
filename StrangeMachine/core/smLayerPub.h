#ifndef SM_CORE_LAYER_PUB_H
#define SM_CORE_LAYER_PUB_H

#include "smpch.h"

#include "event/smEvent.h"

typedef void (*layer_on_attach_f)(void *user_data);
typedef void (*layer_on_detach_f)(void *user_data);
typedef void (*layer_on_update_f)(void *user_data, float dt);
typedef void (*layer_on_gui_f)(void *user_data);
typedef bool (*layer_on_event_f)(event_s *event, void *user_data);

#endif /* SM_CORE_LAYER_PUB_H */
