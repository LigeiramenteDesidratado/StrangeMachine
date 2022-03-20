#ifndef SM_CORE_BASE_H
#define SM_CORE_BASE_H

#include "smpch.h"

#include "core/smAssert.h"
#include "core/smLog.h"

#ifdef SM_DEBUG

  #define SM_UNUSED(x) (void)(x)

  #define SM_UNREACHABLE()                                                                                             \
    do {                                                                                                               \
      SM_LOG_FATAL("Unreachable code reached");                                                                        \
      abort();                                                                                                         \
    } while (0)

  #define SM_UNIMPLEMENTED(EXIT_IF_TRUE)                                                                               \
    do {                                                                                                               \
      if (EXIT_IF_TRUE) {                                                                                              \
        SM_LOG_FATAL("Unimplemented code reached");                                                                    \
        abort();                                                                                                       \
      } else                                                                                                           \
        SM_LOG_WARN("Unimplemented code reached");                                                                     \
                                                                                                                       \
    } while (0)

#else

  #define SM_UNUSED(x) (void)(x)
  #define SM_UNREACHABLE()
  #define SM_UNIMPLEMENTED(EXIT_IF_TRUE)

#endif

#endif /* SM_CORE_BASE_H */
