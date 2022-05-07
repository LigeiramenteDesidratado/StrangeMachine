#ifndef SM_SCENE_H
#define SM_SCENE_H

#include "smpch.h"

#include "scene/smEntity.h"
#include "scene/smSystem.h"

struct sm_scene_s;

struct sm_scene_s *scene_new(void);

bool scene_ctor(struct sm_scene_s *scene, sm_component_t comp);
void scene_dtor(struct sm_scene_s *scene);

sm_entity_s scene_new_entity(struct sm_scene_s *scene, sm_component_t archetype);
void scene_set_component(struct sm_scene_s *scene, sm_entity_s entity, void *data);
const void *scene_get_component(struct sm_scene_s *scene, sm_entity_s entity);

void scene_register_system(struct sm_scene_s *scene, sm_component_t comp, system_f system, uint32_t flags);

void scene_do(struct sm_scene_s *scene, float dt);

#endif /* SM_SCENE_H */
