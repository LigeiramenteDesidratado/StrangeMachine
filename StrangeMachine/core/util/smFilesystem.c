#include "smpch.h"

#include "core/smAssert.h"
#include "core/smMem.h"

#include "core/util/smFilesystem.h"
#include "core/util/smString.h"

#include "core/data/smArray.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "UTIL_FILE"

b8 sm_filesystem_exists(sm_string file_path) {

  return sm_filesystem_exists_c_str(file_path.str);
}

b8 sm_filesystem_exists_c_str(const char *file_path) {

  SM_CORE_ASSERT(file_path);

#ifdef _MSC_VER
  struct _stat sb;
  return _stat(file, &sb) == 0 && S_ISREG(sb.st_mode);
#else
  struct stat sb;
  return stat(file_path, &sb) == 0 && S_ISREG(sb.st_mode);
#endif
}

b8 sm_filesystem_open(sm_string file_path, file_modes_e mode, b8 binary, sm_file_handle_s *out_handle) {

  return sm_filesystem_open_c_str(file_path.str, mode, binary, out_handle);
}

b8 sm_filesystem_open_c_str(const char *file_path, file_modes_e mode, b8 binary, sm_file_handle_s *out_handle) {

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
    SM_LOG_ERROR("[%s] invalid mode passed while trying to open file", file_path);
    return false;
  }

  FILE *file = fopen(file_path, mode_str);
  if (!file) {
    SM_LOG_ERROR("[%s] error opening file", file_path);
    return false;
  }

  out_handle->handle = file;
  out_handle->is_valid = true;

  return true;
}

b8 sm_filesystem_close(sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(handle->handle);

  int re = fclose((FILE *)handle->handle);
  handle->handle = 0;
  handle->is_valid = false;

  return re == 0;
}

u64 sm_filesystem_size(const sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);

  if (handle->handle) {

    fseek((FILE *)handle->handle, 0, SEEK_END);

    u64 size;
    size = (u64)ftell((FILE *)handle->handle);
    rewind((FILE *)handle->handle);
    return size;
  }

  return 0;
}

sm_string sm_filesystem_read_all_text(const sm_file_handle_s *handle) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(handle->handle);

  // File size
  u64 size = sm_filesystem_size(handle);
  if (!size)
    return (sm_string){NULL};

  return sm_string_from_file_handle(handle, size);
}

const char *sm_filesystem_get_ext_c_str(const char *filename) {

  const char *suffix = NULL;
  const char *dot = strrchr(filename, '.');

  if (!dot || dot == filename)
    return NULL;

  return suffix = dot + 1;
}

sm_string sm_filesystem_get_ext(sm_string filename) {

  return sm_string_from(sm_filesystem_get_ext_c_str(filename.str));
}

b8 sm_filesystem_has_ext(sm_string filename, sm_string suffix) {

  SM_CORE_ASSERT(filename.str);
  SM_CORE_ASSERT(suffix.str);

  sm_string ext = sm_filesystem_get_ext(filename);

  if (!ext.str)
    return false;

  b8 result = false;
  SM_ARRAY(sm_string) split = sm_string_split(suffix, ';');
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

b8 sm_filesystem_has_ext_c_str(const char *filename, const char *suffix) {

  SM_CORE_ASSERT(filename);
  SM_CORE_ASSERT(suffix);

  b8 result = false;

  const char *ext = sm_filesystem_get_ext_c_str(filename);
  if (!ext)
    return false;

  sm_string ext_str = sm_string_from(ext);
  sm_string suffix_str = sm_string_from(suffix);

  SM_ARRAY(sm_string) split = sm_string_split(suffix_str, ';');
  for (size_t i = 0; i < SM_ARRAY_LEN(split); i++) {

    if (!result && sm_string_eq(split[i], ext_str)) {
      result = true;
    }

    sm_string_dtor(split[i]);
  }

  SM_ARRAY_DTOR(split);

  sm_string_dtor(ext_str);
  sm_string_dtor(suffix_str);

  return result;
}

b8 sm_filesystem_read_line(const sm_file_handle_s *handle, sm_string string_buf) {

  SM_CORE_ASSERT(handle);

  char *buf = string_buf.str;
  size_t buf_cap = sm_string_cap(string_buf);

  if (fgets(buf, buf_cap, (FILE *)handle->handle) != NULL) {
    sm_string_set_len(string_buf, strlen(buf));

    return true;
  }

  return false;
}

b8 filesystem_write_line(sm_file_handle_s *handle, sm_string string) {

  SM_CORE_ASSERT(handle);

  i32 result = fputs(string.str, (FILE *)handle->handle);

  if (result != EOF)
    result = fputc('\n', (FILE *)handle->handle);

  /* Make sure to flush the stream so it is written to the file immediately. */
  /* This prevents data loss in the event of a crash. */
  fflush((FILE *)handle->handle);

  return result != EOF;
}

b8 sm_filesystem_write_bytes(const sm_file_handle_s *handle, const void *data, u64 size) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(data);

  u32 result = fwrite(data, 1, size, (FILE *)handle->handle);

  /* Make sure to flush the stream so it is written to the file immediately. */
  /* This prevents data loss in the event of a crash. */
  fflush((FILE *)handle->handle);

  return result != 0;
}

b8 sm_filesystem_read_bytes(const sm_file_handle_s *handle, void *data, u64 size) {

  SM_CORE_ASSERT(handle);
  SM_CORE_ASSERT(data);

  u32 result = fread(data, 1, size, (FILE *)handle->handle);

  return result != 0;
}

#undef SM_MODULE_NAME
