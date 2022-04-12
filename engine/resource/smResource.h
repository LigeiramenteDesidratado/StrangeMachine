#ifndef SM_RESOURCES_H
#define SM_RESOURCES_H

#include "resource/smTextureResourcePub.h"
#include "smpch.h"

typedef enum {
  RESOURCE_STATUS_NONE = 0,
  RESOURCE_STATUS_FOUND = 1 << 0,
  /* if the file was once found, but sudenly is gone (generally because of a manual delete) */
  RESOURCE_STATUS_NOT_FOUND = 1 << 1,
  RESOURCE_STATUS_RELOADED = 1 << 2, /* if the file was changed on disk this flag is set */
  RESOURCE_STATUS_INVALID = 1 << 3,

  RESOURCE_STATUS_MASK_ALL =
      RESOURCE_STATUS_FOUND | RESOURCE_STATUS_NOT_FOUND | RESOURCE_STATUS_RELOADED | RESOURCE_STATUS_INVALID

} resource_status_e;

typedef enum {
  RESOURCE_TYPE_NONE = 0,
  RESOURCE_TYPE_IMAGE = 1 << 0,
  RESOURCE_TYPE_AUDIO = 1 << 1,
  RESOURCE_TYPE_SHADER = 1 << 2,
  RESOURCE_TYPE_MODEL = 1 << 3,
  RESOURCE_TYPE_INVALID = 1 << 4,

  FILE_TYPE_MASK_ALL =
      RESOURCE_TYPE_IMAGE | RESOURCE_TYPE_AUDIO | RESOURCE_TYPE_SHADER | RESOURCE_TYPE_MODEL | RESOURCE_TYPE_INVALID

} resource_type_e;

typedef struct {

  resource_status_e status;

  resource_type_e type;

} resource_s;

bool resource_init(const char *folder);
void resource_teardown();
resource_s *resource_get(const char *key);

typedef struct {

  resource_type_e type;
  resource_status_e status;
  size_t index;
} resource_iter_s;

resource_iter_s resource_iter_new(resource_type_e type, resource_status_e status);
const char *resource_iter_next(resource_iter_s *iter);
void resource_iter_free(resource_iter_s *iter);

texture_handler_s resource_load_texture(const char *resource);

#endif /* SM_RESOURCES_H */
