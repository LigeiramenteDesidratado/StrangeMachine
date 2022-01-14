#ifndef SM_MEM_H
#define SM_MEM_H

#include <stddef.h>

#define SM_MEM_HEADER_SIZE sizeof(size_t)

void *__smmem_malloc(size_t size);
void *__smmem_calloc(size_t nmemb, size_t size);
void *__smmem_realloc(void *ptr, size_t size);
void __smmem_free(void *ptr);
void __smmem_print(void);

/* Memory manage */
#define SM_MALLOC(SIZE)        __smmem_malloc(SIZE)
#define SM_CALLOC(NMEMB, SIZE) __smmem_calloc(NMEMB, SIZE)
#define SM_REALLOC(PTR, SIZE)  __smmem_realloc(PTR, SIZE)
#define SM_FREE(PTR)           __smmem_free(PTR)

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
    size_t *raw = SM_MALLOC(2 * SM_MEM_HEADER_SIZE + sizeof((*arr)) * (size));                                         \
    raw[0] = 0;                                                                                                        \
    raw[1] = size;                                                                                                     \
    arr = (void *)&raw[2];                                                                                             \
  } while (0)

#define SM_ARRAY_NEW_EMPTY() (&((size_t *)SM_CALLOC(1, 2 * SM_MEM_HEADER_SIZE))[2])

#define SM_ARRAY_SET_SIZE(arr, size)                                                                                   \
  do {                                                                                                                 \
    assert(size >= 0 && "negative size");                                                                              \
    size_t *raw = (!arr) ? SM_CALLOC(1, 2 * SM_MEM_HEADER_SIZE + sizeof((*arr)) * (size)) : ((size_t *)(arr)-2);       \
    if (size > raw[1]) {                                                                                               \
      void *__tmp = NULL;                                                                                              \
      __tmp = SM_REALLOC(raw, 2 * SM_MEM_HEADER_SIZE + size * sizeof((*arr)));                                         \
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
    assert(size > 0 && "negative size or non zero");                                                                   \
    size_t *raw = (!arr) ? SM_CALLOC(1, 2 * SM_MEM_HEADER_SIZE + sizeof((*arr)) * (size)) : ((size_t *)(arr)-2);       \
    {                                                                                                                  \
      void *__tmp = NULL;                                                                                              \
      __tmp = SM_REALLOC(raw, 2 * SM_MEM_HEADER_SIZE + size * sizeof((*arr)));                                         \
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
    size_t *raw = (!arr) ? SM_CALLOC(1, 2 * SM_MEM_HEADER_SIZE + sizeof((*arr))) : ((size_t *)(arr)-2);                \
    raw[0] = raw[0] + 1;                                                                                               \
    if (raw[1] == 0) { /* TODO : remove this check ? */                                                                \
      raw[1] = 1;                                                                                                      \
      {                                                                                                                \
        void *__tmp = NULL;                                                                                            \
        __tmp = SM_REALLOC(raw, 2 * SM_MEM_HEADER_SIZE + raw[1] * sizeof((value)));                                    \
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
        __tmp = SM_REALLOC(raw, 2 * SM_MEM_HEADER_SIZE + raw[1] * sizeof((value)));                                    \
        if (__tmp == NULL)                                                                                             \
          err(EXIT_FAILURE, NULL);                                                                                     \
                                                                                                                       \
        raw = __tmp;                                                                                                   \
      }                                                                                                                \
      (arr) = (void *)&raw[2];                                                                                         \
    }                                                                                                                  \
    arr[raw[0] - 1] = (value);                                                                                         \
  } while (0)

#define SM_ARRAY_DEL(arr, i, n)                                                                                        \
  do {                                                                                                                 \
    assert(i >= 0 && "negative index");                                                                                \
    size_t *raw = ((size_t *)(arr)-2);                                                                                 \
    assert(i < raw[0] && "index out of range");                                                                        \
    if (n == -1) {                                                                                                     \
      raw[0] = i;                                                                                                      \
      continue;                                                                                                        \
    }                                                                                                                  \
    memmove(&(arr)[(i)], &(arr)[(i) + (n)], sizeof((*arr)) * ((raw)[0] - ((i) + (n))));                                \
    raw[0] = ((i) + (n) >= raw[0]) ? (raw[0] - (raw[0] - (i))) : raw[0] - (n);                                         \
  } while (0)

/* TODO: HASH TABLE */

#endif // SM_MEM_H
