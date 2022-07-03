#ifndef SM_SCENE_ENTITY_H
#define SM_SCENE_ENTITY_H

#include "scene/smComponents.h"

typedef struct sm__entity_s {

  u32 handle;
  sm_component_t archetype_index;

} sm_entity_s;

#endif /* SM_SCENE_ENTITY_H */
