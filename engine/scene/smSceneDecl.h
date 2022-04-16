#ifndef SM_SCENE_DECL_H
#define SM_SCENE_DECL_H

#include "smpch.h"

#include "scene/smComponents.h"

typedef struct {

  uint32_t handle;
  uint32_t archetype_index;

} sm_entity_s;

typedef void (*sm__system)(component_desc_s *desc, void *data, float dt);

#endif /* SM_SCENE_DECL_H */
