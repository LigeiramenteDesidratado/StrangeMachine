#include "smpch.h"

#include "core/smAssert.h"
#include "core/smMem.h"

#include <SDL2/SDL.h>

typedef struct {

  Uint64 start;

} timer_s;

timer_s *timer_new(void) {

  timer_s *t = SM_MALLOC(sizeof(timer_s));

  return t;
}

void timer_ctor(timer_s *t) {

  SM_ASSERT(t);

  t->start = SDL_GetPerformanceCounter();
}

void timer_dtor(timer_s *t) {

  SM_ASSERT(t);

  SM_FREE(t);
}

void timer_reset(timer_s *t) {

  SM_ASSERT(t);

  t->start = SDL_GetPerformanceCounter();
}

double timer_get_elapsed(timer_s *t) {
  SM_ASSERT(t);

  Uint64 end = SDL_GetPerformanceCounter();

  return (double)(end - t->start) / SDL_GetPerformanceFrequency();
}
