#include "smpch.h"

#include "core/smAssert.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_DIR"

bool sm__dir_exists(const char *dir) {

  SM_CORE_ASSERT(dir);

  struct stat sb;
  return stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode);
}

#undef SM_MODULE_NAME
