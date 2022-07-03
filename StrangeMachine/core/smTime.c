#include <SDL2/SDL.h>
#include "smpch.h"

u32 sm__get_ticks(void) {

  return SDL_GetTicks();
}

void sm__delay(u32 ms) {

  SDL_Delay(ms);
}
