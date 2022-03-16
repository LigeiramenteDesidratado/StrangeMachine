#ifndef UTIL_FILE_H
#define UTIL_FILE_H

#include "util/common.h"

#include "smMem.h"

char* read_file(const char* filename) {

  char* text = NULL;

  if (filename != NULL) {

    FILE *f = fopen(filename, "rt");

    if (f != NULL) {

      // WARNING: When reading a file as 'text' file,
      // text mode causes carriage return-linefeed translation...
      // ...but using fseek() should return correct byte-offset
      fseek(f, 0, SEEK_END);
      uint64_t size = (uint64_t)ftell(f);
      fseek(f, 0, SEEK_SET);

      if (size > 0) {
        text = (char*)SM_MALLOC((size + 1) * sizeof(char));

        uint64_t count = fread(text, sizeof(char), size, f);
        // WARNING: \r\n is converted to \n on reading, so,
        // read bytes count gets reduced by the number of lines
        if (count < size)
          text = SM_REALLOC(text, count + 1);

        // Zero-terminate the char*
        text[count] = '\0';

        SM_LOG_INFO("[%s] text file loaded successfully", filename);
      } else
        SM_LOG_ERROR("[%s] failed to read text file", filename);

      fclose(f);
    } else
      SM_LOG_ERROR("[%s] failed to open text file", filename);
  } else
    SM_LOG_ERROR("file name provided is not valid");

  return text;
}

#endif // UTIL_FILE_H
