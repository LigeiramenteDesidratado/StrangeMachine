#ifndef SM_SCENE_GRAPH_H
#define SM_SCENE_GRAPH_H

#include "smpch.h"

#include "core/smCore.h"
#include "scene/smEntity.h"

typedef struct sm__graph_s sm_graph_s;

#define INVALID_NODE 0xFFFFFFFFu
#define NO_PARENT    0xFFFFFFFEu
#define ROOT         0u

sm_graph_s *sm_scene_graph_new(void);
b8 sm_scene_graph_ctor(sm_graph_s *graph, size_t initial_capacity);
void sm_scene_graph_dtor(sm_graph_s *graph);

void sm_scene_graph_set_parent(sm_graph_s *graph, u32 index, u32 parent);
u32 sm_scene_graph_get_parent(sm_graph_s *graph, u32 index);
void sm_scene_graph_set_name(sm_graph_s *graph, u32 index, sm_string name);
b8 sm_scene_graph_has_parent(sm_graph_s *graph, u32 index);
void sm_scene_graph_remove_child(sm_graph_s *graph, u32 index, u32 child);
u32 sm_scene_graph_add_child(sm_graph_s *graph, u32 index, u32 child);
u32 sm_scene_graph_new_node(sm_graph_s *graph);
b8 sm_scene_graph_set_entity(sm_graph_s *graph, u32 index, sm_entity_s entity);
u32 sm_scene_graph_get_by_name(sm_graph_s *graph, sm_string name);
u32 sm_scene_graph_get_by_entity(sm_graph_s *graph, sm_entity_s entity);
u32 sm_scene_graph_get_root(sm_graph_s *graph);
sm_entity_s sm_scene_graph_get_entity(sm_graph_s *graph, u32 index);
void sm_scene_graph_for_each(sm_graph_s *graph, u32 index,
                              void (*callback)(sm_graph_s *graph, u32 index, void *user_data), void *user_data);
sm_string sm_scene_graph_get_name(sm_graph_s *graph, u32 index);

#endif /* SM_SCENE_GRAPH_H */
