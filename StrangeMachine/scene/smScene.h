#ifndef SM_SCENE_H
#define SM_SCENE_H

#include "smpch.h"

#include "scene/smEntity.h"
#include "scene/smSceneGraph.h"
#include "scene/smSystem.h"

/* ECS allocates memory in "chuncks". A chunck always contains entities of a single archetype */
typedef struct sm__chunk_s {

  struct handle_pool_s *pool;

  b8 alligned;
  size_t length; /* number of components in the pool */
  size_t size;   /* total size in bytes  */

  SM_ARRAY(sm_component_view_s) view;

  void *data;

} sm_chunk_s;

typedef struct sm__scene_s {

  /* All registered components in the scene */
  const sm_component_t registered_components;

  sm_hashmap_u64_m *map_archetype;
  SM_ARRAY(sm_system_s) systems;
  sm_graph_s *scene_graph;

} sm_scene_s;

sm_scene_s *sm_scene_new(void);

b8 sm_scene_ctor(sm_scene_s *scene, sm_component_t comp);
void sm_scene_dtor(sm_scene_s *scene);

sm_entity_s sm_scene_new_entity(sm_scene_s *scene, sm_component_t archetype);
/* void sm_scene_set_component_data(sm_scene_s *scene, sm_entity_s entity, void *data); */
void sm_scene_set_component_data(sm_scene_s *scene, sm_entity_s entity, sm_component_t components, void *data);
void sm_scene_set_all_component_data(sm_scene_s *scene, sm_entity_s entity, void *data);
const void *sm_scene_get_component(sm_scene_s *scene, sm_entity_s entity);
sm_entity_s sm_scene_get_entity(sm_scene_s *scene, u32 idx);
b8 sm_scene_save(sm_scene_s *scene, sm_string path);
sm_scene_s *sm_scene_open(sm_string path);
sm_transform_s sm_scene_get_global_transform(sm_scene_s *scene, u32 index);
sm_transform_s sm_scene_get_local_transform(sm_scene_s *scene, u32 index);
sm_string sm_scene_entity_to_string(sm_component_t components);

void sm_scene_register_system(sm_scene_s *scene, sm_component_t comp, system_f system, u32 flags);

void sm_scene_do(sm_scene_s *scene, f32 dt);

#endif /* SM_SCENE_H */
