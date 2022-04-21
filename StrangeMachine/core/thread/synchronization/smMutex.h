#ifndef SM_CORE_THREAD_SYNCHRONIZATION_H
#define SM_CORE_THREAD_SYNCHRONIZATION_H

#include "core/thread/synchronization/smMutexDecl.h"

sm_mutex *sm__mutex_ctor(void);
void sm__mutex_dtor(sm_mutex *mutex);

void sm__mutex_lock(sm_mutex *mutex);
void sm__mutex_unlock(sm_mutex *mutex);
void sm__mutex_trylock(sm_mutex *mutex);

#define SM_MUTEX_CTOR()         sm__mutex_ctor()
#define SM_MUTEX_DTOR(mutex)    sm__mutex_dtor(mutex)
#define SM_MUTEX_LOCK(mutex)    sm__mutex_lock(mutex)
#define SM_MUTEX_UNLOCK(mutex)  sm__mutex_unlock(mutex)
#define SM_MUTEX_TRYLOCK(mutex) sm__mutex_trylock(mutex)

#endif /* SM_CORE_THREAD_SYNCHRONIZATION_H */

