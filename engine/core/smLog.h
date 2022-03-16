#ifndef SM_CORE_LOG_H
#define SM_CORE_LOG_H

#ifdef SM_DEBUG
  #include "vendor/logc/src/log.h"

  #define SM_FILE (__builtin_strrchr("/"__BASE_FILE__, '/') + 1)

  #define SM_LOG_TRACE(...) log_log(LOG_TRACE, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)
  #define SM_LOG_DEBUG(...) log_log(LOG_DEBUG, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)
  #define SM_LOG_INFO(...)  log_log(LOG_INFO, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)
  #define SM_LOG_WARN(...)  log_log(LOG_WARN, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)
  #define SM_LOG_ERROR(...) log_log(LOG_ERROR, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)
  #define SM_LOG_FATAL(...) log_log(LOG_FATAL, SM_FILE, __LINE__, "[SM] " __VA_ARGS__)

  #define SM_CORE_LOG_TRACE(...) log_log(LOG_TRACE, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)
  #define SM_CORE_LOG_DEBUG(...) log_log(LOG_DEBUG, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)
  #define SM_CORE_LOG_INFO(...)  log_log(LOG_INFO, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)
  #define SM_CORE_LOG_WARN(...)  log_log(LOG_WARN, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)
  #define SM_CORE_LOG_ERROR(...) log_log(LOG_ERROR, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)
  #define SM_CORE_LOG_FATAL(...) log_log(LOG_FATAL, SM_FILE, __LINE__, "[SM CORE] " __VA_ARGS__)

#else

  #define SM_LOG_TRACE(...) (void *)0
  #define SM_LOG_DEBUG(...) (void *)0
  #define SM_LOG_INFO(...)  (void *)0
  #define SM_LOG_WARN(...)  (void *)0
  #define SM_LOG_ERROR(...) (void *)0
  #define SM_LOG_FATAL(...) (void *)0

  #define SM_CORE_LOG_TRACE(...) (void *)0
  #define SM_CORE_LOG_DEBUG(...) (void *)0
  #define SM_CORE_LOG_INFO(...)  (void *)0
  #define SM_CORE_LOG_WARN(...)  (void *)0
  #define SM_CORE_LOG_ERROR(...) (void *)0
  #define SM_CORE_LOG_FATAL(...) (void *)0

#endif

#endif /* SM_CORE_LOG_H */
