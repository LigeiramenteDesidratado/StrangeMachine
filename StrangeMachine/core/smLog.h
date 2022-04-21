#ifndef SM_CORE_LOG_H
#define SM_CORE_LOG_H

#ifdef SM_DEBUG
  #include "vendor/logc/src/log.h"

void log_log(int level, const char *file, int line, const char *fmt, ...) __attribute__((format(printf, 4, 5)));

  #ifndef SM_MODULE_NAME
    #define SM_MODULE_NAME "NO_NAME"
  #endif

  #define SM_FILE (__builtin_strrchr("/"__BASE_FILE__, '/') + 1)

  #define SM_LOG_TRACE(...) log_log(LOG_TRACE, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_LOG_DEBUG(...) log_log(LOG_DEBUG, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_LOG_INFO(...)  log_log(LOG_INFO, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_LOG_WARN(...)  log_log(LOG_WARN, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_LOG_ERROR(...) log_log(LOG_ERROR, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_LOG_FATAL(...) log_log(LOG_FATAL, SM_FILE, __LINE__, "[SM " SM_MODULE_NAME "] " __VA_ARGS__)

  #define SM_CORE_LOG_TRACE(...) log_log(LOG_TRACE, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_CORE_LOG_DEBUG(...) log_log(LOG_DEBUG, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_CORE_LOG_INFO(...)  log_log(LOG_INFO, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_CORE_LOG_WARN(...)  log_log(LOG_WARN, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_CORE_LOG_ERROR(...) log_log(LOG_ERROR, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)
  #define SM_CORE_LOG_FATAL(...) log_log(LOG_FATAL, SM_FILE, __LINE__, "[SMC " SM_MODULE_NAME "] " __VA_ARGS__)

#else

  #define SM_LOG_TRACE(...)
  #define SM_LOG_DEBUG(...)
  #define SM_LOG_INFO(...)
  #define SM_LOG_WARN(...)
  #define SM_LOG_ERROR(...)
  #define SM_LOG_FATAL(...)

  #define SM_CORE_LOG_TRACE(...)
  #define SM_CORE_LOG_DEBUG(...)
  #define SM_CORE_LOG_INFO(...)
  #define SM_CORE_LOG_WARN(...)
  #define SM_CORE_LOG_ERROR(...)
  #define SM_CORE_LOG_FATAL(...)

#endif

#endif /* SM_CORE_LOG_H */
