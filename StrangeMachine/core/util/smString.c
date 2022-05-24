#include "smpch.h"

#include "core/smAssert.h"
#include "core/smLog.h"
#include "core/smMem.h"

#include "core/data/smArray.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_STRING"

int32_t sm__string_compare(const char *a, const char *b) {

  SM_CORE_ASSERT(a);
  SM_CORE_ASSERT(b);

  uint8_t c1, c2;

  do {
    c1 = (uint8_t)*a++;
    c2 = (uint8_t)*b++;
    if (c1 == '\0')
      return c1 - c2;
  } while (c1 == c2);

  return c1 - c2;
}

bool sm__string_eq(const char *a, const char *b) {

  SM_CORE_ASSERT(a);
  SM_CORE_ASSERT(b);

  return sm__string_compare(a, b) == 0;
}

const char **sm__string_split(const char *str, char delim) {

  SM_CORE_ASSERT(str);

  if (str[0] == '\0')
    return NULL;

  SM_ARRAY(char *) result = NULL;
  size_t buf_index = 0;
  char buf[128];

  for (const char *p = str; *p != '\0'; ++p) {
    if (*p == delim || *(p + 1) == '\0') {

      if (buf_index > 0) {

        char *dup = SM_MALLOC(buf_index + 1);
        dup[buf_index] = '\0';
        strncpy(dup, buf, buf_index);

        SM_ARRAY_PUSH(result, dup);
        buf_index = 0;
      }
      continue;
    }

    SM_CORE_ASSERT(buf_index + 1 < sizeof(buf));
    buf[buf_index++] = *p;
  }

  return (const SM_ARRAY(char *))result;
}

const char *sm__string_to_lower(const char *str) {

  SM_CORE_ASSERT(str);

  size_t len = strlen(str) + 1;
  char *result = SM_MALLOC(len);
  strncpy(result, str, len);

  for (char *p = result; *p != '\0'; ++p) {
    *p = tolower(*p);
  }

  return result;
}

#undef SM_MODULE_NAME
