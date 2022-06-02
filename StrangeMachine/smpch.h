#ifndef SM_PCH_H
#define SM_PCH_H

#include <assert.h>
#include <ctype.h> /* tolower */
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <math.h> /* mod sqrt sin cos... */
#include <stdatomic.h>
#include <stdbool.h> /* bool true false */
#include <stddef.h>  /* NULL */
#include <stdint.h>
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* malloc calloc realloc free */
#include <string.h>   /* memcpy and string manipulation */
#include <sys/stat.h> /* stat */

typedef uint8_t u8;

/** @brief Unsigned 16-bit integer */
typedef uint16_t u16;

/** @brief Unsigned 32-bit integer */
typedef uint32_t u32;

/** @brief Unsigned 64-bit integer */
typedef uint64_t u64;

/* Signed int types. */

/** @brief Signed 8-bit integer */
typedef char i8;

/** @brief Signed 16-bit integer */
typedef int16_t i16;

/** @brief Signed 32-bit integer */
typedef int32_t i32;

/** @brief Signed 64-bit integer */
typedef int64_t i64;

/* Floating point types */

/** @brief 32-bit floating point number */
typedef float f32;

/** @brief 64-bit floating point number */
typedef double f64;

/* Boolean type */
/** @brief 8-bit boolean type */
typedef _Bool b8;

#if defined(__clang__) || defined(__gcc__)
  /** @brief Static assertion */
  #define STATIC_ASSERT _Static_assert
#else

  /** @brief Static assertion */
  #define STATIC_ASSERT static_assert
#endif

/* Ensure all types are of the correct size. */

/** @brief Assert u8 to be 1 byte.*/
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");

/** @brief Assert u16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");

/** @brief Assert u32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");

/** @brief Assert u64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

/** @brief Assert i8 to be 1 byte.*/
STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");

/** @brief Assert i16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");

/** @brief Assert i32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");

/** @brief Assert i64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

/** @brief Assert f32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");

/** @brief Assert f64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

#endif /* SM_PCH_H */
