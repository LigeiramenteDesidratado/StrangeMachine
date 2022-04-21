#ifndef SM_SCENE_DECL_H
#define SM_SCENE_DECL_H

#include "smpch.h"

#include "scene/smComponents.h"

/* matches all archetypes that contains the components designed by the system */
#define SM_SYSTEM_INCLUSIVE_FLAG (0x00000001 << 0)
/* matches only the archetypes that contains the components designed by the system */
#define SM_SYSTEM_EXCLUSIVE_FLAG (0x00000001 << 1)

typedef struct {
  void *data;
  size_t size;

} system_data_desc_s;

typedef struct {

  size_t length; /* length of the array */
  size_t size;   /* size of each element */

  uint32_t index;
  component_desc_s *desc;

  uint8_t iter_data_count;
  system_data_desc_s iter_data[32];

  void *data;

} system_iterator_s;

typedef void (*system_f)(system_iterator_s *iter, float dt);
bool system_iter_next(system_iterator_s *iter);

typedef struct {

  uint32_t handle;
  uint64_t archetype_index;

} sm_entity_s;

#endif /* SM_SCENE_DECL_H */
