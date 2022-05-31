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
#define SM__STRING_HEADER_OFFSET   (SM__STRING_LEN_HEADER_SIZE + SM__STRING_CAP_HEADER_SIZE)
#define SM__STRING_CHAR_SIZE       (sizeof(char))

#define SM__STRING_RAW(STRING)       ((size_t *)(STRING->_str - SM__STRING_HEADER_OFFSET))
#define SM__STRING_LEN(STRING)       (SM__STRING_RAW(STRING)[0])
#define SM__STRING_CAP(STRING)       (SM__STRING_RAW(STRING)[1])
#define SM__STRING_NULL_TERM(STRING) ((STRING->_str + SM__STRING_LEN(STRING))[0] = '\0')

typedef struct sm__string {

  /* LEN, CAPACITY, and STRING are all stored in the same memory block.
   * LEN is the number of characters in the string, not including the null
   * terminator. CAPACITY is the number of characters that can be stored in
   * the string, not including the null terminator. STRING is the pointer to the
   * first character in the string.
   */
  char *_str;

  /* rc is used to keep track of the number of references to this string.
   * When rc reaches 0, the string is freed.
   */
  sm_rc_s rc;

} sm_string;

sm_string *sm_string_ctor(size_t cap) {

  sm_string *string = SM_CALLOC(1, sizeof(sm_string));
  SM_CORE_ASSERT(string);

  char *_str = SM_MALLOC(SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * cap) + SM__STRING_CHAR_SIZE);
  SM_CORE_ASSERT(_str);

  string->_str = _str + SM__STRING_HEADER_OFFSET;

  SM__STRING_LEN(string) = 0;
  SM__STRING_CAP(string) = cap;
  string->_str[0] = '\0';

  string->rc = (sm_rc_s){1};

  return string;
}

sm_string *sm_string_from(const char *c_string) {

  SM_CORE_ASSERT(c_string);

  size_t len = strlen(c_string);

  sm_string *string = sm_string_ctor(len);

  /* copy the string pointed by c_string (including the null character) to the destination. */
  strcpy(string->_str, c_string);

  /* size_t* raw = SM__STRING_RAW(string->_str); */
  SM__STRING_LEN(string) = len;

  return string;
}

size_t sm_string_len(sm_string *string) {

  SM_CORE_ASSERT(string);

  return SM__STRING_LEN(string);
}

size_t sm_string_cap(sm_string *string) {

  SM_CORE_ASSERT(string);

  return SM__STRING_CAP(string);
}

void *sm_string_dtor(sm_string *string) {

  SM_CORE_ASSERT(string);

  sm_rc_dec(&string->rc);
  if (string->rc.count == 0) {
    SM_FREE(SM__STRING_RAW(string));
    SM_FREE(string);
  }

  return NULL;
}

const char *sm_string_c_str(sm_string *string) {

  SM_CORE_ASSERT(string);
  SM_CORE_ASSERT(string->_str);

  return string->_str;
}

/*
 * <0, if the first non-matching character in str1 is lower (in ASCII) than that of str2.
 *  0, if strings are equal
 * >0, if the first non-matching character in str1 is greater (in ASCII) than that of str2.
 */
int32_t sm_string_compare(sm_string *restrict  str1, sm_string * restrict str2) {

  SM_CORE_ASSERT(str1->_str);
  SM_CORE_ASSERT(str2->_str);

  uint8_t c1, c2;

  uint8_t *a = (uint8_t *)str1->_str;
  uint8_t *b = (uint8_t *)str2->_str;

  do {
    c1 = (uint8_t)*a++;
    c2 = (uint8_t)*b++;
    if (c1 == '\0')
      return c1 - c2;
  } while (c1 == c2);

  return c1 - c2;
}

bool sm_string_eq(sm_string *str1, sm_string *str2) {

  SM_CORE_ASSERT(str1->_str);
  SM_CORE_ASSERT(str2->_str);

  if (SM__STRING_LEN(str1) == SM__STRING_LEN(str2))
    return sm_string_compare(str1, str2) == 0;

  return false;
}

SM_ARRAY(sm_string *) sm_string_split(sm_string *string, char delim) {

  SM_CORE_ASSERT(string);

  char *str = string->_str;

  if (str[0] == '\0')
    return NULL;

  SM_ARRAY(sm_string *) result = NULL;
  size_t buf_index = 0;
  char buf[128];

  for (size_t i = 0; i < SM__STRING_LEN(string); ++i) {
    if (str[i] == delim) {

      if (buf_index > 0) {
        buf[buf_index] = '\0';
        sm_string *new_str = sm_string_from(buf);

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
    sm_string *new_str = sm_string_from(buf);

    SM_ARRAY_PUSH(result, new_str);
  }

  return result;
}

/* make a deep copy */
sm_string *sm_string_copy(sm_string *string) {

  SM_CORE_ASSERT(string);

  sm_string *new_string = sm_string_from(string->_str);

  return new_string;
}

/* make a shallow copy */
sm_string *sm_string_reference(sm_string *string) {

  SM_CORE_ASSERT(string);

  sm_rc_inc(&string->rc);

  return string;
}

sm_string *sm_string_to_lower(sm_string *string) {

  SM_CORE_ASSERT(string);

  sm_string *lower = sm_string_copy(string);

  char *str = lower->_str;
  for (size_t i = 0; i < SM__STRING_LEN(lower); ++i)
    str[i] = tolower(str[i]);

  return lower;
}

sm_string *sm_string_to_upper(sm_string *string) {

  SM_CORE_ASSERT(string);

  sm_string *upper = sm_string_copy(string);

  char *str = upper->_str;
  for (size_t i = 0; i < SM__STRING_LEN(upper); ++i)
    str[i] = toupper(str[i]);

  return upper;
}

/*
 * sm_string_trim - Trim leading and trailing whitespace from a string.
 * All changes are made in place
 */
void sm_string_trim(sm_string *string) {

  SM_CORE_ASSERT(string);

  size_t len = SM__STRING_LEN(string);
  char *str = string->_str;

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

void sm_string_append(sm_string *string, sm_string *append) {

  SM_CORE_ASSERT(string);
  SM_CORE_ASSERT(append);

  size_t len = SM__STRING_LEN(string);
  size_t append_len = SM__STRING_LEN(append);

  SM__STRING_LEN(string) += append_len;

  if (SM__STRING_LEN(string) > SM__STRING_CAP(string)) {
    SM__STRING_CAP(string) = SM__STRING_LEN(string);

    void *__temp =
        SM_REALLOC(SM__STRING_RAW(string),
                   SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * SM__STRING_CAP(string)) + SM__STRING_CHAR_SIZE);

    SM_CORE_ASSERT(__temp);

    string->_str = __temp + SM__STRING_HEADER_OFFSET;
  }

  char *str = string->_str;
  char *append_str = append->_str;

  memcpy(str + len, append_str, append_len);

  /* apply the null operator at the end */
  SM__STRING_NULL_TERM(string);
}

sm_string *sm_string_from_file_handle(sm_file_handle_s *file_handle, uint64_t size) {

  sm_string *string = sm_string_ctor(size);

  uint64_t bytes_read = fread(string->_str, sizeof(char), size, (FILE *)file_handle->handle);
  SM__STRING_LEN(string) = size;

  if (bytes_read < size) {

    void *__temp = SM_REALLOC(SM__STRING_RAW(string),
                              SM__STRING_HEADER_OFFSET + (SM__STRING_CHAR_SIZE * bytes_read) + SM__STRING_CHAR_SIZE);

    SM_CORE_ASSERT(__temp);

    string->_str = __temp + SM__STRING_HEADER_OFFSET;

    SM__STRING_LEN(string) = bytes_read;
    SM__STRING_CAP(string) = bytes_read;
  }
  SM__STRING_NULL_TERM(string);

  return string;
}

#undef SM_MODULE_NAME
