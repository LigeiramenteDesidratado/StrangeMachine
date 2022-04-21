#ifndef SM_CORE_POOL_H
#define SM_CORE_POOL_H

#include "smpch.h"

#include "core/smHandle.h"

struct handle_pool_s;

/* Allocate Memory */
struct handle_pool_s *pool_new(void);

/* Constructor */
bool pool_ctor(struct handle_pool_s *pool, uint32_t capacity);

/* Destructor */
void pool_dtor(struct handle_pool_s *pool);

sm_handle handle_new(struct handle_pool_s *pool);
bool handle_valid(const struct handle_pool_s *pool, sm_handle handle);
bool handle_full(const struct handle_pool_s *pool);
void handle_del(struct handle_pool_s *pool, sm_handle handle);

#endif /* SM_CORE_POOL_H */
