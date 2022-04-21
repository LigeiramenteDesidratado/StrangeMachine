#ifndef SM_CORE_UTIL_DIR_H
#define SM_CORE_UTIL_DIR_H

#include "smpch.h"

bool sm__dir_exists(const char *dir);

#define SM_DIR_EXISTS(DIR) sm__dir_exists(DIR)

#endif /* SM_CORE_UTIL_DIR_H */
