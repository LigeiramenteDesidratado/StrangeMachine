#include <SDL2/SDL.h>

uint32_t sm__get_ticks(void) {

  return SDL_GetTicks();
}

void sm__delay(uint32_t ms) {

  SDL_Delay(ms);
}
