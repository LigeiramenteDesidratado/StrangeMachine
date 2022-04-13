#include <SDL2/SDL.h>

#include "core/thread/synchronization/smMutexDecl.h"

#include "core/smLog.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "MUTEX"

void sm__mutex_lock(sm_mutex *mutex) {

  if (SDL_LockMutex((SDL_mutex *)mutex) < 0) {
    SM_CORE_LOG_ERROR("Failed to lock mutex: %s", SDL_GetError());
  }
}

void sm__mutex_unlock(sm_mutex *mutex) {

  if (SDL_UnlockMutex((SDL_mutex *)mutex) < 0) {
    SM_CORE_LOG_ERROR("Failed to unlock mutex: %s", SDL_GetError());
  }
}

bool sm__mutex_trylock(sm_mutex *mutex) {

  int32_t status = SDL_TryLockMutex((SDL_mutex *)mutex);
  if (status < 0) {
    SM_CORE_LOG_ERROR("Failed to trylock mutex: %s", SDL_GetError());
    return false;
  }

  return status == 0;
}

sm_mutex *sm__mutex_ctor(void) {

  SDL_mutex *mutex = SDL_CreateMutex();
  if (mutex == NULL) {
    SM_CORE_LOG_ERROR("Failed to create mutex: %s", SDL_GetError());
    return NULL;
  }
  return (sm_mutex *)mutex;
}

void sm__mutex_dtor(sm_mutex *mutex) {

  SDL_DestroyMutex((SDL_mutex *)mutex);
}

#undef SM_MODULE_NAME
