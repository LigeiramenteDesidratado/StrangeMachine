#include "smpch.h"

#include "core/smCore.h"

#include "scene/smComponents.h"
#include "scene/smEntity.h"
#include "scene/smSystem.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "SCENE"

#define BIT (sizeof(uint64_t) * 8)

typedef struct sm__chunk_s {

  struct handle_pool_s *pool;

  bool alligned;
  size_t count; /* number of components in the pool */
  size_t size;  /* total size in bytes  */

  SM_ARRAY(sm_component_desc_s) desc;

  void *data;

} sm_chunk_s;

typedef struct sm__chunk_m {

  sm_component_t key;
  sm_chunk_s *value;

} sm_chunk_m;

typedef struct sm__scene_s {

  /* All registered components in the scene */
  const sm_component_t registered_components;

  sm_chunk_m *map_archetype;
  SM_ARRAY(sm_system_s) systems;

} scene_s;

scene_s *scene_new(void) {
  scene_s *scene = SM_CALLOC(1, sizeof(scene_s));
  SM_ASSERT(scene);

  return scene;
}

bool scene_ctor(scene_s *scene, sm_component_t comp) {

  SM_ASSERT(scene);

  memcpy((void *)&scene->registered_components, &comp, sizeof(comp));

  sm_chunk_s *df = NULL;
  SM_HT_DEFAULT(scene->map_archetype, df);

  return true;
}

void scene_dtor(scene_s *scene) {

  SM_ASSERT(scene);

  if (scene->systems)
    SM_ARRAY_DTOR(scene->systems);

  for (size_t i = 0; i < SM_HT_LENGTH(scene->map_archetype); ++i) {
    pool_dtor(scene->map_archetype[i].value->pool);

    (scene->map_archetype[i].value->alligned) ? SM_ALIGNED_FREE(scene->map_archetype[i].value->data)
                                              : SM_FREE(scene->map_archetype[i].value->data);

    SM_ARRAY_DTOR(scene->map_archetype[i].value->desc);
    free(scene->map_archetype[i].value);
  }

  SM_HT_DTOR(scene->map_archetype);

  SM_FREE(scene);
}

sm_entity_s scene_new_entity(scene_s *scene, sm_component_t archetype) {

  sm_chunk_s *chunk = SM_HT_GET(scene->map_archetype, archetype);

  if (chunk == NULL) {
    sm_chunk_s *new_chunk = calloc(1, sizeof(sm_chunk_s));

    new_chunk->pool = pool_new();

    if (!pool_ctor(new_chunk->pool, 16384)) {
      SM_LOG_WARN("failed to create pool for archetype %lu", archetype);
      return (sm_entity_s){0};
    }
    for (uint32_t i = 0; i < BIT; i++) {
      sm_component_t c = archetype & (sm_component_t)(1ULL << i);
      if (c) {
        sm_component_desc_s desc = *component_get_desc(c);
        new_chunk->alligned |= desc.alligned;
        desc.offset += new_chunk->size;
        SM_ARRAY_PUSH(new_chunk->desc, desc);
        new_chunk->size += desc.size;
      }
    }

    chunk = new_chunk;
    SM_HT_INSERT(scene->map_archetype, archetype, new_chunk);
  }

  sm_handle handle = handle_new(chunk->pool);
  SM_ASSERT(handle != SM_INVALID_HANDLE);

  chunk->count++;

  void *data = NULL;
  if (chunk->alligned) {

    data = SM_ALIGNED_ALLOC(16, chunk->size * chunk->count);
    SM_ASSERT(data);
    if (chunk->data) {
      memcpy(data, chunk->data, chunk->size * (chunk->count - 1));
      SM_ALIGNED_FREE(chunk->data);
    }

  } else {
    data = SM_REALLOC(chunk->data, chunk->size * chunk->count);
    SM_ASSERT(data);
  }
  chunk->data = data;

  return (sm_entity_s){.handle = handle, .archetype_index = archetype};
}

void scene_set_component(scene_s *scene, sm_entity_s entity, void *data) {

  SM_ASSERT(scene);

  sm_chunk_s *chunk = SM_HT_GET(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_WARN("no chunk for archetype %lu", entity.archetype_index);
    return;
  }
  SM_ASSERT(chunk->pool);

  uint32_t index = sm_handle_index(entity.handle);

  memcpy((uint8_t *)chunk->data + (index * chunk->size), data, chunk->size);
}

const void *scene_get_component(scene_s *scene, sm_entity_s entity) {

  SM_ASSERT(scene);

  sm_chunk_s *chunk = SM_HT_GET(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_INFO("no chunk for archetype %lu", entity.archetype_index);
    return NULL;
  }

  SM_ASSERT(chunk->pool);
  SM_ASSERT(handle_valid(chunk->pool, entity.handle));

  uint32_t index = sm_handle_index(entity.handle);
  SM_ASSERT(index < chunk->count);

  return (uint8_t *)chunk->data + (index * chunk->size);

  /* memcpy(data, (uint8_t *)chunk->data + (index * chunk->size), chunk->size); */
}

void scene_register_system(scene_s *scene, sm_component_t comp, system_f system, uint32_t flags) {

  SM_ASSERT(scene);
  SM_ASSERT(SM_MASK_CHK_EQ(scene->registered_components, comp) && "component not registered");

  sm_system_s sys;
  sys.components = comp;
  sys.system = system;
  sys.flags = flags;

  SM_ARRAY_PUSH(scene->systems, sys);
}

void scene_do(scene_s *scene, float dt) {

  SM_ASSERT(scene);

  for (size_t i = 0; i < SM_ARRAY_SIZE(scene->systems); ++i) {

    sm_system_s *sys = &scene->systems[i];

    if (SM_MASK_CHK(sys->flags, SM_SYSTEM_EXCLUSIVE_FLAG)) {

      sm_chunk_s *chunk = SM_HT_GET(scene->map_archetype, sys->components);

      if (chunk == NULL) {
        /* SM_LOG_WARN("no chunk found for system %lu", sys->components); */
        continue;
      }

      sm_system_iterator_s iter;
      iter.length = chunk->count;
      iter.size = chunk->size;
      iter.data = chunk->data;
      iter.desc = chunk->desc;
      iter.index = 0;

      sys->system(&iter, dt);

    } else {

      for (size_t j = 0; j < SM_HT_LENGTH(scene->map_archetype); ++j) {

        sm_component_t key = scene->map_archetype[j].key;
        sm_chunk_s *chunk = scene->map_archetype[j].value;

        if (!chunk->pool) {
          SM_LOG_WARN("no pool for archetype %zu", j);
          continue;
        }

        sm_system_iterator_s iter = {0};
        iter.size = chunk->size;
        size_t offset = 0;
        iter.length = chunk->count;
        iter.data = chunk->data;
        iter.index = 0;
        if (SM_MASK_CHK_EQ(key, sys->components)) {

          for (uint32_t k = 0; k < BIT; ++k) {

            sm_component_t cmp = key & (sm_component_t)(1ULL << k);

            if (cmp) {

              sm_component_desc_s desc = *component_get_desc(cmp);

              if (SM_MASK_CHK(sys->components, cmp)) {

                desc.offset = offset;
                SM_ARRAY_PUSH(iter.desc, desc);
              }
              offset += desc.size;
            }
          }

          sys->system(&iter, dt);
          SM_ARRAY_DTOR(iter.desc);
        }
      }
    }
  }
}

#undef SM_MODULE_NAME
