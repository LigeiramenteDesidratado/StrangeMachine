#include "smpch.h"

#include <math.h>

#include "core/smCore.h"

#include "core/data/smArray.h"
#include "core/util/smBitMask.h"

#include "core/data/smHashTable.h"

#include "scene/smSceneDecl.h"

SM_PRIVATE
uint32_t sm__log2_32(uint32_t value);

#define BIT (sizeof(uint64_t) * 8)

typedef struct {

  uint32_t flags;
  /* components that the system is designed to work with */
  component_s components;
  system_f system;

} system_s;

typedef struct {

  struct handle_pool_s *pool;

  size_t count; /* number of components in the pool */
  size_t size;  /* total size in bytes  */

  component_desc_s *desc;

  void *data;

} chunk_s;

typedef struct {

  component_s key;
  chunk_s *value;

} map_chunk_s;

typedef struct scene {

  /* All registered components in the scene */
  const component_s registered_components;

  /* goup all possible combinations of components */
  /* chunk_s *set_archetype; */

  map_chunk_s *map_archetype;
  system_s *systems;

} scene_s;

scene_s *scene_new(void) {
  scene_s *scene = SM_CALLOC(1, sizeof(scene_s));
  SM_ASSERT(scene);

  return scene;
}

bool scene_ctor(scene_s *scene, component_s comp) {

  SM_ASSERT(scene);

  memcpy((void *)&scene->registered_components, &comp, sizeof(comp));

  chunk_s *df = NULL;
  SM_HT_DEFAULT(scene->map_archetype, df);

  return true;
}

void scene_dtor(scene_s *scene) {

  SM_ASSERT(scene);

  if (scene->systems) {
    SM_ARRAY_DTOR(scene->systems);
  }
  for (size_t i = 0; i < SM_HT_LENGTH(scene->map_archetype); ++i) {
    pool_dtor(scene->map_archetype[i].value->pool);
    SM_FREE(scene->map_archetype[i].value->data);
    SM_ARRAY_DTOR(scene->map_archetype[i].value->desc);
    free(scene->map_archetype[i].value);
  }

  SM_HT_DTOR(scene->map_archetype);

  SM_FREE(scene);
}

sm_entity_s scene_new_entity(scene_s *scene, component_s archetype) {

  chunk_s *chunk = SM_HT_GET(scene->map_archetype, archetype);

  if (chunk == NULL) {
    chunk_s *new_chunk = calloc(1, sizeof(chunk_s));

    new_chunk->pool = pool_new();

    if (!pool_ctor(new_chunk->pool, 16384)) {
      printf("failed to create pool for archetype %lu\n", archetype);
      return (sm_entity_s){0};
    }

    for (uint32_t i = 0; i < BIT; i++) {
      component_s c = archetype & (component_s)(1ULL << i);
      if (c) {
        component_desc_s desc = *component_get_desc(c);
        desc.offset += new_chunk->size;
        SM_ARRAY_PUSH(new_chunk->desc, desc);
        new_chunk->size += desc.size;
      }
    }

    chunk = new_chunk;
    SM_HT_INSERT(scene->map_archetype, archetype, new_chunk);
  }

  sm_handle handle = handle_new(chunk->pool);
  assert(handle != SM_INVALID_HANDLE);

  chunk->count++;

  void *data = NULL;
  data = SM_REALLOC(chunk->data, chunk->size * chunk->count);
  assert(data);
  chunk->data = data;

  return (sm_entity_s){.handle = handle, .archetype_index = archetype};
}

void scene_set_component(scene_s *scene, sm_entity_s entity, void *data) {

  assert(scene);

  chunk_s *chunk = SM_HT_GET(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    printf("no chunk for archetype %lu\n", entity.archetype_index);
    return;
  }
  assert(chunk->pool);

  uint32_t index = sm_handle_index(entity.handle);

  memcpy((uint8_t *)chunk->data + (index * chunk->size), data, chunk->size);
}

void scene_get_component(scene_s *scene, sm_entity_s entity, void *data) {

  SM_ASSERT(scene);

  chunk_s *chunk = SM_HT_GET(scene->map_archetype, entity.archetype_index);
  if (chunk == NULL) {
    SM_LOG_INFO("no chunk for archetype %lu", entity.archetype_index);
    return;
  }

  SM_ASSERT(chunk->pool);
  SM_ASSERT(handle_valid(chunk->pool, entity.handle));

  uint32_t index = sm_handle_index(entity.handle);
  SM_ASSERT(index < chunk->count);

  memcpy(data, (uint8_t *)chunk->data + (index * chunk->size), chunk->size);
}

void scene_set_system(scene_s *scene, component_s comp, system_f system, uint32_t flags) {

  assert(scene);
  assert(SM_MASK_CHK_EQ(scene->registered_components, comp) && "component not registered");

  system_s sys;
  sys.components = comp;
  sys.system = system;
  sys.flags = flags;

  SM_ARRAY_PUSH(scene->systems, sys);
}

bool system_iter_next(system_iterator_s *iter) {

  if (iter->index >= iter->length) {
    return false;
  }

  iter->iter_data_count = 0;

  for (uint32_t i = 0; i < SM_ARRAY_SIZE(iter->desc); i++) {
    const component_desc_s *desc = &iter->desc[i];
    iter->iter_data[i].data = ((uint8_t *)iter->data) + (iter->index * iter->size) + desc->offset;
    iter->iter_data[i].size = desc->size;
    iter->iter_data_count++;
  }

  iter->index++;

  return true;
}

void scene_do(scene_s *scene, float dt) {

  SM_ASSERT(scene);

  for (size_t i = 0; i < SM_ARRAY_SIZE(scene->systems); ++i) {

    system_s *sys = &scene->systems[i];

    if (SM_MASK_CHK(sys->flags, SM_SYSTEM_EXCLUSIVE_FLAG)) {

      chunk_s *chunk = SM_HT_GET(scene->map_archetype, sys->components);

      if (chunk == NULL) {
        printf("no chunk found for system %lu\n", sys->components);
        continue;
      }

      system_iterator_s iter;
      iter.length = chunk->count;
      iter.size = chunk->size;
      iter.data = chunk->data;
      iter.desc = chunk->desc;
      iter.index = 0;

      sys->system(&iter, dt);

    } else {

      for (size_t j = 0; j < SM_HT_LENGTH(scene->map_archetype); ++j) {

        component_s key = scene->map_archetype[j].key;
        chunk_s *chunk = scene->map_archetype[j].value;

        if (!chunk->pool) {
          printf("no pool for archetype %zu", j);
          continue;
        }

        system_iterator_s iter = {0};
        iter.size = chunk->size;
        size_t offset = 0;
        iter.length = chunk->count;
        iter.data = chunk->data;
        iter.index = 0;

        if (SM_MASK_CHK_EQ(key, sys->components)) {

          for (uint32_t k = 0; k < BIT; ++k) {

            component_s cmp = key & (component_s)(1ULL << k);

            if (cmp) {

              component_desc_s desc = *component_get_desc(cmp);

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

/* https://stackoverflow.com/a/11398748 */
SM_PRIVATE
const uint32_t sm__tab32[32] = {0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
                                8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31};

SM_PRIVATE
uint32_t sm__log2_32(uint32_t value) {
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return sm__tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}
