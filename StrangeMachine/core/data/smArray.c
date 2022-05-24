#include "smpch.h"

#include "core/data/smArray.h"
#include "core/smAssert.h"
#include "core/smMem.h"

#define SM__ARRAY_LEN(RAW) RAW[0]
#define SM__ARRAY_CAP(RAW) RAW[1]

size_t *sm__array_ctor(size_t cap, size_t size) {

  size_t *array = SM_MALLOC(SM__ARRAY_HEADER_OFFSET + (size * cap));
  SM_CORE_ASSERT(array);

  SM__ARRAY_LEN(array) = 0;
  SM__ARRAY_CAP(array) = cap;

  return array;
}

size_t *sm__array_dtor(size_t *array) {

  SM_FREE(array);

  return NULL;
}

size_t *sm__array_set_len(size_t *array, size_t len, size_t size) {

  if (len > SM__ARRAY_CAP(array)) {

    void *tmp = NULL;
    tmp = SM_REALLOC(array, SM__ARRAY_HEADER_OFFSET + (len * size));
    SM_CORE_ASSERT(tmp);

    array = (size_t *)tmp;
    SM__ARRAY_CAP(array) = len;
  }

  SM__ARRAY_LEN(array) = len;

  return array;
}

size_t *sm__array_set_cap(size_t *array, size_t cap, size_t size) {

  if (cap > SM__ARRAY_CAP(array)) {

    void *tmp = NULL;
    tmp = SM_REALLOC(array, SM__ARRAY_HEADER_OFFSET + (cap * size));
    SM_CORE_ASSERT(tmp);

    array = (size_t *)tmp;
  }

  SM__ARRAY_CAP(array) = cap;
  SM__ARRAY_LEN(array) = (SM__ARRAY_LEN(array) > SM__ARRAY_CAP(array)) ? SM__ARRAY_CAP(array) : SM__ARRAY_LEN(array);

  return array;
}

size_t *sm__array_push(size_t *array, void *value, size_t size) {

  if (++SM__ARRAY_LEN(array) > SM__ARRAY_CAP(array))
    array = sm__array_set_cap(array, SM__ARRAY_CAP(array) * 2, size);

  memcpy((uint8_t *)array + SM__ARRAY_HEADER_OFFSET + (size * (SM__ARRAY_LEN(array) - 1)), value, size);

  return array;
}

size_t *sm__array_pop(size_t *array) {

  SM__ARRAY_LEN(array) = (SM__ARRAY_LEN(array) == 0) ? 0 : (SM__ARRAY_LEN(array) - 1);

  return array;
}
