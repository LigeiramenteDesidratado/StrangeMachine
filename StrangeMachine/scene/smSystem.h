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

  u32 index;
  SM_ARRAY(sm_component_view_s) desc;

  u8 iter_data_count;
  sm_system_data_desc_s iter_data[32];

  void *data;

} sm_system_iterator_s;

typedef void (*system_f)(sm_system_iterator_s *iter, f32 dt);
b8 system_iter_next(sm_system_iterator_s *iter);

typedef u32 sm_system_flags_t;

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
