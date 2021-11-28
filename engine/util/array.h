#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_NEW(T, buf, size)                                               \
  T *buf = NULL;                                                               \
  do {                                                                         \
    size_t *raw = malloc(1 * sizeof(size_t) + sizeof(T) * size);               \
    if (raw = NULL)                                                            \
      err(EXIT_FAILURE, NULL);                                                 \
                                                                               \
    raw[0] = size;                                                             \
    buf = (void *)&raw[1];                                                     \
  } while (0)

#define BUFFER_DTOR(buf)                                                       \
  do {                                                                         \
    size_t *raw = ((size_t *)(buf)-1);                                         \
    free(raw);                                                                 \
    buf = NULL;                                                                \
  } while (0)

#define BUFFER_SIZE(buf) (*((size_t *)buf - 1))

#define ARRAY_NEW(T, arr, size)                                                \
  T *arr = NULL;                                                               \
  do {                                                                         \
    size_t *raw = malloc(2 * sizeof(size_t) + sizeof(T) * (size));             \
    raw[0] = 0;                                                                \
    raw[1] = size;                                                             \
    arr = (void *)&raw[2];                                                     \
  } while (0)

#define ARRAY_NEW_EMPTY() (&((size_t *)calloc(1, 2 * sizeof(size_t)))[2])

#define ARRAY_RESIZE(arr, size)                                                \
  do {                                                                         \
    size_t *raw = ((size_t *)(arr)-2);                                         \
    raw[1] = size;                                                             \
    raw[0] = (raw[0] > raw[1]) ? raw[1] : raw[0];                              \
    {                                                                          \
      void *__tmp = NULL;                                                      \
      __tmp = realloc(raw, 2 * sizeof(size_t) + raw[1] * sizeof((*arr)));      \
      if (__tmp == NULL)                                                       \
        err(EXIT_FAILURE, NULL);                                               \
                                                                               \
      raw = __tmp;                                                             \
    }                                                                          \
    (arr) = (void *)&raw[2];                                                   \
  } while (0)

#define ARRAY_DTOR(arr)                                                        \
  do {                                                                         \
    if (arr != NULL) {                                                         \
      size_t *raw = ((size_t *)(arr)-2);                                       \
      free(raw);                                                               \
      arr = NULL;                                                              \
    }                                                                          \
  } while (0)

#define ARRAY_SIZE(ARR) (((ARR) == NULL ? 0 : *((size_t *)ARR - 2)))
#define ARRAY_CAPACITY(ARR) (*((size_t *)ARR - 1))

#define ARRAY_PUSH(arr, value)                                                 \
  do {                                                                         \
    size_t *raw = ((size_t *)(arr)-2);                                         \
    raw[0] = raw[0] + 1;                                                       \
    if (raw[1] == 0) {                                                         \
      raw[1] = 1;                                                              \
      {                                                                        \
        void *__tmp = NULL;                                                    \
        __tmp = realloc(raw, 2 * sizeof(size_t) + raw[1] * sizeof((value)));   \
        if (__tmp == NULL)                                                     \
          err(EXIT_FAILURE, NULL);                                             \
                                                                               \
        raw = __tmp;                                                           \
      }                                                                        \
      (arr) = (void *)&raw[2];                                                 \
    }                                                                          \
    if (raw[0] > raw[1]) {                                                     \
      raw[1] = (size_t)(1.3 * raw[1]);                                         \
      {                                                                        \
        void *__tmp = NULL;                                                    \
        __tmp = realloc(raw, 2 * sizeof(size_t) + raw[1] * sizeof((value)));   \
        if (__tmp == NULL)                                                     \
          err(EXIT_FAILURE, NULL);                                             \
                                                                               \
        raw = __tmp;                                                           \
      }                                                                        \
      (arr) = (void *)&raw[2];                                                 \
    }                                                                          \
    arr[raw[0] - 1] = (value);                                                 \
  } while (0)

#define ARRAY_DEL(arr, i, n)                                                   \
  do {                                                                         \
    size_t *raw = ((size_t *)(arr)-2);                                         \
    memmove(&(arr)[(i)], &(arr)[(i) + (n)],                                    \
            sizeof((*arr)) * (raw)[0] - (n) - (i));                            \
    raw[0] = raw[0] - (n);                                                     \
  } while (0)\
