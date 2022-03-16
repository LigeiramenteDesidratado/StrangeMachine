#ifndef SM_CORE_WINDOW_H
#define SM_CORE_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include "smWindowPub.h"

struct window_s;

/* acllocate memory for window */
struct window_s *window_new(void);

/* constructor */
bool window_ctor(struct window_s *win, const char *name, uint32_t width, uint32_t height);

/* destructor */
void window_dtor(struct window_s *win);

float window_get_aspect_ratio(struct window_s *win);
uint32_t window_get_width(struct window_s *win);
uint32_t window_get_height(struct window_s *win);
void window_set_vsync(struct window_s *win, bool vsync);
bool window_is_vsync(struct window_s *win);
void window_set_callback(struct window_s *win, event_callback_f callback, void *user_data);
void window_do(struct window_s *win);

#endif /* SM_CORE_WINDOW_H */
