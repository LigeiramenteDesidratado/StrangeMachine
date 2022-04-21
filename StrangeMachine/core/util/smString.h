#ifndef SM_CORE_UTIL_STRING_H
#define SM_CORE_UTIL_STRING_H

#include "smpch.h"

int32_t sm__string_compare(const char *a, const char *b);
bool sm__string_eq(const char *a, const char *b);
const char **sm__string_split(const char *str, char delim);
const char *sm__string_to_lower(const char *str);

#define SM_STRING_COMPARE(A, B)     sm__string_compare(A, B)
#define SM_STRING_EQ(A, B)          sm__string_eq(A, B)
#define SM_STRING_SPLIT(STR, DELIM) sm__string_split(STR, DELIM)
#define SM_STRING_TO_LOWER(STR)     sm__string_to_lower(STR)

#endif /* SM_CORE_UTIL_STRING_H */
