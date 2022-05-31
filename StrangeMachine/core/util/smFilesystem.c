#include "smpch.h"

#include "core/smAssert.h"
#include "core/smMem.h"

#include "core/util/smFilesystem.h"
#include "core/util/smString.h"

#include "core/data/smArray.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_FILE"

bool sm_filesystem_exists(sm_string *file_path) {

  SM_CORE_ASSERT(file_path);

  return sm_filesystem_exists_c_str(sm_string_c_str(file_path));
}

bool sm_filesystem_exists_c_str(const char *file_path) {

  SM_CORE_ASSERT(file_path);

#ifdef _MSC_VER
  struct _stat sb;
  return _stat(file, &sb) == 0 && S_ISREG(sb.st_mode);
#else
  struct stat sb;
  return stat(file_path, &sb) == 0 && S_ISREG(sb.st_mode);
#endif
}

bool sm_filesystem_open(sm_string *file_path, file_modes_e mode, bool binary, sm_file_handle_s *out_handle) {

  SM_ASSERT(file_path);

  return sm_filesystem_open_c_str(sm_string_c_str(file_path), mode, binary, out_handle);
}

bool sm_filesystem_open_c_str(const char *file_path, file_modes_e mode, bool binary, sm_file_handle_s *out_handle) {

  SM_ASSERT(file_path);
  SM_ASSERT(out_handle);

  out_handle->is_valid = false;
  out_handle->handle = 0;
  const char *mode_str;

  if ((mode & SM_FILE_MODE_READ) != 0 && (mode & SM_FILE_MODE_WRITE) != 0) {
    mode_str = binary ? "w+b" : "w+";
  } else if ((mode & SM_FILE_MODE_READ) != 0 && (mode & SM_FILE_MODE_WRITE) == 0) {
    mode_str = binary ? "rb" : "r";
  } else if ((mode & SM_FILE_MODE_READ) == 0 && (mode & SM_FILE_MODE_WRITE) != 0) {
    mode_str = binary ? "wb" : "w";
  } else {
    /* SM_LOG_ERROR("Invalid mode passed while trying to open file: '%s'", file_path); */
    return false;
  }

  FILE *file = fopen(file_path, mode_str);
  if (!file) {
    /* SM_LOG_ERROR("Error opening file: '%s'", file_path); */
    return false;
  }

  out_handle->handle = file;
  out_handle->is_valid = true;

  return true;
}

/* wtf, why is this not in the standard library? */
void sm_filesystem_close(sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(handle->handle);

  fclose((FILE *)handle->handle);
  handle->handle = 0;
  handle->is_valid = false;
}

uint64_t sm_filesystem_size(sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);

  if (handle->handle) {

    fseek((FILE *)handle->handle, 0, SEEK_END);

    uint64_t size;
    size = (uint64_t)ftell((FILE *)handle->handle);
    rewind((FILE *)handle->handle);
    return size;
  }

  return 0;
}

sm_string *sm_filesystem_read_all_text(sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(handle->handle);

  // File size
  uint64_t size = sm_filesystem_size(handle);
  if (!size)
    return NULL;

  return sm_string_from_file_handle(handle, size);
}

const char *sm_filesystem_get_ext(const char *file) {

  const char *suffix = NULL;
  const char *dot = strrchr(file, '.');

  if (!dot || dot == file)
    return NULL;

  return suffix = dot + 1;
}

bool sm_filesystem_has_ext(sm_string *file_path, sm_string *suffix) {

  SM_CORE_ASSERT(file_path);
  SM_CORE_ASSERT(suffix);

  bool result = false;

  sm_string *ext = sm_string_from(sm_filesystem_get_ext(sm_string_c_str(file_path)));

  if (!ext)
    return false;

  SM_ARRAY(sm_string *) split = sm_string_split(suffix, ';');
  for (size_t i = 0; i < SM_ARRAY_LEN(split); i++) {

    if (!result && sm_string_eq(split[i], ext)) {
      result = true;
    }

    sm_string_dtor(split[i]);
  }

  sm_string_dtor(ext);
  SM_ARRAY_DTOR(split);

  return result;
}

/* #include "smpch.h" */
/**/
/* #include "core/smAssert.h" */
/* #include "core/smLog.h" */
/* #include "core/smMem.h" */
/**/
/* #include "core/util/smFile.h" */
/* #include "core/util/smString.h" */
/**/
/* #include "core/data/smArray.h" */
/**/
/* #undef SM_MODULE_NAME */
/* #define SM_MODULE_NAME "UTIL_FILE" */
/**/
/* bool s__file_has_ext(sm_string *file_path, const char *suffix) { */
/**/
/*   SM_CORE_ASSERT(file_path); */
/*   SM_CORE_ASSERT(suffix); */
/**/
/*   bool result = false; */
/**/
/*   const char *ext = sm__file_get_ext(file_path); */
/**/
/*   if (!ext) */
/*     return false; */
/**/
/*   SM_ARRAY(const char *) split = SM_STRING_SPLIT(suffix, ';'); */
/*   for (size_t i = 0; i < SM_ARRAY_LEN(split); i++) { */
/**/
/*     if (!result && SM_STRING_EQ(split[i], ext)) { */
/*       result = true; */
/*     } */
/**/
/*     SM_FREE((char *)split[i]); */
/*   } */
/**/
/*   SM_ARRAY_DTOR(split); */
/**/
/*   return result; */
/* } */
/**/
/* const char *sm__file_get_ext(const char *file) { */
/**/
/*   const char *suffix = NULL; */
/*   const char *dot = strrchr(file, '.'); */
/**/
/*   if (!dot || dot == file) */
/*     return NULL; */
/**/
/*   return suffix = dot + 1; */
/* } */
/**/
/* bool sm__file_exists(const char *file) { */
/**/
/*   SM_CORE_ASSERT(file); */
/* #ifdef _MSC_VER */
/*   struct _stat sb; */
/*   return _stat(file, &sb) == 0 && S_ISREG(sb.st_mode); */
/* #else */
/*   struct stat sb; */
/*   return stat(file, &sb) == 0 && S_ISREG(sb.st_mode); */
/* #endif */
/* } */
/**/
/* const char *sm__file_read(const char *file) { */
/**/
/*   SM_CORE_ASSERT(file); */
/**/
/*   char *text = NULL; */
/**/
/*   FILE *f = fopen(file, "rt"); */
/*   if (!f) { */
/*     SM_LOG_ERROR("[%s] failed to open text file", file); */
/*     return NULL; */
/*   } */
/**/
/*   fseek(f, 0, SEEK_END); */
/*   uint64_t size = (uint64_t)ftell(f); */
/*   fseek(f, 0, SEEK_SET); */
/**/
/*   if (size > 0) { */
/*     text = (char *)SM_MALLOC((size + 1) * sizeof(char)); */
/**/
/*     uint64_t count = fread(text, sizeof(char), size, f); */
/*
 * WARNING: \r\n is converted to \n on reading, so,
 * read bytes count gets reduced by the number of lines
 */
/*     if (count < size) */
/*       text = SM_REALLOC(text, count + 1); */
/**/
/*     text[count] = '\0'; */
/**/
/*   } else { */
/*     SM_LOG_ERROR("[%s] failed to read text file", file); */
/*   } */
/**/
/*   fclose(f); */
/**/
/*   SM_LOG_INFO("[%s] text file loaded successfully", file); */
/*   return text; */
/* } */
/**/
/* #undef SM_MODULE_NAME */
