#include "smpch.h"

#include "core/smCore.h"

#include "resource/file_format/smSMR.h"
#include "scene/smComponents.h"
#include "scene/smEntity.h"
#include "scene/smScene.h"
#include "scene/smSceneGraph.h"
#include "scene/smSystem.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "SCENE"

#define BIT (sizeof(u64) * 8)

sm_transform_s sm_scene_get_local_transform(sm_scene_s *scene, u32 index);
sm_string sm_scene_entity_to_string(sm_component_t components);

sm_scene_s *sm_scene_new(void) {
  sm_scene_s *scene = SM_CALLOC(1, sizeof(sm_scene_s));
  SM_ASSERT(scene);

  return scene;
}

b8 sm_scene_ctor(sm_scene_s *scene, sm_component_t comp) {

  SM_ASSERT(scene);

  memcpy((void *)&scene->registered_components, &comp, sizeof(comp));

  component_init();

  scene->map_archetype = sm_hashmap_new_u64();
  if (!sm_hashmap_ctor_u64(scene->map_archetype, 16, NULL, NULL)) {
    SM_LOG_ERROR("Failed to create hashmap");
    return false;
  }

  scene->scene_graph = sm_scene_graph_new();
  if (!sm_scene_graph_ctor(scene->scene_graph, 1024)) {
    SM_LOG_ERROR("Failed to create scene graph");
    return false;
  }
  sm_scene_graph_set_parent(scene->scene_graph, ROOT, NO_PARENT);

  return true;
}

b8 sm__scene_dtor_cb(sm_component_t key, void *value, void *user_data) {

  SM_UNUSED(key);
  SM_UNUSED(user_data);

  sm_chunk_s *chunk = value;
  SM_ASSERT(chunk);

  for (u32 i = 0; i < chunk->length; ++i) {
    for (u32 j = 0; j < SM_ARRAY_LEN(chunk->view); ++j) {

      sm_component_view_s *v = &chunk->view[j];

      if (v->dtor)
        v->dtor((u8 *)chunk->data + (i * chunk->size) + v->offset);
    }
  }

  sm_handle_pool_dtor(chunk->pool);
  chunk->alligned ? SM_ALIGNED_FREE(chunk->data) : SM_FREE(chunk->data);
  SM_ARRAY_DTOR(chunk->view);
  SM_FREE(chunk);

  return true;
}

void sm_scene_dtor(sm_scene_s *scene) {

  SM_ASSERT(scene);

  sm_scene_graph_dtor(scene->scene_graph);
  if (scene->systems)
    SM_ARRAY_DTOR(scene->systems);

  sm_hashmap_for_each_u64(scene->map_archetype, sm__scene_dtor_cb, NULL);
  sm_hashmap_dtor_u64(scene->map_archetype);

  component_teardown();

  SM_FREE(scene);
}

sm_entity_s sm_scene_new_entity(sm_scene_s *scene, sm_component_t archetype) {

  SM_ASSERT(SM_MASK_CHK_EQ(scene->registered_components, archetype));

  sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, archetype);

  if (chunk == NULL) {
    sm_chunk_s *new_chunk = SM_CALLOC(1, sizeof(sm_chunk_s));

    new_chunk->pool = sm_handle_pool_new();
    if (!sm_handle_pool_ctor(new_chunk->pool, 16384)) {
      SM_LOG_WARN("failed to create pool for archetype %lu", archetype);
      return (sm_entity_s){0};
    }

    for (u32 i = 0; i < BIT; i++) {
      sm_component_t c = archetype & (sm_component_t)(1ULL << i);
      if (c) {
        sm_component_view_s desc = *component_get_desc(c);
        new_chunk->alligned |= desc.alligned;
        desc.offset += new_chunk->size;
        SM_ARRAY_PUSH(new_chunk->view, desc);
        new_chunk->size += desc.size;
      }
    }

    chunk = new_chunk;
    sm_hashmap_put_u64(scene->map_archetype, archetype, new_chunk);
  }

  sm_handle_t handle = sm_handle_new(chunk->pool);
  SM_ASSERT(handle != SM_INVALID_HANDLE);

  chunk->length++;

  void *data = NULL;
  if (chunk->alligned) {
    SM_LOG_TRACE("alligned chunk");
    data = SM_ALIGNED_ALLOC(16, chunk->size * chunk->length);
    memset(data, 0x0, chunk->size * chunk->length);
    SM_ASSERT(data);
    if (chunk->data) {
      memcpy(data, chunk->data, chunk->size * (chunk->length - 1));
      SM_ALIGNED_FREE(chunk->data);
    }

  } else {
    SM_LOG_TRACE("unaligned chunk");
    data = SM_REALLOC(chunk->data, (chunk->size * chunk->length));
    memset((u8 *)data + chunk->size * (chunk->length - 1), 0x0, chunk->size);
    SM_ASSERT(data);
  }
  chunk->data = data;

  sm_entity_s entity = {.handle = handle, .archetype_index = archetype};

  return entity;
}

sm_entity_s sm_scene_get_entity(sm_scene_s *scene, u32 idx) {
  return sm_scene_graph_get_entity(scene->scene_graph, idx);
}

void sm_scene_set_component_data(sm_scene_s *scene, sm_entity_s entity, sm_component_t components, void *data) {

  SM_ASSERT(scene);

  sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_WARN("no chunk for archetype %lu", entity.archetype_index);
    return;
  }
  SM_ASSERT(chunk->pool);

  u32 index = sm_handle_index(entity.handle);

  for (u32 i = 0; i < SM_ARRAY_LEN(chunk->view); ++i) {
    sm_component_view_s *v = &chunk->view[i];
    if (SM_MASK_CHK(components, v->id)) {
      memcpy((u8 *)chunk->data + (index * chunk->size) + v->offset, data, v->size);
    }
  }
}

void sm_scene_set_all_component_data(sm_scene_s *scene, sm_entity_s entity, void *data) {

  SM_ASSERT(scene);

  sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_WARN("no chunk for archetype %lu", entity.archetype_index);
    return;
  }
  SM_ASSERT(chunk->pool);

  u32 index = sm_handle_index(entity.handle);

  memcpy((u8 *)chunk->data + (index * chunk->size), data, chunk->size);
}

const void *sm_scene_get_component(sm_scene_s *scene, sm_entity_s entity) {

  SM_ASSERT(scene);

  sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_INFO("no chunk for archetype %lu", entity.archetype_index);
    return NULL;
  }

  SM_ASSERT(chunk->pool);
  SM_ASSERT(sm_handle_valid(chunk->pool, entity.handle));

  u32 index = sm_handle_index(entity.handle);
  SM_ASSERT(index < chunk->length);

  return (u8 *)chunk->data + (index * chunk->size);
}

void sm_scene_register_system(sm_scene_s *scene, sm_component_t comp, system_f system, u32 flags) {

  SM_ASSERT(scene);
  SM_ASSERT(SM_MASK_CHK_EQ(scene->registered_components, comp) && "component not registered");

  sm_system_s sys;
  sys.components = comp;
  sys.system = system;
  sys.flags = flags;

  SM_ARRAY_PUSH(scene->systems, sys);
}

struct sm__user_data {
  sm_system_s *sys;
  f32 dt;
};

sm_string sm_scene_entity_to_string(sm_component_t components) {

  char buf[512];
  buf[0] = '\0';
  b8 first = true;

  for (u32 i = 0; i < BIT; i++) {
    sm_component_t c = components & (sm_component_t)(1ULL << i);
    if (c) {
      sm_component_view_s desc = *component_get_desc(c);
      if (first) {
        first = false;
      } else {
        strcat(buf, "|");
      }
      strcat(buf, desc.name.str);
    }
  }

  sm_string str = sm_string_from(buf);
  sm_string str_upper = sm_string_to_upper(str);
  sm_string_dtor(str);

  return str_upper;
}

b8 sm__scene_inclusive_cb(sm_component_t key, void *value, void *user_data) {

  sm_chunk_s *chunk = (sm_chunk_s *)value;
  struct sm__user_data *u_data = (struct sm__user_data *)user_data;

  if (!chunk->pool) {
    SM_LOG_WARN("no pool for archetype %zu", key);
    return false;
  }

  sm_system_iterator_s iter = {0};
  iter.size = chunk->size;
  size_t offset = 0;
  iter.length = chunk->length;
  iter.data = chunk->data;
  iter.index = 0;
  if (SM_MASK_CHK_EQ(key, u_data->sys->components)) {

    for (u32 k = 0; k < BIT; ++k) {

      sm_component_t cmp = key & (sm_component_t)(1ULL << k);
      if (cmp) {
        sm_component_view_s desc = *component_get_desc(cmp);

        if (SM_MASK_CHK(u_data->sys->components, cmp)) {
          desc.offset = offset;
          SM_ARRAY_PUSH(iter.desc, desc);
        }

        offset += desc.size;
      }
    }

    u_data->sys->system(&iter, u_data->dt);
    SM_ARRAY_DTOR(iter.desc);
  }

  return true;
}

void sm_scene_do(sm_scene_s *scene, f32 dt) {

  SM_ASSERT(scene);

  for (size_t i = 0; i < SM_ARRAY_LEN(scene->systems); ++i) {

    sm_system_s *sys = &scene->systems[i];

    if (SM_MASK_CHK(sys->flags, SM_SYSTEM_EXCLUSIVE_FLAG)) {

      sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, sys->components);

      if (chunk == NULL) {
        /* SM_LOG_WARN("no chunk found for system %lu", sys->components); */
        continue;
      }

      sm_system_iterator_s iter;
      iter.length = chunk->length;
      iter.size = chunk->size;
      iter.data = chunk->data;
      iter.desc = chunk->view;
      iter.index = 0;

      sys->system(&iter, dt);

    } else {
      sm_hashmap_for_each_u64(scene->map_archetype, sm__scene_inclusive_cb, &(struct sm__user_data){sys, dt});
    }
  }
}

struct sm__save_data {
  sm_scene_s *scene;
  sm_file_handle_s *file;
};

void sm_scene_graph_save_cb(sm_graph_s *graph, u32 index, void *user_data) {

  struct sm__save_data *data = (struct sm__save_data *)user_data;
  sm_entity_s entity = sm_scene_graph_get_entity(data->scene->scene_graph, index);

  sm_chunk_s *chunk = sm_hashmap_get_u64(data->scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_WARN("no chunk for archetype %lu", entity.archetype_index);
    return;
  }

  SM_ASSERT(chunk->pool);

  u32 handle_idx = sm_handle_index(entity.handle);
  SM_ASSERT(handle_idx < chunk->length);
  sm_string str = sm_scene_entity_to_string(entity.archetype_index);
  SM_LOG_INFO("saving %s entity %u aka index %u, archetype %lu(%s)", sm_scene_graph_get_name(graph, index).str,
              entity.handle, handle_idx, entity.archetype_index, str.str);
  sm_string_dtor(str);

  /*
   * +------------+
   * | header     |
   * +------------+
   * | index      | <-- node index
   * | parent     | <-- parent node index
   * | archetype  | <-- archetype index
   * | name len   | <-- strlen
   * | name       | <-- name
   * | length     | <-- length of data
   * | size       | <-- size of data in bytes
   * | data       | <-- the data itself
   * +------------+
   * | index      | <-- again
   * | parent     |
   * |...         |
   * +------------+
   */

  if (!sm_filesystem_write_bytes(data->file, &index, sizeof(index))) {
    SM_LOG_ERROR("failed to write index");
    return;
  }

  u32 parent = sm_scene_graph_get_parent(graph, index);
  if (!sm_filesystem_write_bytes(data->file, &parent, sizeof(u32))) {
    SM_LOG_ERROR("failed to write parent");
    return;
  }

  if (!sm_filesystem_write_bytes(data->file, &entity.archetype_index, sizeof(sm_component_t))) {
    SM_LOG_ERROR("failed to write archetype index");
    return;
  }

  sm_string name = sm_scene_graph_get_name(graph, index);
  size_t name_len = sm_string_len(name);
  if (!sm_filesystem_write_bytes(data->file, &name_len, sizeof(size_t))) {
    SM_LOG_ERROR("failed to write name len");
    return;
  }

  if (!sm_filesystem_write_bytes(data->file, name.str, name_len)) {
    SM_LOG_ERROR("failed to write name");
    return;
  }

  for (u32 i = 0; i < SM_ARRAY_LEN(chunk->view); ++i) {
    sm_component_view_s *v = &chunk->view[i];
    void *chdata = (u8 *)chunk->data + (handle_idx * chunk->size) + v->offset;
    if (v->write) {
      v->write(data->file, chdata);
    }
  }
}

sm_transform_s sm_scene_get_global_transform(sm_scene_s *scene, u32 index) {
  SM_ASSERT(scene);

  sm_transform_s result = sm_scene_get_local_transform(scene, index);

  for (u32 p = sm_scene_graph_get_parent(scene->scene_graph, index); p > 0;
       p = sm_scene_graph_get_parent(scene->scene_graph, p)) {

    sm_entity_s entt = sm_scene_graph_get_entity(scene->scene_graph, p);

    sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entt.archetype_index);
    if (chunk == NULL) {
      SM_LOG_WARN("no chunk for archetype %lu", entt.archetype_index);
      return sm_transform_identity();
    }
    SM_ASSERT(chunk->pool);

    u32 idx = sm_handle_index(entt.handle);

    for (u32 i = 0; i < SM_ARRAY_LEN(chunk->view); ++i) {
      sm_component_view_s *v = &chunk->view[i];
      if (v->id == SM_TRANSFORM_COMP) {
        sm_transform_s parent_transform = *(sm_transform_s *)((u8 *)chunk->data + (idx * chunk->size) + v->offset);
        result = transform_combine(parent_transform, result);
      }
    }
  }

  return result;
}

sm_transform_s sm_scene_get_local_transform(sm_scene_s *scene, u32 index) {
  SM_ASSERT(scene);

  sm_entity_s entt = sm_scene_graph_get_entity(scene->scene_graph, index);

  sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entt.archetype_index);
  if (chunk == NULL) {
    SM_LOG_WARN("no chunk for archetype %lu", entt.archetype_index);
    return sm_transform_identity();
  }
  SM_ASSERT(chunk->pool);

  u32 idx = sm_handle_index(entt.handle);

  sm_transform_s result = sm_transform_identity();
  for (u32 i = 0; i < SM_ARRAY_LEN(chunk->view); ++i) {
    sm_component_view_s *v = &chunk->view[i];
    if (v->id == SM_TRANSFORM_COMP) {
      result = *(sm_transform_s *)((u8 *)chunk->data + (idx * chunk->size) + v->offset);
      break;
    }
  }

  return result;
}

b8 sm_scene_save(sm_scene_s *scene, sm_string path) {

  SM_ASSERT(scene);

  sm_file_handle_s fhandle;
  smr_file_header_s header = smr_file_header_new(SMR_RESOURCE_SCENE);

  if (!sm_filesystem_open(path, SM_FILE_MODE_WRITE, true, &fhandle)) {
    SM_LOG_ERROR("[%s] failed to open file", path.str);
    return false;
  }

  if (!smr_write_header(&header, &fhandle)) {
    SM_LOG_ERROR("[%s] failed to write header", path.str);
    return false;
  }

  struct sm__save_data data = {scene, &fhandle};
  sm_scene_graph_for_each(scene->scene_graph, 0, sm_scene_graph_save_cb, &data);

  sm_filesystem_close(&fhandle);

  return true;
}

sm_scene_s *sm_scene_open(sm_string path) {

  sm_scene_s *scene = sm_scene_new();
  if (!sm_scene_ctor(scene, SM_ALL_COMP)) {
    SM_LOG_ERROR("failed to create scene");
    return NULL;
  }

  sm_file_handle_s fhandle;
  if (!sm_filesystem_open(path, SM_FILE_MODE_READ, true, &fhandle)) {
    SM_LOG_ERROR("[%s] failed to open file", path.str);
    return NULL;
  }

  smr_file_header_s header;
  if (!smr_read_header(&header, &fhandle)) {
    SM_LOG_ERROR("[%s] failed to read header", path.str);
    return NULL;
  }

  while (true) {

    u32 index = 0;
    if (!sm_filesystem_read_bytes(&fhandle, &index, sizeof(index))) {
      if (feof((FILE *)fhandle.handle)) {
        break;
      }
      SM_LOG_ERROR("failed to read index");
      return NULL;
    }

    u32 parent;
    if (!sm_filesystem_read_bytes(&fhandle, &parent, sizeof(u32))) {
      SM_LOG_ERROR("failed to read parent");
      return NULL;
    }

    sm_component_t archetype = 0;
    if (!sm_filesystem_read_bytes(&fhandle, &archetype, sizeof(archetype))) {
      SM_LOG_ERROR("failed to read archetype index");
      return NULL;
    }

    size_t name_len = 0;
    if (!sm_filesystem_read_bytes(&fhandle, &name_len, sizeof(size_t))) {
      SM_LOG_ERROR("failed to read name len");
      return NULL;
    }

    sm_string name;
    char buf[name_len];
    if (!sm_filesystem_read_bytes(&fhandle, buf, name_len)) {
      SM_LOG_ERROR(" failed to read the string");
      return NULL;
    }
    buf[name_len] = '\0';
    name = sm_string_from(buf);

    sm_entity_s entity = sm_scene_new_entity(scene, archetype);
    if (entity.handle == SM_INVALID_HANDLE) {
      SM_LOG_ERROR("failed to create entity");
      return NULL;
    }

    sm_chunk_s *chunk = sm_hashmap_get_u64(scene->map_archetype, entity.archetype_index);
    if (chunk == NULL) {
      SM_LOG_WARN("no chunk for archetype %lu", entity.archetype_index);
      return NULL;
    }

    u32 handle_idx = sm_handle_index(entity.handle);

    for (u32 i = 0; i < SM_ARRAY_LEN(chunk->view); ++i) {
      sm_component_view_s *v = &chunk->view[i];
      if (v->read) {
        memset((u8 *)chunk->data + (handle_idx * chunk->size) + v->offset, 0x0, v->size);
        v->read(&fhandle, (u8 *)chunk->data + (handle_idx * chunk->size) + v->offset);
      }
    }

    if (parent != NO_PARENT)
      sm_scene_graph_add_child(scene->scene_graph, parent, index);
    else
      sm_scene_graph_set_parent(scene->scene_graph, index, -2);
    sm_scene_graph_set_name(scene->scene_graph, index, name);
    sm_scene_graph_set_entity(scene->scene_graph, index, entity);
    name.str = sm_string_dtor(name);
  }

  sm_filesystem_close(&fhandle);

  return scene;
}

#undef SM_MODULE_NAME
