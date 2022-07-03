#ifndef SM_INPUT_H
#define SM_INPUT_H

#include "smpch.h"

#include "core/smKeyCode.h"
#include "event/smEvent.h"

void input_init(void);
bool input_on_event(event_s *event, void *user_data);
void input_do();
void input_tear_down(void);
b8 input_scan_key(sm_key_code key);
b8 input_scan_key_lock(sm_key_code key);
f32 input_get_mouse_scroll();
f32 input_get_x_rel_mouse();
f32 input_get_y_rel_mouse();
f32 input_get_x_mouse();
f32 input_get_y_mouse();
f32 input_get_x_last_mouse();
f32 input_get_y_last_mouse();

#endif // SM_INPUT_H
