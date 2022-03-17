#ifndef SM_CORE_ASSERT_H
#define SM_CORE_ASSERT_H

#include "core/smLog.h"

#ifdef SM_DEBUG

  #define SM_ASSERT(...)                                                                                               \
    do {                                                                                                               \
      if (!(__VA_ARGS__)) {                                                                                            \
        SM_LOG_FATAL("Assertion failed: %s", #__VA_ARGS__);                                                            \
        abort();                                                                                                       \
      }                                                                                                                \
    } while (0)

  #define SM_CORE_ASSERT(...)                                                                                          \
    do {                                                                                                               \
      if (!(__VA_ARGS__)) {                                                                                            \
        SM_CORE_LOG_FATAL("Assertion failed: %s", #__VA_ARGS__);                                                       \
        abort();                                                                                                       \
      }                                                                                                                \
    } while (0)

#else

  #define SM_ASSERT(...)

#endif

#endif /* SM_CORE_ASSERT_H */
