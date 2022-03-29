#ifndef SM_INPUT_H
#define SM_INPUT_H

#include "smpch.h"

#include "core/smKeyCode.h"
#include "event/smEvent.h"

void input_init(void);
bool input_on_event(event_s *event, void *user_data);
void input_do();
void input_tear_down(void);
bool input_scan_key(int32_t key);
bool input_scan_key_lock(int32_t key);
float input_get_mouse_scroll();
float input_get_x_rel_mouse();
float input_get_y_rel_mouse();
float input_get_x_mouse();
float input_get_y_mouse();
float input_get_x_last_mouse();
float input_get_y_last_mouse();

#endif // SM_INPUT_H
