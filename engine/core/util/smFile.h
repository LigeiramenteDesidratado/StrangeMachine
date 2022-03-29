#ifndef SM_CORE_UTIL_FILE_H
#define SM_CORE_UTIL_FILE_H

#include "smpch.h"

bool sm__file_has_ext(const char *file, const char *suffix);
bool sm__file_exists(const char *file);
const char *sm__file_get_ext(const char *file);
const char *sm__file_read(const char *file);

#define SM_FILE_HAS_EXT(FILE, SUFFIX) sm__file_has_ext(FILE, SUFFIX)
#define SM_FILE_EXISTS(FILE)          sm__file_exists(FILE)
#define SM_FILE_GET_EXT(FILE)         sm__file_get_ext(FILE)
#define SM_FILE_READ(FILE)            sm__file_read(FILE)

#endif /* SM_CORE_UTIL_FILE_H */
