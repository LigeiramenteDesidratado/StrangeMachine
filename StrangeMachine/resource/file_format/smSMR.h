#ifndef SM_RESOURCE_FILE_FORMAT_SMR_H
#define SM_RESOURCE_FILE_FORMAT_SMR_H

#include "smpch.h"

#include "core/smCore.h"

#define SMR_RESOURCE_NONE  0x0ull
#define SMR_RESOURCE_SCENE 0x1ull

/* magic number 'SMR' */
#define SMR_MAGIC_NUMBER   0xFF524D53
#define SMR_VERSION_NUMBER 0x01ul

typedef struct smr__file_header_s {

  u32 signature;
  u32 version;
  u64 resouce_type;

} smr_file_header_s;

smr_file_header_s smr_file_header_new(u64 resource_type);
b8 smr_write_header(smr_file_header_s *header, sm_file_handle_s *file);
b8 smr_read_header(smr_file_header_s *header, sm_file_handle_s *file);

#endif /* SM_RESOURCE_FILE_FORMAT_SMR_H */
