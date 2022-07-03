#include "smpch.h"

#include "core/smCore.h"

#include "scene/smComponents.h"
#include "scene/smEntity.h"
#include "scene/smSceneGraph.h"

typedef struct sm__node_s {

  u32 parent;
  sm_string name;
  sm_entity_s entity;
  SM_ARRAY(u32) children;

} sm_node_s;

static inline sm_node_s sm_node_new_invalid(void) {

  sm_node_s node = {
      .parent = INVALID_NODE,
      .name = sm_string_from("INVALID"),
      .entity = (sm_entity_s){0},
      .children = NULL,
  };

  return node;
}

typedef struct sm__graph_s {

  SM_ARRAY(sm_node_s) nodes;

} sm_graph_s;

sm_graph_s *sm_scene_graph_new(void) {

  sm_graph_s *graph = SM_CALLOC(1, sizeof(sm_graph_s));
  SM_ASSERT(graph);

  return graph;
}

b8 sm_scene_graph_ctor(sm_graph_s *graph, size_t initial_capacity) {

  SM_ASSERT(graph);

  SM_ARRAY_SET_LEN(graph->nodes, initial_capacity);
  for (size_t i = 0; i < initial_capacity; i++) {
    graph->nodes[i] = sm_node_new_invalid();
  }

  /* set the first node as the root node */
  graph->nodes[0].parent = NO_PARENT;
  graph->nodes[0].name.str = sm_string_dtor(graph->nodes[0].name);
  graph->nodes[0].name = sm_string_from("ROOT");

  return true;
}

void sm_scene_graph_dtor(sm_graph_s *graph) {

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes); i++) {
    if (graph->nodes[i].name.str)
      graph->nodes[i].name.str = sm_string_dtor(graph->nodes[i].name);
    SM_ARRAY_DTOR(graph->nodes[i].children);
  }
  SM_ARRAY_DTOR(graph->nodes);
  SM_FREE(graph);
}

void sm_scene_graph_set_parent(sm_graph_s *graph, u32 index, u32 parent) {
  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  graph->nodes[index].parent = parent;
}

u32 sm_scene_graph_get_parent(sm_graph_s *graph, u32 index) {
  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  return graph->nodes[index].parent;
}

void sm_scene_graph_set_name(sm_graph_s *graph, u32 index, sm_string name) {
  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  if (graph->nodes[index].name.str) {
    sm_string_dtor(graph->nodes[index].name);
  }

  graph->nodes[index].name = sm_string_reference(name);
}

sm_string sm_scene_graph_get_name(sm_graph_s *graph, u32 index) {
  SM_ASSERT(graph);

  if (index >= SM_ARRAY_LEN(graph->nodes)) {
    SM_LOG_ERROR("index %d out of bounds", index);
    return (sm_string){0};
  }

  return graph->nodes[index].name;
}

b8 sm_scene_graph_has_parent(sm_graph_s *graph, u32 index) {
  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  return graph->nodes[index].parent != INVALID_NODE;
}

void sm_scene_graph_remove_child(sm_graph_s *graph, u32 index, u32 child) {
  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes[index].children); i++) {
    if (graph->nodes[index].children[i] == child) {
      SM_ARRAY_DEL(graph->nodes[index].children, i, 1);
      return;
    }
  }
}

u32 sm_scene_graph_add_child(sm_graph_s *graph, u32 index, u32 child) {

  SM_ASSERT(graph);
  SM_LOG_TRACE("index %u, child %d, len %lu", index, child, SM_ARRAY_LEN(graph->nodes));
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  if (child == ROOT) {
    SM_LOG_ERROR("cannot add root as child");
    return INVALID_NODE;
  }

  if (child == index) {
    SM_LOG_ERROR("cannot add self as child");
    return INVALID_NODE;
  }

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes[child].children); ++i) {
    if (graph->nodes[child].children[i] == index) {
      SM_LOG_ERROR("cyclic dependency of %d and %d", index, child);
      return INVALID_NODE;
    }
  }

  u32 parent = graph->nodes[index].parent;
  while (parent != ROOT && parent != NO_PARENT && parent != INVALID_NODE) {
    if (child == parent) {
      SM_LOG_ERROR("cyclic dependency of %d and %d", index, child);
      return INVALID_NODE;
    }
    parent = graph->nodes[parent].parent;
  }

  if (graph->nodes[index].parent == INVALID_NODE) {
    SM_LOG_WARN("node %d has no parent", index);
    sm_scene_graph_add_child(graph, ROOT, index);
  }

  if (sm_scene_graph_has_parent(graph, child)) {
    sm_scene_graph_remove_child(graph, graph->nodes[child].parent, child);
  }

  SM_ARRAY_PUSH(graph->nodes[index].children, child);
  sm_scene_graph_set_parent(graph, child, index);

  return child;
}

u32 sm_scene_graph_new_node(sm_graph_s *graph) {
  SM_ASSERT(graph);

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes); i++) {
    if (graph->nodes[i].parent == INVALID_NODE) {
      return i;
    }
  }

  /* if we get here, there are no invalid nodes */
  sm_node_s n = sm_node_new_invalid();
  SM_ARRAY_PUSH(graph->nodes, n);
  return SM_ARRAY_LEN(graph->nodes) - 1;
}

b8 sm_scene_graph_set_entity(sm_graph_s *graph, u32 index, sm_entity_s entity) {
  SM_ASSERT(graph);

  if (index >= SM_ARRAY_LEN(graph->nodes)) {
    SM_LOG_ERROR("index %d out of bounds", index);
    return false;
  }

  if (graph->nodes[index].parent == INVALID_NODE) {
    SM_LOG_ERROR("node %d has no parent", index);
    return false;
  }

  graph->nodes[index].entity = entity;

  return true;
}

u32 sm_scene_graph_get_by_name(sm_graph_s *graph, sm_string name) {
  SM_ASSERT(graph);

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes); i++) {
    if (graph->nodes[i].name.str && sm_string_eq(graph->nodes[i].name, name)) {
      return i;
    }
  }

  return INVALID_NODE;
}

u32 sm_scene_graph_get_by_entity(sm_graph_s *graph, sm_entity_s entity) {
  SM_ASSERT(graph);

  for (size_t i = 0; i < SM_ARRAY_LEN(graph->nodes); i++) {
    sm_entity_s e = graph->nodes[i].entity;
    if (e.handle == entity.handle && e.archetype_index == entity.archetype_index) {
      return i;
    }
  }

  return INVALID_NODE;
}

u32 sm_scene_graph_get_root(sm_graph_s *graph) {

  SM_ASSERT(graph);

  return ROOT;
}

sm_entity_s sm_scene_graph_get_entity(sm_graph_s *graph, u32 index) {
  SM_ASSERT(graph);

  if (index >= SM_ARRAY_LEN(graph->nodes)) {
    SM_LOG_ERROR("index %d out of bounds", index);
    return (sm_entity_s){0};
  }

  return graph->nodes[index].entity;
}

void sm_scene_graph_for_each(sm_graph_s *graph, u32 index,
                              void (*callback)(sm_graph_s *graph, u32 index, void *user_data), void *user_data) {

  SM_ASSERT(graph);
  SM_ASSERT(index < SM_ARRAY_LEN(graph->nodes));

  /* static int depth = -1; */
  /* depth++; */

  /* for (int i = 0; i < depth; i++) { */
  /*   printf("| "); */
  /* } */
  callback(graph, index, user_data);

  for (u32 i = 0; i < SM_ARRAY_LEN(graph->nodes[index].children); i++) {
    sm_scene_graph_for_each(graph, graph->nodes[index].children[i], callback, user_data);
  }
  /* depth--; */
}
