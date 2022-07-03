#ifndef SM_LAYER_H
#define SM_LAYER_H

#include "smpch.h"

#include "core/smLayerPub.h"

struct layer_s;

struct layer_s *layer_new(void);

b8 layer_ctor(struct layer_s *layer, char *name, void *user_data, layer_on_attach_f on_attach,
              layer_on_detach_f on_detach, layer_on_update_f on_update, layer_on_gui_f on_gui,
              layer_on_event_f on_event);

void layer_dtor(struct layer_s *layer);

void layer_attach(struct layer_s *layer);
void layer_detach(struct layer_s *layer);
void layer_update(struct layer_s *layer, f32 dt);
void layer_gui(struct layer_s *layer);
b8 layer_event(struct layer_s *layer, event_s *event);
void *layer_get_user_data(struct layer_s *layer);

#endif /* SM_LAYER_H */
