#ifndef SM_CORE_THREAD_H
#define SM_CORE_THREAD_H

#include "core/thread/smThreadDecl.h"

sm_thread *sm__thread_create(sm__thread_function function, void *data);
int32_t sm__thread_wait(sm_thread *thread);
void sm__thread_detach(sm_thread *thread);

#define SM_THREAD_CTOR(FUNC, DATA) sm__thread_create(FUNC, DATA)
#define SM_THREAD_DETACH(THREAD)   sm__thread_detach(THREAD)
#define SM_THREAD_WAIT(THREAD)     sm__thread_wait(THREAD)

#endif /* SM_CORE_THREAD_H */
