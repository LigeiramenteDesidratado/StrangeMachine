#include "smpch.h"

#include "core/smCore.h"

#include "core/util/smBitMask.h"

#include "resource/smResource.h"
#include "resource/smShaderResource.h"
#include "resource/smTextureResourcePriv.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "RESOURCE MANAGER"

#define DMON_IMPL

#define DMON_MALLOC  SM_MALLOC
#define DMON_FREE    SM_FREE
#define DMON_REALLOC SM_REALLOC

#define DMON_ASSERT SM_ASSERT

#define DMON_LOG_ERROR   SM_LOG_ERROR
#define DMON_LOG_DEBUG   SM_LOG_TRACE
#define _DMON_LOG_ERRORF SM_LOG_ERROR
#define _DMON_LOG_DEBUGF SM_LOG_TRACE
#include "vendor/dmon/dmon.h"

typedef struct {

  sm_string root_folder;

  sm_hashmap_str_m *map;

} resource_manager_s;

/* globals */

resource_manager_s *RESOURCE_MANAGER = NULL;

char *expected_ext = "jpeg;jpg;png;gltf;glb;obj;mtl;mp3;ogg;shader"; /* supported resource types */

/* private functions */
resource_manager_type_e sm__resource_manager_get_file_type(const char *file);
void sm__resource_manager_on_found(sm_string path);
void sm__resource_manager_on_delete(sm_string path);
void sm__resource_manager_on_reload(sm_string path);
void sm__resource_manager_watch_cb(dmon_watch_id watch_id, dmon_action action, const char *rootdir,
                                   const char *filepath, const char *oldfilepath, void *user);
void sm__resource_manager_dir_read(const char *folder);

bool sm_resource_manager_init(const char *root_folder) {

  SM_ASSERT(RESOURCE_MANAGER == NULL && "resource already initialized");
  SM_ASSERT(root_folder);

  RESOURCE_MANAGER = SM_CALLOC(1, sizeof(resource_manager_s));
  SM_ASSERT(RESOURCE_MANAGER);

  RESOURCE_MANAGER->root_folder = sm_string_from(root_folder);

  RESOURCE_MANAGER->map = sm_hashmap_new_str();
  if (!sm_hashmap_ctor_str(RESOURCE_MANAGER->map, 16, NULL, NULL)) {
    SM_LOG_ERROR("failed to create hashmap");
    return false;
  }

  /* Reads the folder and loads the resources */
  /* into the resource manager */
  sm__resource_manager_dir_read(RESOURCE_MANAGER->root_folder.str);

  dmon_init();
  dmon_watch(RESOURCE_MANAGER->root_folder.str, sm__resource_manager_watch_cb, DMON_WATCHFLAGS_RECURSIVE, NULL);

  sm_texture_resource_init(32);
  sm_shader_resource_init(32);

  return true;
}

b8 sm__reource_manager_dtor_cb(sm_string key, void *value, void *user) {

  sm_resource_s *res = (sm_resource_s *)value;
  SM_FREE(res);
  sm_string_dtor(key);

  return true;
}

void sm_resource_manager_teardown(void) {

  SM_ASSERT(RESOURCE_MANAGER);

  sm_shader_resource_teardown();
  sm_texture_resource_teardown();

  dmon_deinit();

  sm_hashmap_for_each_str(RESOURCE_MANAGER->map, sm__reource_manager_dtor_cb, NULL);
  sm_hashmap_dtor_str(RESOURCE_MANAGER->map);

  sm_string_dtor(RESOURCE_MANAGER->root_folder); /* TODO: create sm funtion for string duplication and free */

  SM_FREE(RESOURCE_MANAGER);
}

sm_resource_s *sm_resource_manager_get(sm_string key) {

  SM_ASSERT(key.str);

  sm_resource_s *res = NULL;
  sm_string res_name;

  if (!sm_string_contains(key, RESOURCE_MANAGER->root_folder)) {
    res_name = sm_string_append(RESOURCE_MANAGER->root_folder, key);
  } else {
    res_name = sm_string_reference(key);
  }

  sm_hashmap_lock_str(RESOURCE_MANAGER->map);
  res = sm_hashmap_get_str(RESOURCE_MANAGER->map, res_name);
  sm_hashmap_unlock_str(RESOURCE_MANAGER->map);

  sm_string_dtor(res_name);

  if (res == NULL) {
    SM_LOG_WARN("[%s] resource not found", res_name.str);
    return NULL;
  }

  return res;
}

sm_resource_manager_iter_s sm_resource_manager_iter_new(resource_manager_type_e type,
                                                        resource_manager_status_e status) {

  sm_resource_manager_iter_s iter;
  iter.type = type;
  iter.status = status;
  iter.index = 0;

  return iter;
}

resource_manager_type_e sm__resource_manager_get_file_type(const char *file) {

  SM_ASSERT(file);

  if (sm_filesystem_has_ext_c_str(file, "jpg;jpeg;png"))
    return RESOURCE_TYPE_TEXTURE;
  if (sm_filesystem_has_ext_c_str(file, "mp3;ogg"))
    return RESOURCE_TYPE_AUDIO;
  if (sm_filesystem_has_ext_c_str(file, "shader"))
    return RESOURCE_TYPE_SHADER;
  if (sm_filesystem_has_ext_c_str(file, "fbx;obj;gltf;glb"))
    return RESOURCE_TYPE_MODEL;

  return RESOURCE_TYPE_INVALID;
}

void sm__resource_manager_on_found(sm_string path) {

  SM_ASSERT(path.str);

  if (!sm_filesystem_exists(path)) {
    SM_LOG_WARN("[%s] resource does not exist", path.str);
    return;
  }

  SM_LOG_TRACE("[%s] resource found", path.str);

  resource_manager_type_e ft = sm__resource_manager_get_file_type(path.str);
  if (ft == RESOURCE_TYPE_INVALID) {
    SM_LOG_WARN("[%s] resource not supported", path.str);
    return;
  }

  sm_resource_s *res = SM_MALLOC(sizeof(sm_resource_s));
  res->type = ft;
  res->status = RESOURCE_STATUS_FOUND;
  res->handle = SM_INVALID_HANDLE;

  sm_resource_s *r = sm_hashmap_put_str(RESOURCE_MANAGER->map, sm_string_reference(path), res);
  if (r) {
    SM_FREE(r);
    SM_LOG_WARN("[%s] resource already exists. Replacing it", path.str);
  }
}

void sm__resource_manager_on_delete(sm_string path) {

  SM_ASSERT(path.str);

  /* TODO: reclaim the key memory */
  /* if (!sm_hashmap_remove_str(RESOURCE->map, path)) */
  /*   SM_LOG_WARN("[%s] resource does not exist", path.str); */
  /* else */
  /*   SM_LOG_TRACE("[%s] resource deleted", path.str); */

  sm_resource_s *res = sm_hashmap_get_str(RESOURCE_MANAGER->map, path);
  if (!res) {
    SM_LOG_WARN("[%s] resource not found in the map (NOT_FOUND)", path.str);
    return;
  }

  SM_LOG_TRACE("[%s] resource removed", path.str);
  res->status = RESOURCE_STATUS_INVALID;
  res->type = RESOURCE_TYPE_INVALID;
}

void sm__resource_manager_on_reload(sm_string path) {

  SM_ASSERT(path.str);

  sm_resource_s *res = sm_hashmap_get_str(RESOURCE_MANAGER->map, path);
  if (!res) {
    SM_LOG_WARN("[%s] resource not found in the map (NOT_FOUND)", path.str);
    return;
  }

  SM_LOG_TRACE("[%s] resource reloaded", path.str);
  res->status |= RESOURCE_STATUS_RELOADED;
}

void sm__resource_manager_watch_cb(dmon_watch_id watch_id, dmon_action action, const char *rootdir,
                                   const char *filepath, const char *oldfilepath, void *user) {

  SM_ASSERT(rootdir);
  SM_ASSERT(filepath);
  SM_ASSERT(action < 5);

  SM_UNUSED(user);
  SM_UNUSED(watch_id);
  SM_UNUSED(oldfilepath);

  /* receive change events. type of event is stored in 'action' variable */

  if (!sm_filesystem_has_ext_c_str(filepath, expected_ext)) {
    SM_LOG_WARN("[%s] resource not supported", filepath);
    return;
  }

  void (*handler[5])(sm_string key) = {

      [DMON_ACTION_CREATE] = sm__resource_manager_on_found,
      [DMON_ACTION_DELETE] = sm__resource_manager_on_delete,
      [DMON_ACTION_MODIFY] = sm__resource_manager_on_reload,
      [DMON_ACTION_MOVE] = sm__resource_manager_on_reload,
  };

  /* char *key = SM_MALLOC(strlen(rootdir) + strlen(filepath) + 1); */
  sm_string key = sm_string_from(rootdir);
  sm_string fpath = sm_string_from(filepath);
  sm_string_append(key, fpath);
  /* strcpy(key, rootdir); */
  /* strcat(key, filepath); */

  sm_hashmap_lock_str(RESOURCE_MANAGER->map);

  handler[action](key);

  sm_hashmap_unlock_str(RESOURCE_MANAGER->map);

  sm_string_dtor(key);
  sm_string_dtor(fpath);
}

void sm__resource_manager_dir_read(const char *folder) {

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
      if (sm_filesystem_has_ext_c_str(ent->d_name, expected_ext)) {

        /* create the full path */
        strcpy(buf, ent->d_name);

        sm_string str_root = sm_string_from(root);
        sm__resource_manager_on_found(str_root);
        sm_string_dtor(str_root);
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
      sm__resource_manager_dir_read(root);
    }
  }

  closedir(dir);
}

#undef SM_MODULE_NAME
