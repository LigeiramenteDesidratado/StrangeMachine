#include "smpch.h"

#include "event/smEvent.h"
#include "smWindowPub.h"

#include "vendor/gladGL21/glad.h"
#include <SDL2/SDL.h>

typedef struct {
  SDL_Window *window;
  SDL_GLContext gc;
  uint32_t width, height;
  bool vsync;

  event_callback_f event_callback;
  void *user_data; // temporary

} window_s;

window_s *window_new(void) {

  window_s *win = SM_CALLOC(1, sizeof(window_s));
  SM_CORE_ASSERT(win);

  return win;
}

bool window_ctor(window_s *win, const char *name, uint32_t width, uint32_t height) {

  SM_CORE_ASSERT(win);
  SM_CORE_ASSERT(name);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SM_CORE_LOG_ERROR("SDL_Init Error: %s", SDL_GetError());
    return false;
  }

  win->window =
      SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_MOUSE_GRABBED);

  if (win->window == NULL) {
    SM_CORE_LOG_ERROR("SDL_CreateWindow Error: %s", SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  win->gc = SDL_GL_CreateContext(win->window);
  if (win->gc == NULL) {
    SM_CORE_LOG_ERROR("SDL_GL_CreateContext Error: %s", SDL_GetError());
    return false;
  }

  /* TODO: LOAD OPENGL FUNCTIONS PROPERLY */
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    SM_CORE_LOG_ERROR("failed to initialize OpengGL ctx: %s\n", SDL_GetError());
    return false;
  }

  GLint n_attrs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attrs);
  SM_CORE_LOG_DEBUG("GL Renderer: %s", glGetString(GL_RENDERER));
  SM_CORE_LOG_DEBUG("GL Version: %s", glGetString(GL_VERSION));
  SM_CORE_LOG_DEBUG("GL MAX_VERTEX_ATTRIBS: %d", n_attrs);

  // vsync enable by default
  SDL_GL_SetSwapInterval(1);
  win->vsync = true;

  win->width = width;
  win->height = height;

  return true;
}

void window_dtor(window_s *win) {

  SM_CORE_ASSERT(win);

  SDL_GL_DeleteContext(win->gc);
  SDL_DestroyWindow(win->window);
  SDL_Quit();

  SM_FREE(win);
}

void window_do(window_s *win) {

  SM_CORE_ASSERT(win);
  SM_CORE_ASSERT(win->event_callback);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      event_s e = event_new_window(SM_EVENT_WINDOW_CLOSE, 0, 0);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        win->width = event.window.data1;
        win->height = event.window.data2;
        event_s e = event_new_window(SM_EVENT_WINDOW_RESIZE, event.window.data1, event.window.data2);
        win->event_callback(&e, win->user_data);
        break;
      case SDL_WINDOWEVENT_FOCUS_GAINED: {
        event_s e = event_new_window(SM_EVENT_WINDOW_FOCUS, 0, 0);
        win->event_callback(&e, win->user_data);
        break;
      }
      case SDL_WINDOWEVENT_FOCUS_LOST: {
        event_s e = event_new_window(SM_EVENT_WINDOW_UNFOCUS, 0, 0);
        win->event_callback(&e, win->user_data);
        break;
      }
      }
      break;
    case SDL_KEYDOWN: {
      event_s e = event_new_key(SM_EVENT_KEY_DOWN, event.key.keysym.scancode);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_KEYUP: {
      event_s e = event_new_key(SM_EVENT_KEY_UP, event.key.keysym.scancode);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_MOUSEMOTION: {
      event_s e = event_new_mouse(SM_EVENT_MOUSE_MOVE, 0, event.motion.x, event.motion.y, event.motion.xrel,
                                  event.motion.yrel, 0.0f);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      event_s e = event_new_mouse(SM_EVENT_MOUSE_DOWN, event.button.button, event.button.x, event.button.y, 0, 0, 0.0f);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      event_s e = event_new_mouse(SM_EVENT_MOUSE_UP, event.button.button, event.button.x, event.button.y, 0, 0, 0.0f);
      win->event_callback(&e, win->user_data);
      break;
    }
    case SDL_MOUSEWHEEL: {
      event_s e = event_new_mouse(SM_EVENT_MOUSE_WHEEL, 0, 0, 0, 0, 0, event.wheel.preciseY);
      win->event_callback(&e, win->user_data);
      break;
    }
    }
  }

  if (win->vsync) {
    SDL_GL_SwapWindow(win->window);
  }
}

void window_set_callback(window_s *win, event_callback_f callback, void *user_data) {

  SM_CORE_ASSERT(win);
  SM_CORE_ASSERT(callback);

  win->event_callback = callback;
  win->user_data = user_data;
}

float window_get_aspect_ratio(window_s *win) {

  SM_CORE_ASSERT(win);

  return (float)win->width / (float)win->height;
}

uint32_t window_get_width(window_s *win) {

  SM_CORE_ASSERT(win);

  return win->width;
}

uint32_t window_get_height(window_s *win) {

  SM_CORE_ASSERT(win);

  return win->height;
}

bool window_is_vsync(window_s *win) {

  SM_CORE_ASSERT(win);

  return win->vsync;
}

void window_set_vsync(window_s *win, bool vsync) {

  SM_CORE_ASSERT(win);

  win->vsync = vsync;
  SDL_GL_SetSwapInterval(vsync);
}
