#ifndef SM_CORE_DATA_ARRAY_H
#define SM_CORE_DATA_ARRAY_H

#include "smpch.h"

#define SM_ARRAY(ARRAY) ARRAY *

#define SM_STATIC_TYPE_ASSERT(X, Y) _Generic((Y), __typeof__(X) : _Generic((X), __typeof__(Y) : (void)NULL))

#define SM__ARRAY_LEN_HEADER_SIZE (sizeof(size_t))
#define SM__ARRAY_CAP_HEADER_SIZE (sizeof(size_t))
#define SM__ARRAY_HEADER_OFFSET   (SM__ARRAY_LEN_HEADER_SIZE + SM__ARRAY_CAP_HEADER_SIZE)

#define SM__RAW(ARRAY) ((size_t *)((char *)ARRAY - SM__ARRAY_HEADER_OFFSET))

size_t *sm__array_ctor(size_t cap, size_t size);
size_t *sm__array_dtor(size_t *array);
size_t *sm__array_set_len(size_t *array, size_t len, size_t size);
size_t *sm__array_set_cap(size_t *array, size_t cap, size_t size);
size_t *sm__array_push(size_t *array, void *value, size_t size);
size_t *sm__array_pop(size_t *array);

/* array utilities */
#define SM_ARRAY_CTOR(ARR, CAPACITY)                                                                                   \
  do {                                                                                                                 \
    if (ARR)                                                                                                           \
      continue;                                                                                                        \
    size_t *raw = sm__array_ctor((CAPACITY), sizeof((*ARR)));                                                          \
    ARR = (void *)&raw[2];                                                                                             \
  } while (0)

#define SM_ARRAY_DTOR(ARR)                                                                                             \
  do {                                                                                                                 \
    if (ARR) {                                                                                                         \
      size_t *raw = ((size_t *)(ARR)-2);                                                                               \
      sm__array_dtor(raw);                                                                                             \
      ARR = NULL;                                                                                                      \
    }                                                                                                                  \
  } while (0)

#define SM_ARRAY_SET_LEN(ARR, LEN)                                                                                     \
  do {                                                                                                                 \
    size_t *raw = (!ARR) ? sm__array_ctor((LEN), sizeof((*ARR))) : SM__RAW((ARR));                                     \
    raw = sm__array_set_len(raw, (LEN), sizeof((*ARR)));                                                               \
    (ARR) = (void *)&raw[2];                                                                                           \
  } while (0)

#define SM_ARRAY_SET_CAP(ARR, CAP)                                                                                     \
  do {                                                                                                                 \
    size_t *raw = (!ARR) ? sm__array_ctor((CAP), sizeof((*ARR))) : SM__RAW((ARR));                                     \
    raw = sm__array_set_cap(raw, (CAP), sizeof((*ARR)));                                                               \
    (ARR) = (void *)&raw[2];                                                                                           \
  } while (0)

#define SM_ARRAY_LEN(arr) ((arr) == NULL ? 0 : *((size_t *)arr - 2))
#define SM_ARRAY_CAP(arr) ((arr) == NULL ? 0 : *((size_t *)arr - 1))

#define SM_ARRAY_PUSH(ARR, VALUE)                                                                                      \
  do {                                                                                                                 \
    size_t *raw = (!ARR) ? sm__array_ctor(1, sizeof((*ARR))) : SM__RAW((ARR));                                         \
    SM_STATIC_TYPE_ASSERT((*ARR), (VALUE));                                                                            \
    raw = sm__array_push(raw, &(VALUE), sizeof((VALUE)));                                                              \
    (ARR) = (void *)&raw[2];                                                                                           \
  } while (0)

/* WARNING: do not use SM_ARRAY_POP inside a loop that depends SM_ARRAY_LEN.
 * like:
 *  for (size_t i = 0; i < SM_ARRAY_LEN(array); ++i) {
 *    // ...
 *    SM_ARRAY_POP(array);
 *  }
 */
#define SM_ARRAY_POP(ARR)                                                                                              \
  do {                                                                                                                 \
    if (ARR) {                                                                                                         \
      size_t *raw = SM__RAW(ARR);                                                                                      \
      raw[0] = (raw[0] == 0) ? 0 : raw[0] - 1;                                                                         \
    }                                                                                                                  \
  } while (0)

#define SM_ARRAY_DEL(arr, i, n)                                                                                        \
  do {                                                                                                                 \
    SM_ASSERT(i >= 0 && "negative index");                                                                             \
    size_t *raw = (((size_t *)(arr)) - 2);                                                                             \
    SM_ASSERT(i < raw[0] && "index out of range");                                                                     \
    if (n == -1) {                                                                                                     \
      raw[0] = i;                                                                                                      \
      continue;                                                                                                        \
    }                                                                                                                  \
    memmove(&(arr)[(i)], &(arr)[(i) + (n)], sizeof((*arr)) * ((raw)[0] - ((i) + (n))));                                \
    raw[0] = ((i) + (n) >= raw[0]) ? (raw[0] - (raw[0] - (i))) : raw[0] - (n);                                         \
  } while (0)

/* ALIGNED ARRAY */

/* TODO: implement aligned array properly */
#define SM_ALIGNED_ARRAY_SIZE(arr) ((arr) == NULL ? 0 : *((size_t *)arr - 2))

#define SM_ALIGNED_ARRAY_NEW(arr, alignment, size)                                                                     \
  do {                                                                                                                 \
    SM_ASSERT(size > 0 && "negative size or non zero");                                                                \
    SM_ASSERT(arr == NULL && "already allocated");                                                                     \
    size_t *raw = SM_ALIGNED_ALLOC(alignment, SM__ARRAY_HEADER_OFFSET + size * sizeof((*arr)));                        \
    if (raw == NULL)                                                                                                   \
      err(EXIT_FAILURE, NULL);                                                                                         \
    raw[0] = size;                                                                                                     \
    raw[1] = size;                                                                                                     \
    (arr) = (void *)&raw[2];                                                                                           \
  } while (0)

#define SM_ALIGNED_ARRAY_DTOR(arr)                                                                                     \
  do {                                                                                                                 \
    if ((arr)) {                                                                                                       \
      size_t *raw = ((size_t *)(arr)-2);                                                                               \
      SM_ALIGNED_FREE((raw));                                                                                          \
      (arr) = NULL;                                                                                                    \
    }                                                                                                                  \
  } while (0)

#endif /* SM_CORE_DATA_ARRAY_H */
