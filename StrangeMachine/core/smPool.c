#include "smpch.h"

#include "core/smAssert.h"
#include "core/smBase.h"
#include "core/smHandle.h"
#include "core/smMem.h"

#include "core/data/smArray.h"

#define sm_align_mask(VALUE, MASK) (((VALUE) + (MASK)) & ((~0) & (~(MASK))))

typedef struct {

  SM_ARRAY(sm_handle) dense;
  SM_ARRAY(uint32_t) sparse;

} handle_pool_s;

handle_pool_s *pool_new(void) {

  handle_pool_s *pool = SM_CALLOC(1, sizeof(handle_pool_s));
  SM_ASSERT(pool);

  return pool;
}

bool pool_ctor(handle_pool_s *pool, uint32_t capacity) {

  /* Align count to 16, for a better aligned internal memory */
  uint32_t size = sm_align_mask(capacity, 15);

  SM_ARRAY_NEW(pool->dense, size);
  SM_ARRAY_NEW(pool->sparse, size);

  for (size_t i = 0; i < SM_ARRAY_CAPACITY(pool->dense); i++) {
    pool->dense[i] = sm__handle_make(0, i);
  }

  return true;
}

void pool_dtor(handle_pool_s *pool) {

  SM_ARRAY_DTOR(pool->sparse);
  SM_ARRAY_DTOR(pool->dense);

  SM_FREE(pool);
}

sm_handle handle_new(handle_pool_s *pool) {

  if (SM_ARRAY_SIZE(pool->dense) < SM_ARRAY_CAPACITY(pool->dense)) {

    uint32_t index = (uint32_t)SM_ARRAY_SIZE(pool->dense);
    sm_handle handle = pool->dense[index];

    uint32_t gen = sm_handle_gen(handle);
    int _index = sm_handle_index(handle);
    sm_handle new_handle = sm__handle_make(++gen, _index);

    pool->dense[index] = new_handle;
    pool->sparse[_index] = index;
    SM_ARRAY_SET_SIZE(pool->dense, SM_ARRAY_SIZE(pool->dense) + 1);
    SM_ARRAY_SET_SIZE(pool->sparse, SM_ARRAY_SIZE(pool->sparse) + 1);

    return new_handle;
  }

  SM_UNREACHABLE();
  return SM_INVALID_HANDLE;
}

bool handle_valid(const handle_pool_s *pool, sm_handle handle) {

  SM_ASSERT(handle);

  uint32_t index = pool->sparse[sm_handle_index(handle)];
  return index < SM_ARRAY_SIZE(pool->dense) && pool->dense[index] == handle;
}

bool handle_full(const handle_pool_s *pool) {
  return SM_ARRAY_SIZE(pool->dense) == SM_ARRAY_CAPACITY(pool->dense);
}

void handle_del(handle_pool_s *pool, sm_handle handle) {
  SM_ASSERT(SM_ARRAY_SIZE(pool->dense) > 0);
  SM_CORE_ASSERT(handle_valid(pool, handle));

  uint32_t index = pool->sparse[sm_handle_index(handle)];
  sm_handle last_handle = pool->dense[SM_ARRAY_SIZE(pool->dense) - 1];

  SM_ARRAY_POP(pool->dense);
  SM_ARRAY_POP(pool->sparse);

  pool->dense[SM_ARRAY_SIZE(pool->dense)] = handle;
  pool->sparse[sm_handle_index(last_handle)] = index;
  pool->dense[index] = last_handle;
}
