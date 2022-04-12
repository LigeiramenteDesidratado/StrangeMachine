#include "smpch.h"

#include "event/smEvent.h"

#include "core/smAssert.h"
#include "core/smLayerPub.h"
#include "core/smMem.h"

typedef struct {

  char *name;

  void *user_data;

  layer_on_attach_f on_attach;
  layer_on_detach_f on_detach;
  layer_on_update_f on_update;
  layer_on_gui_f on_gui;

  layer_on_event_f on_event;

} layer_s;

layer_s *layer_new(void) {

  layer_s *layer = SM_CALLOC(1, sizeof(layer_s));
  SM_ASSERT(layer);

  return layer;
}

bool layer_ctor(layer_s *layer, char *name, void *user_data, layer_on_attach_f on_attach, layer_on_detach_f on_detach,
                layer_on_update_f on_update, layer_on_gui_f on_gui, layer_on_event_f on_event) {

  SM_ASSERT(layer);

  layer->name = strdup(name);
  layer->user_data = user_data;
  layer->on_attach = on_attach;
  layer->on_detach = on_detach;
  layer->on_update = on_update;
  layer->on_gui = on_gui;
  layer->on_event = on_event;

  return true;
}

void layer_dtor(layer_s *layer) {

  SM_ASSERT(layer);

  free(layer->name);
  SM_FREE(layer);
}

void layer_attach(layer_s *layer) {

  SM_ASSERT(layer);

  if (layer->on_attach) {
    layer->on_attach(layer->user_data);
  }
}

void layer_detach(layer_s *layer) {

  SM_ASSERT(layer);

  if (layer->on_detach) {
    layer->on_detach(layer->user_data);
  }
}

void layer_update(layer_s *layer, float dt) {

  SM_ASSERT(layer);

  if (layer->on_update) {
    layer->on_update(layer->user_data, dt);
  }
}

void layer_gui(layer_s *layer) {

  SM_ASSERT(layer);

  if (layer->on_gui) {
    layer->on_gui(layer->user_data);
  }
}

bool layer_event(layer_s *layer, event_s *event) {

  SM_ASSERT(layer);

  if (layer->on_event) {
    return layer->on_event(event, layer->user_data);
  }

  return false;
}

void *layer_get_user_data(layer_s *layer) {

  SM_ASSERT(layer);

  return layer->user_data;
}
