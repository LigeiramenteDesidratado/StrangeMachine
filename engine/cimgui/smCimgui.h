#ifndef SM_CIMGUI_H
#define SM_CIMGUI_H

#include "core/smLayer.h"
#include "core/smWindow.h"
#include "event/smEvent.h"

#include "cimgui/smCimguiImpl.h"

typedef struct layer_s cimgui_s;

cimgui_s *cimgui_new(void);
bool cimgui_ctor(cimgui_s *cimgui, struct window_s *window);
void cimgui_dtor(cimgui_s *cimgui);

void cimgui_begin(cimgui_s *cimgui);
void cimgui_end(cimgui_s *cimgui);

#endif /* SM_CIMGUI_H */
