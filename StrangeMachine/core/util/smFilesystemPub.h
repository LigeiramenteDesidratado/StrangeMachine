#ifndef SM_CORE_UTIL_FILESYSTEM_PUB_H
#define SM_CORE_UTIL_FILESYSTEM_PUB_H

#include "smpch.h"

typedef struct sm__file_handle_s {
  void *handle;
  bool is_valid;

} sm_file_handle_s;

#endif /* SM_CORE_UTIL_FILESYSTEM_PUB_H */
