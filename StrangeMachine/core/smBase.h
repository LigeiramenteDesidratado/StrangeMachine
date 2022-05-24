#ifndef SM_CORE_BASE_H
#define SM_CORE_BASE_H

#include "smpch.h"

#include "core/smAssert.h"
#include "core/smLog.h"

#ifdef SM_DEBUG

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

  #define SM_UNREACHABLE()
  #define SM_UNIMPLEMENTED(EXIT_IF_TRUE)

#endif /* SM_DEBUG */

#define SM_UNUSED(x) (void)(x)

#if defined(__GNUC__) || defined(__clang__)
  #define SM_PRIVATE __attribute__((unused)) static
#else
  #define SM_PRIVATE static
#endif

/* Platform */
#define SM_PLATFORM_ANDROID    0
#define SM_PLATFORM_BSD        0
#define SM_PLATFORM_EMSCRIPTEN 0
#define SM_PLATFORM_HURD       0
#define SM_PLATFORM_IOS        0
#define SM_PLATFORM_LINUX      0
#define SM_PLATFORM_NX         0
#define SM_PLATFORM_OSX        0
#define SM_PLATFORM_PS4        0
#define SM_PLATFORM_RPI        0
#define SM_PLATFORM_WINDOWS    0
#define SM_PLATFORM_WINRT      0
#define SM_PLATFORM_XBOXONE    0

#if defined(__ANDROID__) || defined(ANDROID)
  #undef SM_PLATFORM_ANDROID
  #define SM_PLATFORM_ANDROID __ANDROID__

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__)
  #undef SM_PLATFORM_BSD
  #define SM_PLATFORM_BSD 1

#elif defined(__EMSCRIPTEN__)
  #undef SM_PLATFORM_EMSCRIPTEN
  #define SM_PLATFORM_EMSCRIPTEN 1

#elif defined(__GNU__) || defined(__gnu_hurd__)
  #undef SM_PLATFORM_HURD
  #define SM_PLATFORM_HURD 1

#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
  #undef SM_PLATFORM_IOS
  #define SM_PLATFORM_IOS 1

#elif defined(__linux__)
  #undef SM_PLATFORM_LINUX
  #define SM_PLATFORM_LINUX 1

#elif defined(__NX__)
  #undef SM_PLATFORM_NX
  #define SM_PLATFORM_NX 1

#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
  #undef SX_PLATFORM_OSX
  #define SX_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__

#elif defined(__ORBIS__)
  #undef SM_PLATFORM_PS4
  #define SM_PLATFORM_PS4 1

#elif defined(__VCCOREVER__) || defined(__RPI__)
  /* RaspberryPi compiler defines __linux__ */
  #undef SX_PLATFORM_RPI
  #define SX_PLATFORM_RPI 1

#elif defined(_WIN32) || defined(_WIN64)
  #undef SM_PLATFORM_WINDOWS
  #define SM_PLATFORM_WINDOWS 1

#elif defined(WIN32) || defined(WIN64)
  #undef SM_PLATFORM_WINRT
  #define SM_PLATFORM_WINRT 1

#endif /* Platform */

#define SM_PLATFORM_MOBILE (SM_PLATFORM_ANDROID || SM_PLATFORM_IOS)
#define SM_PLATFORM_DESKTOP                                                                                            \
  (SM_PLATFORM_WINDOWS || SM_PLATFORM_LINUX || SM_PLATFORM_OSX || SM_PLATFORM_BSD || SM_PLATFORM_HURD)

#endif /* SM_CORE_BASE_H */
