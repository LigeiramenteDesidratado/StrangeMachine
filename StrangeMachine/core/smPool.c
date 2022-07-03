#include "smpch.h"

#include "core/smAssert.h"
#include "core/smBase.h"
#include "core/smHandle.h"
#include "core/smMem.h"

#include "core/data/smArray.h"

#define sm_align_mask(VALUE, MASK) (((VALUE) + (MASK)) & ((~0) & (~(MASK))))

typedef struct sm__handle_pool_s {

  SM_ARRAY(sm_handle_t) dense;
  SM_ARRAY(u32) sparse;

} sm_handle_pool_s;

sm_handle_pool_s *sm_handle_pool_new(void) {

  sm_handle_pool_s *pool = SM_CALLOC(1, sizeof(sm_handle_pool_s));
  SM_ASSERT(pool);

  return pool;
}

b8 sm_handle_pool_ctor(sm_handle_pool_s *pool, u32 capacity) {

  /* Align count to 16, for a better aligned internal memory */
  u32 size = sm_align_mask(capacity, 15);

  SM_ARRAY_CTOR(pool->dense, size);
  SM_ARRAY_CTOR(pool->sparse, size);

  for (size_t i = 0; i < SM_ARRAY_CAP(pool->dense); i++) {
    pool->dense[i] = sm__handle_make(0, i);
  }

  return true;
}

void sm_handle_pool_dtor(sm_handle_pool_s *pool) {

  SM_ARRAY_DTOR(pool->sparse);
  SM_ARRAY_DTOR(pool->dense);

  SM_FREE(pool);
}

sm_handle_t sm_handle_new(sm_handle_pool_s *pool) {

  if (SM_ARRAY_LEN(pool->dense) < SM_ARRAY_CAP(pool->dense)) {

    u32 index = (u32)SM_ARRAY_LEN(pool->dense);
    sm_handle_t handle = pool->dense[index];

    u32 gen = sm_handle_gen(handle);
    u32 _index = sm_handle_index(handle);
    sm_handle_t new_handle = sm__handle_make(++gen, _index);

    pool->dense[index] = new_handle;
    pool->sparse[_index] = index;
    SM_ARRAY_SET_LEN(pool->dense, SM_ARRAY_LEN(pool->dense) + 1);
    SM_ARRAY_SET_LEN(pool->sparse, SM_ARRAY_LEN(pool->sparse) + 1);

    return new_handle;
  }

  SM_UNREACHABLE();
  return SM_INVALID_HANDLE;
}

b8 sm_handle_valid(const sm_handle_pool_s *pool, sm_handle_t handle) {

  SM_ASSERT(handle);

  u32 index = pool->sparse[sm_handle_index(handle)];
  return index < SM_ARRAY_LEN(pool->dense) && pool->dense[index] == handle;
}

b8 sm_handle_full(const sm_handle_pool_s *pool) {
  return SM_ARRAY_LEN(pool->dense) == SM_ARRAY_CAP(pool->dense);
}

void sm_handle_del(sm_handle_pool_s *pool, sm_handle_t handle) {
  SM_ASSERT(SM_ARRAY_LEN(pool->dense) > 0);
  SM_CORE_ASSERT(sm_handle_valid(pool, handle));

  u32 index = pool->sparse[sm_handle_index(handle)];
  sm_handle_t last_handle = pool->dense[SM_ARRAY_LEN(pool->dense) - 1];

  SM_ARRAY_POP(pool->dense);
  SM_ARRAY_POP(pool->sparse);

  pool->dense[SM_ARRAY_LEN(pool->dense)] = handle;
  pool->sparse[sm_handle_index(last_handle)] = index;
  pool->dense[index] = last_handle;
}
