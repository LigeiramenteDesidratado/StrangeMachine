#ifndef __UTIL_ARRAY_H__
#define __UTIL_ARRAY_H__

#include "util/common.h"
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "smMem.h"

#define SM_SIZE_HEADER_SIZE     (sizeof(size_t))
#define SM_CAPACITY_HEADER_SIZE (sizeof(size_t))
#define SM_HEADER_OFFSET        (SM_SIZE_HEADER_SIZE + SM_CAPACITY_HEADER_SIZE)

/* Buffer utilities */
#define SM_BUFFER_NEW(T, buf, size)                                                                                    \
  T *buf = NULL;                                                                                                       \
  do {                                                                                                                 \
    size_t *raw = SM_MALLOC(1 * sizeof(size_t) + sizeof(T) * size);                                                    \
    if (raw = NULL)                                                                                                    \
      err(EXIT_FAILURE, NULL); /* display the current errno information string and exit */                             \
                                                                                                                       \
    raw[0] = size;                                                                                                     \
    buf = (void *)&raw[1];                                                                                             \
  } while (0)

#define SM_BUFFER_DTOR(buf)                                                                                            \
  do {                                                                                                                 \
    size_t *raw = ((size_t *)(buf)-1);                                                                                 \
    SM_FREE(raw);                                                                                                      \
    buf = NULL;                                                                                                        \
  } while (0)

#define SM_BUFFER_SIZE(buf) (*((size_t *)buf - 1))

/* array utilities */
#define SM_ARRAY_NEW(arr, size)                                                                                        \
  do {                                                                                                                 \
    if (arr)                                                                                                           \
      continue;                                                                                                        \
    size_t *raw = SM_MALLOC(SM_HEADER_OFFSET + sizeof((*arr)) * (size));                                               \
    raw[0] = 0;                                                                                                        \
    raw[1] = size;                                                                                                     \
    arr = (void *)&raw[2];                                                                                             \
  } while (0)

#define SM_ARRAY_NEW_EMPTY() (&((size_t *)SM_CALLOC(1, SM_HEADER_OFFSET))[2])

#define SM_ARRAY_SET_SIZE(arr, size)                                                                                   \
  do {                                                                                                                 \
    SM_ASSERT(size >= 0 && "negative size");                                                                           \
    size_t *raw = (!arr) ? SM_CALLOC(1, SM_HEADER_OFFSET + sizeof((*arr)) * (size)) : ((size_t *)(arr)-2);             \
    if ((size) > raw[1]) {                                                                                             \
      void *__tmp = NULL;                                                                                              \
      __tmp = SM_REALLOC(raw, SM_HEADER_OFFSET + size * sizeof((*arr)));                                               \
      if (__tmp == NULL)                                                                                               \
        err(EXIT_FAILURE, NULL);                                                                                       \
                                                                                                                       \
      raw = __tmp;                                                                                                     \
      raw[1] = size;                                                                                                   \
    }                                                                                                                  \
    raw[0] = size;                                                                                                     \
    (arr) = (void *)&raw[2];                                                                                           \
  } while (0)

#define SM_ARRAY_SET_CAPACITY(arr, size)                                                                               \
  do {                                                                                                                 \
    SM_ASSERT(size > 0 && "negative size or non zero");                                                                \
    size_t *raw = (!arr) ? SM_CALLOC(1, SM_HEADER_OFFSET + sizeof((*arr)) * (size)) : ((size_t *)(arr)-2);             \
    {                                                                                                                  \
      void *__tmp = NULL;                                                                                              \
      __tmp = SM_REALLOC(raw, SM_HEADER_OFFSET + size * sizeof((*arr)));                                               \
      if (__tmp == NULL)                                                                                               \
        err(EXIT_FAILURE, NULL);                                                                                       \
                                                                                                                       \
      raw = __tmp;                                                                                                     \
      raw[1] = size;                                                                                                   \
    }                                                                                                                  \
    raw[0] = (raw[0] > raw[1]) ? raw[1] : raw[0];                                                                      \
    (arr) = (void *)&raw[2];                                                                                           \
  } while (0)

#define SM_ARRAY_DTOR(arr)                                                                                             \
  do {                                                                                                                 \
    if (arr) {                                                                                                         \
      size_t *raw = ((size_t *)(arr)-2);                                                                               \
      SM_FREE(raw);                                                                                                    \
      arr = NULL;                                                                                                      \
    }                                                                                                                  \
  } while (0)

#define SM_ARRAY_SIZE(arr)     ((arr) == NULL ? 0 : *((size_t *)arr - 2))
#define SM_ARRAY_CAPACITY(arr) ((arr) == NULL ? 0 : *((size_t *)arr - 1))

#define SM_ARRAY_PUSH(arr, value)                                                                                      \
  do {                                                                                                                 \
    size_t *raw = (!arr) ? SM_CALLOC(1, SM_HEADER_OFFSET + sizeof((*arr))) : ((size_t *)(arr)-2);                      \
    raw[0] = raw[0] + 1;                                                                                               \
    if (raw[1] == 0) { /* TODO : remove this check ? */                                                                \
      raw[1] = 1;                                                                                                      \
      {                                                                                                                \
        void *__tmp = NULL;                                                                                            \
        __tmp = SM_REALLOC(raw, SM_HEADER_OFFSET + raw[1] * sizeof((value)));                                          \
        if (__tmp == NULL)                                                                                             \
          err(EXIT_FAILURE, NULL);                                                                                     \
                                                                                                                       \
        raw = __tmp;                                                                                                   \
      }                                                                                                                \
      (arr) = (void *)&raw[2];                                                                                         \
    }                                                                                                                  \
    if (raw[0] > raw[1]) {                                                                                             \
      raw[1] = (size_t)(2 * raw[1]);                                                                                   \
      {                                                                                                                \
        void *__tmp = NULL;                                                                                            \
        __tmp = SM_REALLOC(raw, SM_HEADER_OFFSET + raw[1] * sizeof((value)));                                          \
        if (__tmp == NULL)                                                                                             \
          err(EXIT_FAILURE, NULL);                                                                                     \
                                                                                                                       \
        raw = __tmp;                                                                                                   \
      }                                                                                                                \
      (arr) = (void *)&raw[2];                                                                                         \
    }                                                                                                                  \
    memcpy(&(arr)[(raw)[0] - 1], &(value), sizeof((value)));                                                           \
  } while (0)

#define SM_ARRAY_POP(arr)                                                                                              \
  do {                                                                                                                 \
    if (arr) {                                                                                                         \
      size_t *raw = ((size_t *)(arr)-2);                                                                               \
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

#define SM_ALIGNED_ARRAY_SIZE(arr) ((arr) == NULL ? 0 : *((size_t *)arr - 2))

#define SM_ALIGNED_ARRAY_NEW(arr, alignment, size)                                                                     \
  do {                                                                                                                 \
    SM_ASSERT(size > 0 && "negative size or non zero");                                                                \
    SM_ASSERT(arr == NULL && "already allocated");                                                                     \
    size_t *raw = SM_ALIGNED_ALLOC(alignment, SM_HEADER_OFFSET + size * sizeof((*arr)));                               \
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

#endif /* __UTIL_ARRAY_H__ */
