#include "smpch.h"

#include "core/smAssert.h"
#include "core/smLog.h"
#include "core/smMem.h"

#include "core/util/smFile.h"
#include "core/util/smString.h"

#include "core/data/smArray.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_FILE"

bool sm__file_has_ext(const char *file, const char *suffix) {

  SM_CORE_ASSERT(file);
  SM_CORE_ASSERT(suffix);

  bool result = false;

  const char *ext = sm__file_get_ext(file);

  if (!ext)
    return false;

  const char **split = SM_STRING_SPLIT(suffix, ';');
  for (size_t i = 0; i < SM_ARRAY_SIZE(split); i++) {

    if (!result && SM_STRING_EQ(split[i], ext)) {
      result = true;
    }

    SM_FREE((char *)split[i]);
  }

  SM_ARRAY_DTOR(split);

  return result;
}

const char *sm__file_get_ext(const char *file) {

  const char *suffix = NULL;
  const char *dot = strrchr(file, '.');

  if (!dot || dot == file)
    return NULL;

  return suffix = dot + 1;
}

bool sm__file_exists(const char *file) {

  SM_CORE_ASSERT(file);

  struct stat sb;
  return stat(file, &sb) == 0 && S_ISREG(sb.st_mode);
}

const char *sm__file_read(const char *file) {

  SM_CORE_ASSERT(file);

  char *text = NULL;

  FILE *f = fopen(file, "rt");
  if (!f) {
    SM_LOG_ERROR("[%s] failed to open text file", file);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  uint64_t size = (uint64_t)ftell(f);
  fseek(f, 0, SEEK_SET);

  if (size > 0) {
    text = (char *)SM_MALLOC((size + 1) * sizeof(char));

    uint64_t count = fread(text, sizeof(char), size, f);
    /* WARNING: \r\n is converted to \n on reading, so, */
    /* read bytes count gets reduced by the number of lines */
    if (count < size)
      text = SM_REALLOC(text, count + 1);

    text[count] = '\0';

  } else {
    SM_LOG_ERROR("[%s] failed to read text file", file);
  }

  fclose(f);

  SM_LOG_INFO("[%s] text file loaded successfully", file);
  return text;
}
#undef SM_MODULE_NAME
