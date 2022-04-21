#ifndef SM_SCENE_H
#define SM_SCENE_H

#include "smpch.h"

#include "scene/smSceneDecl.h"

struct scene_s;

struct scene_s *scene_new(void);

bool scene_ctor(struct scene_s *scene, component_s comp);
void scene_dtor(struct scene_s *scene);

sm_entity_s scene_new_entity(struct scene_s *scene, component_s archetype);
void scene_set_component(struct scene_s *scene, sm_entity_s entity, void *data);
void scene_get_component(struct scene_s *scene, sm_entity_s entity, void *data);

void scene_set_system(struct scene_s *scene, component_s comp, system_f system, uint32_t flags);

void scene_do(struct scene_s *scene, float dt);

#endif /* SM_SCENE_H */
