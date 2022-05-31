#ifndef SM_CORE_UTIL_FILESYSTEM_H
#define SM_CORE_UTIL_FILESYSTEM_H

#include "smpch.h"

#include "core/util/smFilesystemPub.h"
#include "core/util/smString.h"

/* File open modes. Can be combined. */
typedef enum sm__file_modes_e {
  SM_FILE_MODE_READ = 0x1,
  SM_FILE_MODE_WRITE = 0x2

} file_modes_e;

/*
 * Checks if a file exists.
 *
 * @param file_path The path to the file.
 * @return True if the file exists, false otherwise.
 */
bool sm_filesystem_exists(sm_string *file_path);

/*
 * Checks if a file exists. C string version.
 *
 * @param file_path The path to the file.
 * @return True if the file exists, false otherwise.
 */
bool sm_filesystem_exists_c_str(const char *file_path);

/*
 * Opens a file.
 *
 * @param file_path The path to the file.
 * @param mode The file open mode. See sm_file_modes_e.
 * @param binary If true, the file is opened in binary mode.
 * @param out_handle The file handle.
 * @return True if the file was opened successfully, false otherwise.
 */
bool sm_filesystem_open(sm_string *file_path, file_modes_e mode, bool binary, sm_file_handle_s *out_handle);

/*
 * Opens a file. C string version.
 *
 * @param file_path The path to the file.
 * @param mode The file open mode. See sm_file_modes_e.
 * @param binary If true, the file is opened in binary mode.
 * @param out_handle The file handle.
 * @return True if the file was opened successfully, false otherwise.
 */
bool sm_filesystem_open_c_str(const char *file_path, file_modes_e mode, bool binary, sm_file_handle_s *out_handle);

/*
 * Closes a file.
 *
 * @param handle The file handle.
 */
void sm_filesystem_close(sm_file_handle_s *handle);

/*
 * Gets the size of a file.
 *
 * @param handle The file handle.
 * @return The size of the file.
 */
uint64_t sm_filesystem_size(sm_file_handle_s *handle);

/*
 * Reads data from a file and puts it into a string.
 *
 * @param handle The file handle.
 * @return The string containing the data.
 */
sm_string *sm_filesystem_read_all_text(sm_file_handle_s *handle);

/*
 * Checks if the name of a file has a certain extension.
 * The extension is not case sensitive.
 * You can use multiple extensions by separating them with a `;`.
 * For example, `jpg;png` will match if the file is either a jpg or png.
 *
 * @param file_name The name of the file.
 * @param suffix The extensions to check.
 * @return True if the file has the extension, false otherwise.
 */
bool sm_filesystem_has_ext(sm_string *file_path, sm_string *suffix);

#endif /* SM_CORE_UTIL_FILESYSTEM_H */
