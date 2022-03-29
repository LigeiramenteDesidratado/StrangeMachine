#include "smpch.h"

#include "core/smCore.h"

#include "core/data/smArray.h"
#include "core/data/smHashTable.h"

#include "core/util/smBitMask.h"

#include "smResources.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "RESOURCE"

#define DMON_IMPL

#define DMON_MALLOC  SM_MALLOC
#define DMON_FREE    SM_FREE
#define DMON_REALLOC SM_REALLOC

#define DMON_ASSERT SM_ASSERT

#define DMON_LOG_ERROR   SM_LOG_ERROR
#define DMON_LOG_DEBUG   SM_LOG_DEBUG
#define _DMON_LOG_ERRORF SM_LOG_ERROR
#define _DMON_LOG_DEBUGF SM_LOG_DEBUG
#include "vendor/dmon/dmon.h"

typedef struct {
  char *key;
  resource_s value;

} resource_m;

typedef struct {

  const char *root_folder;

  resource_m *map;

} resource_manager_s;

/* globals */
static resource_manager_s RESOURCE = {0};
static bool IS_RESOURCE_INIT = false;

/* supported resource types */
static char *expected_ext = "jpeg;jpg;png;gltf;glb;obj;mtl;mp3;ogg;glsl;vert;frag;fs;vs;";

static resource_type_e sm__resource_get_file_type(const char *file) {

  SM_ASSERT(file);

  if (SM_FILE_HAS_EXT(file, "jpg;jpeg;png;"))
    return RESOURCE_TYPE_IMAGE;
  if (SM_FILE_HAS_EXT(file, "mp3;ogg"))
    return RESOURCE_TYPE_AUDIO;
  if (SM_FILE_HAS_EXT(file, "glsl;vert;frag;fs;vs;"))
    return RESOURCE_TYPE_SHADER;
  if (SM_FILE_HAS_EXT(file, "fbx;obj;gltf;glb"))
    return RESOURCE_TYPE_MODEL;

  return RESOURCE_TYPE_INVALID;
}

static void sm__resource_on_found(const char *path) {

  SM_ASSERT(path);

  if (!SM_FILE_EXISTS(path)) {
    SM_LOG_WARN("[%s] resource does not exist (NOT_FOUNT)", path);
    return;
  }

  SM_LOG_DEBUG("[%s] resource found (FOUND)", path);

  resource_type_e ft = sm__resource_get_file_type(path);
  if (ft == RESOURCE_TYPE_INVALID) {
    SM_LOG_WARN("[%s] resource supported", path);
    return;
  }

  resource_s res;
  res.type = ft;
  res.status = RESOURCE_STATUS_FOUND;

  SM_SH_INSERT(RESOURCE.map, path, res);
}

static void sm__resource_on_delete(const char *path) {

  SM_ASSERT(path);

  if (!SM_SH_DELETE(RESOURCE.map, path))
    SM_LOG_WARN("[%s] resource does not exist", path);
  else
    SM_LOG_DEBUG("[%s] resource deleted", path);
}

static void sm__resource_on_reload(const char *path) {

  SM_ASSERT(path);

  resource_s *res = &SM_SH_GET(RESOURCE.map, path);
  if (!res) {
    SM_LOG_WARN("[%s] resource not found in the map (NOT_FOUND)", path);
    return;
  }

  SM_LOG_DEBUG("[%s] resource reloaded", path);
  res->status |= RESOURCE_STATUS_RELOADED;
}

static void sm__watch_cb(dmon_watch_id watch_id, dmon_action action, const char *rootdir, const char *filepath,
                         const char *oldfilepath, void *user) {

  SM_ASSERT(rootdir);
  SM_ASSERT(filepath);
  SM_ASSERT(action < 5);

  SM_UNUSED(user);
  SM_UNUSED(watch_id);
  SM_UNUSED(oldfilepath);

  /* receive change events. type of event is stored in 'action' variable */

  if (!SM_FILE_HAS_EXT(filepath, expected_ext)) {
    SM_LOG_WARN("[%s] resource not supported", filepath);
    return;
  }

  static void (*handler[5])(const char *key) = {

      [DMON_ACTION_CREATE] = sm__resource_on_found,
      [DMON_ACTION_DELETE] = sm__resource_on_delete,
      [DMON_ACTION_MODIFY] = sm__resource_on_reload,
      [DMON_ACTION_MOVE] = sm__resource_on_reload,
  };

  char *key = SM_MALLOC(strlen(rootdir) + strlen(filepath) + 1);
  strcpy(key, rootdir);
  strcat(key, filepath);

  handler[action](key);

  SM_FREE(key);
}

static void sm__resource_dir_read(const char *folder) {

  DIR *dir = opendir(folder);
  if (!dir) {
    return;
  }

  size_t len = strlen(folder);

  char root[256];
  strcpy(root, folder);

  char *buf = root + len;

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {

    /* check if it is a regular file */
    if (SM_MASK_CHK(ent->d_type, DT_REG)) {

      /* check if it is a supported file type */
      if (SM_FILE_HAS_EXT(ent->d_name, expected_ext)) {

        /* create the full path */
        strcpy(buf, ent->d_name);

        sm__resource_on_found(root);
      }
    } else if (SM_MASK_CHK(ent->d_type, DT_DIR)) {

      /* check if it is a directory */
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
        continue;
      }

      /* create the full path */
      strcpy(buf, ent->d_name);
      strcat(buf, "/");

      /* recurse */
      sm__resource_dir_read(root);
    }
  }

  closedir(dir);
}

bool resource_init(const char *root_folder) {

  SM_ASSERT(root_folder);

  SM_ASSERT(!IS_RESOURCE_INIT && "resource manager already initialized");

  RESOURCE.root_folder = strdup(root_folder);

  SM_SH_STRDUP(RESOURCE.map);
  resource_s df;

  df.status = RESOURCE_STATUS_NOT_FOUND;
  df.type = RESOURCE_TYPE_INVALID;

  SM_SH_DEFAULT(RESOURCE.map, df);

  /* reads the folder and loads all the files */
  /* into the resource manager */
  sm__resource_dir_read(RESOURCE.root_folder);

  dmon_init();
  dmon_watch(RESOURCE.root_folder, sm__watch_cb, DMON_WATCHFLAGS_RECURSIVE, NULL);

  IS_RESOURCE_INIT = true;

  return true;
}

void resource_teardown(void) {

  SM_ASSERT(IS_RESOURCE_INIT && "resource manager not initialized");

  /* unload all the resources */
  SM_SH_DTOR(RESOURCE.map);
  free((void *)RESOURCE.root_folder); /* TODO: create sm funtion for string duplication and free */

  dmon_deinit();
}

resource_s *resource_get(const char *key) {

  SM_ASSERT(key);

  char path[260];
  snprintf(path, sizeof(path), "%s%s", RESOURCE.root_folder, key);

  resource_s *res = &SM_SH_GET(RESOURCE.map, path);
  if (res->status == RESOURCE_STATUS_NOT_FOUND && res->type == RESOURCE_TYPE_INVALID) {
    SM_LOG_WARN("[%s] resource not found (NOT_FOUND)", path);
    return NULL;
  }

  return res;
}

resource_iter_s resource_iter_new(resource_type_e type, resource_status_e status) {

  resource_iter_s iter;
  iter.type = type;
  iter.status = status;
  iter.index = 0;

  return iter;
}

const char *resource_iter_next(resource_iter_s *iter) {

  SM_ASSERT(iter);

  while (iter->index < SM_SH_LENGTH(RESOURCE.map)) {

    resource_m res = RESOURCE.map[iter->index++];

    if (SM_MASK_CHK(res.value.type, iter->type) && SM_MASK_CHK(res.value.status, iter->status)) {
      return res.key;
    }
  }

  return NULL;
}

#undef SM_MODULE_NAME
