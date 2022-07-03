#ifndef SM_RESOURCES_H
#define SM_RESOURCES_H

#include "smpch.h"

#include "core/smCore.h"
#include "resource/smTextureResourcePub.h"

typedef enum {
  RESOURCE_STATUS_NONE = 0,
  RESOURCE_STATUS_FOUND = 1 << 0,
  /* if the file was once found, but sudenly is gone (generally because of a manual delete) */
  RESOURCE_STATUS_NOT_FOUND = 1 << 1,
  RESOURCE_STATUS_RELOADED = 1 << 2, /* if the file was changed on disk this flag is set */
  RESOURCE_STATUS_INVALID = 1 << 3,

  RESOURCE_STATUS_MASK_ALL =
      RESOURCE_STATUS_FOUND | RESOURCE_STATUS_NOT_FOUND | RESOURCE_STATUS_RELOADED | RESOURCE_STATUS_INVALID

} resource_manager_status_e;

typedef enum {
  RESOURCE_TYPE_NONE = 0,
  RESOURCE_TYPE_TEXTURE = 1 << 0,
  RESOURCE_TYPE_AUDIO = 1 << 1,
  RESOURCE_TYPE_SHADER = 1 << 2,
  RESOURCE_TYPE_MODEL = 1 << 3,
  RESOURCE_TYPE_INVALID = 1 << 4,

  RESOURCE_TYPE_MASK_ALL =
      RESOURCE_TYPE_TEXTURE | RESOURCE_TYPE_AUDIO | RESOURCE_TYPE_SHADER | RESOURCE_TYPE_MODEL | RESOURCE_TYPE_INVALID

} resource_manager_type_e;

typedef struct {

  resource_manager_type_e type;
  resource_manager_status_e status;
  u32 handle;

} sm_resource_s;

b8 sm_resource_manager_init(const char *folder);
void sm_resource_manager_teardown();
sm_resource_s *sm_resource_manager_get(sm_string key);

typedef struct sm__resource_iter_s {

  resource_manager_type_e type;
  resource_manager_status_e status;
  size_t index;
} sm_resource_manager_iter_s;

sm_resource_manager_iter_s sm_resource_manager_iter_new(resource_manager_type_e type, resource_manager_status_e status);
const char *sm_resource_iter_next(sm_resource_manager_iter_s *iter);
void sm_resource_iter_free(sm_resource_manager_iter_s *iter);

#endif /* SM_RESOURCES_H */
