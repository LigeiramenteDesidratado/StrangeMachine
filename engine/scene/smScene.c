#include "smpch.h"

#include <math.h>

#include "core/smCore.h"

#include "core/data/smArray.h"
#include "core/util/smBitMask.h"

#include "scene/smSceneDecl.h"

/* matches all archetypes that contains the components designed by the system */
#define SM_SYSTEM_INCLUSIVE_FLAG 0x80000000
/* matches only the archetypes that contains the components designed by the system */
#define SM_SYSTEM_EXCLUSIVE_FLAG 0x40000000

typedef struct {

  uint8_t flags;
  component_s components;
  sm__system system;

} system_s;

typedef struct {

  /* uint32_t archetype; */

  struct handle_pool_s *pool;

  size_t count; /* number of components in the pool */
  size_t size;  /* total size in bytes  */

  component_desc_s *desc;

  void *data;

} chunk_s;

typedef struct scene {

  const component_s registered_components;

  /* goup all possible combinations of components */
  chunk_s *set_archetype;

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

  uint32_t size = fabsf(log2f(comp));
  uint32_t sum = 0;
  for (uint32_t i = 0; i < size; i++) {
    sum += size - i;
  }
  sum += size; /* all possible combinations */

  /* this is equivalent to the code above */
  /* for (uint32_t i = comp; i; i >>= 1) { */
  /*   for (uint32_t j = (i >> 1); j; j >>= 1) { */
  /*   } */
  /* } */

  SM_ARRAY_SET_SIZE(scene->set_archetype, sum);
  memset(scene->set_archetype, 0x0, sizeof(chunk_s) * sum);

  return true;
}

void scene_dtor(scene_s *scene) {

  SM_ASSERT(scene);

  if (scene->systems) {
    SM_ARRAY_DTOR(scene->systems);
  }
  for (size_t i = 0; i < SM_ARRAY_SIZE(scene->set_archetype); i++) {
    chunk_s *chunk = &scene->set_archetype[i];

    if (chunk->pool) {
      pool_dtor(chunk->pool);
    }

    if (chunk->data) {
      SM_FREE(chunk->data);
    }
  }

  SM_FREE(scene);
}

sm_entity_s scene_new_entity(scene_s *scene, component_s archetype) {

  SM_ASSERT(scene);
  SM_ASSERT(SM_MASK_CHK(scene->registered_components, archetype) && "archetype not registered");

  uint32_t index = roundf(log2f(archetype));
  chunk_s *chunk = &scene->set_archetype[index];

  if (chunk->pool == NULL) {
    SM_LOG_INFO("creating new pool for archetype %d", archetype);
    chunk->pool = pool_new();

    if (!pool_ctor(chunk->pool, 32)) {
      SM_LOG_ERROR("failed to create pool for archetype %d", index);
      return (sm_entity_s){0};
    }

#define BIT (sizeof(archetype) * 8)

    for (uint32_t i = 0; i < BIT; i++) {
      if (archetype & (1 << i)) {
        const component_desc_s *desc = component_get_desc(archetype & (1 << i));
        chunk->size += desc->size;
        SM_ARRAY_PUSH(chunk->desc, *desc);
      }
    }

  } else {
    SM_LOG_INFO("reusing existing pool for archetype %d", index);
  }
  sm_handle handle = handle_new(chunk->pool);
  if (handle == SM_INVALID_HANDLE) {
    SM_LOG_ERROR("failed to create handle for archetype %d", index);
    return (sm_entity_s){0};
  }

  chunk->count++;

  void *data = NULL;
  data = SM_REALLOC(chunk->data, chunk->size * chunk->count);
  SM_ASSERT(data);
  chunk->data = data;

  return (sm_entity_s){handle, index};
}

void scene_set_component(scene_s *scene, sm_entity_s entity, void *data) {

  SM_ASSERT(scene);

  SM_LOG_INFO("setting component for entity %d:%d", entity.handle, entity.archetype_index);
  chunk_s *chunk = &scene->set_archetype[entity.archetype_index];

  SM_ASSERT(chunk->pool);
  SM_ASSERT(handle_valid(chunk->pool, entity.handle));

  uint32_t index = sm_handle_index(entity.handle);
  SM_ASSERT(index < chunk->count);

  memcpy((uint8_t *)chunk->data + (index * chunk->size), data, chunk->size);
}

void scene_get_component(scene_s *scene, sm_entity_s entity, void *data) {

  SM_ASSERT(scene);
  /* SM_ASSERT(SM_MASK_CHK(scene->registered_components, comp) && "component not registered"); */

  /* uint32_t chkindex = roundf(log2f(comp)); */
  SM_LOG_INFO("getting component for entity %d:%d", entity.handle, entity.archetype_index);
  chunk_s *chunk = &scene->set_archetype[entity.archetype_index];

  SM_ASSERT(chunk->pool);
  SM_ASSERT(handle_valid(chunk->pool, entity.handle));

  uint32_t index = sm_handle_index(entity.handle);
  SM_ASSERT(index < chunk->count);

  memcpy(data, (uint8_t *)chunk->data + (index * chunk->size), chunk->size);
}

typedef struct {

  uint32_t count;

} iterator_s;

void scene_set_system(scene_s *scene, component_s comp, sm__system system) {

  SM_ASSERT(scene);
  SM_ASSERT(SM_MASK_CHK(scene->registered_components, comp) && "component not registered");

  system_s sys;
  sys.components = comp;
  sys.system = system;

  SM_ARRAY_PUSH(scene->systems, sys);
}

void scene_do(scene_s *scene, float dt) {

  SM_ASSERT(scene);

  for (size_t i = 0; i < SM_ARRAY_SIZE(scene->systems); ++i) {
    system_s *sys = &scene->systems[i];

    uint32_t index = roundf(log2f(sys->components));
    chunk_s *chunk = &scene->set_archetype[index];

    SM_ASSERT(chunk->pool);

    for (uint32_t j = 0; j < chunk->count; ++j) {

      void *data = (uint8_t *)chunk->data + (j * chunk->size);
      sys->system(chunk->desc, data, dt);
    }
  }
}
