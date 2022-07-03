#ifndef SM_CORE_POOL_H
#define SM_CORE_POOL_H

#include "smpch.h"

#include "core/smHandle.h"

struct handle_pool_s;

/* Allocate Memory */
struct handle_pool_s *sm_handle_pool_new(void);

/* Constructor */
b8 sm_handle_pool_ctor(struct handle_pool_s *pool, u32 capacity);

/* Destructor */
void sm_handle_pool_dtor(struct handle_pool_s *pool);

sm_handle_t sm_handle_new(struct handle_pool_s *pool);
b8 sm_handle_valid(const struct handle_pool_s *pool, sm_handle_t handle);
b8 sm_handle_full(const struct handle_pool_s *pool);
void sm_handle_del(struct handle_pool_s *pool, sm_handle_t handle);

#endif /* SM_CORE_POOL_H */
