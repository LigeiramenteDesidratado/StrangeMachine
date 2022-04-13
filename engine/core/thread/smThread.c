#include <SDL2/SDL.h>

#include "core/thread/smThreadDecl.h"

#include "core/smAssert.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "THREAD"

sm_thread *sm__thread_create(sm__thread_function function, void *data) {

  sm_thread *thread = (sm_thread *)SDL_CreateThread(function, "", data);
  SM_CORE_ASSERT(thread);

  return thread;
}

int32_t sm__thread_wait(sm_thread *thread) {

  SM_CORE_ASSERT(thread);

  int32_t status;

  SDL_WaitThread((SDL_Thread *)thread, &status);

  return status;
}

void sm__thread_detach(sm_thread *thread) {

  SM_CORE_ASSERT(thread);

  SDL_DetachThread((SDL_Thread *)thread);
}

#undef SM_MODULE_NAME
