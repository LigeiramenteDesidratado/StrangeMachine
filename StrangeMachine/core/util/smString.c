#include "smpch.h"

#include "core/data/smArray.h"
#include "core/smAssert.h"
#include "core/smMem.h"
#include "core/smRc.h"
#include "core/util/smFilesystem.h"
#include "core/util/smString.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_STRING"

#define SM__STRING_LEN_HEADER_SIZE (sizeof(size_t))
#define SM__STRING_CAP_HEADER_SIZE (sizeof(size_t))
#define SM__STRING_RC_HEADER_SIZE  (sizeof(atomic_size_t))
#define SM__STRING_HEADER_OFFSET   (SM__STRING_LEN_HEADER_SIZE + SM__STRING_CAP_HEADER_SIZE + SM__STRING_RC_HEADER_SIZE)
#define SM__STRING_CHAR_SIZE       (sizeof(char))

#define SM__STRING_RAW(STRING) ((size_t *)(STRING.str - SM__STRING_HEADER_OFFSET))
#define SM__STRING_LEN(STRING) (SM__STRING_RAW(STRING)[0])
#define SM__STRING_CAP(STRING) (SM__STRING_RAW(STRING)[1])
#define SM__STRING_RC(STRING)  (SM__STRING_RAW(STRING)[2])

#define SM__STRING_NULL_TERM(STRING) ((STRING.str + SM__STRING_LEN(STRING))[0] = '\0')

sm_string sm_string_ctor(size_t cap) {

  char *str = SM_MALLOC(SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * cap) + SM__STRING_CHAR_SIZE);
  SM_CORE_ASSERT(str);

  sm_string string;

  string.str = str + SM__STRING_HEADER_OFFSET;

  SM__STRING_LEN(string) = 0;
  SM__STRING_CAP(string) = cap;
  SM__STRING_RC(string) = 1;
  SM__STRING_NULL_TERM(string);

  return string;
}

sm_string sm_string_from(const char *c_string) {

  SM_CORE_ASSERT(c_string);

  size_t len = strlen(c_string);

  sm_string string = sm_string_ctor(len);

  /* copy the string pointed by c_string (including the null character) to the destination. */
  strcpy(string.str, c_string);

  SM__STRING_LEN(string) = len;

  return string;
}

size_t sm_string_len(sm_string string) {

  SM_CORE_ASSERT(string.str);

  return SM__STRING_LEN(string);
}

void sm_string_set_len(sm_string string, size_t len) {

  SM_CORE_ASSERT(string.str);

  SM__STRING_LEN(string) = len;

  SM_CORE_ASSERT(SM__STRING_LEN(string) <= SM__STRING_CAP(string));

  SM__STRING_NULL_TERM(string);
}

size_t sm_string_cap(sm_string string) {

  SM_CORE_ASSERT(string.str);

  return SM__STRING_CAP(string);
}

void *sm_string_dtor(sm_string string) {

  SM_CORE_ASSERT(string.str);

  atomic_fetch_sub((atomic_size_t *)(string.str - sizeof(atomic_size_t)), 1);
  if (SM__STRING_RC(string) == 0)
    SM_FREE(SM__STRING_RAW(string));

  return NULL;
}

/*
 * <0, if the first non-matching character in str1 is lower (in ASCII) than that of str2.
 *  0, if strings are equal
 * >0, if the first non-matching character in str1 is greater (in ASCII) than that of str2.
 */
i32 sm_string_compare(sm_string str1, sm_string str2) {

  SM_CORE_ASSERT(str1.str);
  SM_CORE_ASSERT(str2.str);

  return sm_string_compare_c_str(str1, str2.str);
}

b8 sm_string_eq(sm_string str1, sm_string str2) {

  SM_CORE_ASSERT(str1.str);
  SM_CORE_ASSERT(str2.str);

  if (SM__STRING_LEN(str1) == SM__STRING_LEN(str2))
    return sm_string_compare_c_str(str1, str2.str) == 0;

  return false;
}

b8 sm_string_eq_c_str(sm_string str, const char *c_str) {

  SM_CORE_ASSERT(str.str);
  SM_CORE_ASSERT(c_str);

  return sm_string_compare_c_str(str, c_str) == 0;
}

b8 sm_string_compare_c_str(sm_string str, const char *c_str) {

  SM_CORE_ASSERT(str.str);
  SM_CORE_ASSERT(c_str);

  u8 c1, c2;

  u8 *a = (u8 *)str.str;
  u8 *b = (u8 *)c_str;

  do {
    c1 = *a++;
    c2 = *b++;
    if (c1 == '\0')
      return c1 - c2;
  } while (c1 == c2);

  return c1 - c2;
}

SM_ARRAY(sm_string) sm_string_split(sm_string string, char delim) {

  SM_CORE_ASSERT(string.str);

  char *str = string.str;

  if (str[0] == '\0')
    return NULL;

  SM_ARRAY(sm_string) result = NULL;
  size_t buf_index = 0;
  char buf[128];

  for (size_t i = 0; i < SM__STRING_LEN(string); ++i) {
    if (str[i] == delim) {

      if (buf_index > 0) {
        buf[buf_index] = '\0';
        sm_string new_str = sm_string_from(buf);

        SM_ARRAY_PUSH(result, new_str);
        buf_index = 0;
      }

      continue;
    }

    SM_CORE_ASSERT(buf_index + 1 < sizeof(buf));
    buf[buf_index++] = str[i];
  }

  if (buf_index > 0) {
    buf[buf_index] = '\0';
    sm_string new_str = sm_string_from(buf);

    SM_ARRAY_PUSH(result, new_str);
  }

  return result;
}

SM_ARRAY(sm_string) sm_string_split_c_str(const char *string, char delim) {

  SM_CORE_ASSERT(string);
  SM_CORE_ASSERT(delim);

  if (string[0] == '\0')
    return NULL;

  SM_ARRAY(sm_string) result = NULL;
  size_t buf_index = 0;
  char buf[128];
  size_t len = strlen(string);

  for (size_t i = 0; i < len; ++i) {
    if (string[i] == delim) {

      if (buf_index > 0) {
        buf[buf_index] = '\0';
        sm_string new_str = sm_string_from(buf);

        SM_ARRAY_PUSH(result, new_str);
        buf_index = 0;
      }

      continue;
    }

    SM_CORE_ASSERT(buf_index + 1 < sizeof(buf));
    buf[buf_index++] = string[i];
  }

  if (buf_index > 0) {
    buf[buf_index] = '\0';
    sm_string new_str = sm_string_from(buf);

    SM_ARRAY_PUSH(result, new_str);
  }

  return result;
}

/* make a deep copy */
sm_string sm_string_copy(sm_string string) {

  SM_CORE_ASSERT(string.str);

  sm_string new_string = sm_string_from(string.str);

  return new_string;
}

/* make a shallow copy */
sm_string sm_string_reference(sm_string string) {

  SM_CORE_ASSERT(string.str);

  atomic_fetch_add((atomic_size_t *)(string.str - sizeof(atomic_size_t)), 1);

  return string;
}

sm_string sm_string_to_lower(sm_string string) {

  SM_CORE_ASSERT(string.str);

  sm_string lower = sm_string_copy(string);

  char *str = lower.str;
  for (size_t i = 0; i < SM__STRING_LEN(lower); ++i)
    str[i] = tolower(str[i]);

  return lower;
}

sm_string sm_string_to_upper(sm_string string) {

  SM_CORE_ASSERT(string.str);

  sm_string upper = sm_string_copy(string);

  char *str = upper.str;
  for (size_t i = 0; i < SM__STRING_LEN(upper); ++i)
    str[i] = toupper(str[i]);

  return upper;
}

/*
 * sm_string_trim - Trim leading and trailing whitespace from a string.
 * All changes are made in place
 */
void sm_string_trim(sm_string string) {

  SM_CORE_ASSERT(string.str);

  size_t len = SM__STRING_LEN(string);
  char *str = string.str;

  while (len > 0 && isspace(str[len - 1]))
    --len;

  SM__STRING_LEN(string) = len;

  /* string contains only whitespace */
  if (len == 0) {
    str[0] = '\0';
    return;
  }

  size_t i = 0;
  while (isspace(str[i]))
    ++i;

  if (i > 0) {
    memmove(str, str + i, len - i);
    SM__STRING_LEN(string) -= i;
  }

  str[SM__STRING_LEN(string)] = '\0';

  SM_CORE_ASSERT(SM__STRING_LEN(string) == strlen(str));

  return;
}

sm_string sm_string_append(sm_string string, sm_string append) {

  SM_CORE_ASSERT(string.str);
  SM_CORE_ASSERT(append.str);

  size_t len = SM__STRING_LEN(string);
  size_t append_len = SM__STRING_LEN(append);

  sm_string new_string = sm_string_ctor(len + append_len);

  memcpy(new_string.str, string.str, len);
  memcpy(new_string.str + len, append.str, append_len);

  SM__STRING_LEN(new_string) = len + append_len;

  /* apply the null operator at the end */
  SM__STRING_NULL_TERM(new_string);

  return new_string;
}

sm_string sm_string_append_c_str(sm_string string, const char *append) {

  SM_CORE_ASSERT(string.str);
  SM_CORE_ASSERT(append);

  size_t len = SM__STRING_LEN(string);
  size_t append_len = strlen(append);

  sm_string new_string = sm_string_ctor(len + append_len);

  memcpy(new_string.str, string.str, len);
  memcpy(new_string.str + len, append, append_len);

  SM__STRING_LEN(new_string) = len + append_len;

  /* apply the null operator at the end */
  SM__STRING_NULL_TERM(string);

  return new_string;
}

b8 sm_string_set(sm_string string, const char *value) {

  SM_CORE_ASSERT(string.str);
  SM_CORE_ASSERT(value);

  size_t len = strlen(value);

  if (len > SM__STRING_CAP(string)) {
    SM__STRING_CAP(string) = len;

    char *__temp =
        SM_REALLOC(SM__STRING_RAW(string),
                   SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * SM__STRING_CAP(string)) + SM__STRING_CHAR_SIZE);

    SM_CORE_ASSERT(__temp);

    string.str = __temp + SM__STRING_HEADER_OFFSET;
  }

  SM__STRING_LEN(string) = len;

  memcpy(string.str, value, len);

  /* apply the null operator at the end */
  SM__STRING_NULL_TERM(string);

  return true;
}

sm_string sm_string_from_file_handle(const sm_file_handle_s *file_handle, u64 size) {

  sm_string string = sm_string_ctor(size);

  u64 bytes_read = fread(string.str, sizeof(char), size, (FILE *)file_handle->handle);
  SM__STRING_LEN(string) = size;

  if (bytes_read < size) {

    char *__temp = SM_REALLOC(SM__STRING_RAW(string),
                              SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * bytes_read) + SM__STRING_CHAR_SIZE);

    SM_CORE_ASSERT(__temp);

    string.str = __temp + SM__STRING_HEADER_OFFSET;

    SM__STRING_LEN(string) = bytes_read;
    SM__STRING_CAP(string) = bytes_read;
  }
  SM__STRING_NULL_TERM(string);

  return string;
}

b8 sm_string_contains(sm_string string, sm_string substring) {

  return sm_string_contains_c_str(string, substring.str);
}

b8 sm_string_contains_c_str(sm_string string, const char *substring) {

  SM_CORE_ASSERT(string.str);
  SM_CORE_ASSERT(substring);

  size_t substring_len = strlen(substring);

  if (substring_len > SM__STRING_LEN(string))
    return false;

  for (size_t i = 0; i < SM__STRING_LEN(string) - substring_len + 1; ++i) {

    b8 result = true;

    for (size_t j = 0; j < substring_len; ++j) {
      if (string.str[i + j] != substring[j]) {
        result = false;
        break;
      }
    }

    if (result)
      return true;
  }

  return false;
}

#undef SM_MODULE_NAME
