#ifndef SM_CORE_WINDOW_H
#define SM_CORE_WINDOW_H

#include "smpch.h"

#include "smWindowPub.h"

struct window_s;

/* acllocate memory for window */
struct window_s *window_new(void);

/* constructor */
b8 window_ctor(struct window_s *win, const char *name, u32 width, u32 height);

/* destructor */
void window_dtor(struct window_s *win);

f32 window_get_aspect_ratio(struct window_s *win);
u32 window_get_width(struct window_s *win);
u32 window_get_height(struct window_s *win);
void window_set_vsync(struct window_s *win, b8 vsync);
b8 window_is_vsync(struct window_s *win);
void window_set_callback(struct window_s *win, event_callback_f callback, void *user_data);
void window_do(struct window_s *win);
void window_swap_buffers(struct window_s *win);
void *window_get_window_raw(struct window_s *win);
void *window_get_context_raw(struct window_s *win);

#endif /* SM_CORE_WINDOW_H */
