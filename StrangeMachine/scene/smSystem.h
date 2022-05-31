#ifndef SM_SCENE_SYSTEM_H
#define SM_SCENE_SYSTEM_H

#include "core/smCore.h"

#include "scene/smComponents.h"

typedef struct sm__system_data_desc_s {

  void *data;
  size_t size;

} sm_system_data_desc_s;

typedef struct sm__system_iterator_s {

  size_t length; /* length of the array */
  size_t size;   /* size of each element */

  uint32_t index;
  SM_ARRAY(sm_component_desc_s) desc;

  uint8_t iter_data_count;
  sm_system_data_desc_s iter_data[32];

  void *data;

} sm_system_iterator_s;

typedef void (*system_f)(sm_system_iterator_s *iter, float dt);
bool system_iter_next(sm_system_iterator_s *iter);

typedef uint32_t sm_system_flags_t;

/* matches all archetypes that contains the components designed by the system */
extern const sm_system_flags_t SM_SYSTEM_INCLUSIVE_FLAG;
/* matches only the archetypes that contains the components designed by the system */
extern const sm_system_flags_t SM_SYSTEM_EXCLUSIVE_FLAG;

typedef struct sm__system_s {

  sm_system_flags_t flags;
  /* components that the system is designed to work with */
  sm_component_t components;
  system_f system;

} sm_system_s;

#endif /* SM_SCENE_SYSTEM_H */
