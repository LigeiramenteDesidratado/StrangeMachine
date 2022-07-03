#include "smpch.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include "core/smCore.h"
#include "math/smMath.h"
#include "scene/smScene.h"
#include "scene/smSceneGraph.h"
#include "util/colors.h"

typedef void (*caller)(const struct aiMesh *ai_mesh, sm_scene_s *sm_scene, u32 sm_node, sm_mat4 mat);

void process_node(struct aiNode *node, const struct aiScene *scene, sm_scene_s *sm_scene, u32 sm_node, caller call);
void load_mesh(const struct aiMesh *ai_mesh, sm_scene_s *sm_scene, u32 sm_node, sm_mat4 mat);

sm_scene_s *sc = NULL;
void print_cb(sm_graph_s *graph, u32 index, void *user_data) {

  SM_UNUSED(user_data);

  sm_entity_s e = sm_scene_graph_get_entity(graph, index);
  sm_string s = sm_scene_entity_to_string(e.archetype_index);
  printf("%d=> %s, parent: %d, entity: %d, %lu(%s)\n", index, sm_scene_graph_get_name(graph, index).str,
         sm_scene_graph_get_parent(graph, index), e.handle, e.archetype_index, s.str);
  sm_string_dtor(s);
  if (false) {
    sm_entity_s entity = sm_scene_graph_get_entity(graph, index);

    const void *data = sm_scene_get_component(sc, entity);
    if (data) {
      sm_transform_s *transform = (sm_transform_s *)data;
      sm_transform_print((*transform));

      sm_mesh_s *mesh = (sm_mesh_s *)((char *)data + sizeof(sm_transform_s));
      printf("pos: %lu\n", SM_ARRAY_LEN(mesh->positions));
      printf("norm: %lu\n", SM_ARRAY_LEN(mesh->normals));
      printf("uvs: %lu\n", SM_ARRAY_LEN(mesh->uvs));
      printf("indices: %lu\n", SM_ARRAY_LEN(mesh->indices));
      printf("colors: %lu\n", SM_ARRAY_LEN(mesh->colors));
    }
  }
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Usage: %s <file.{obj,gltf}>\n", argv[0]);
    return 1;
  }

  const struct aiScene *scene = aiImportFile(argv[1], aiProcessPreset_TargetRealtime_Fast);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    SM_LOG_ERROR("ASSIMP: %s\n", aiGetErrorString());
    exit(1);
  }

  sc = sm_scene_new();
  if (!sm_scene_ctor(sc, SM_ALL_COMP)) {
    SM_LOG_ERROR("StrangeMachine: Failed to create scene");
    exit(EXIT_FAILURE);
  }

  sm_string root_name = sm_string_from(scene->mRootNode->mName.data);
  u32 root_node = sm_scene_graph_get_root(sc->scene_graph);

  if (!sm_string_eq_c_str(root_name, "ROOT")) {
    u32 n = sm_scene_graph_new_node(sc->scene_graph);
    if (sm_scene_graph_add_child(sc->scene_graph, root_node, n) == INVALID_NODE) {
      SM_LOG_ERROR("StrangeMachine: Failed to add child");
      exit(EXIT_FAILURE);
    }
    root_node = n;
  }

  sm_scene_graph_set_name(sc->scene_graph, root_node, root_name);
  sm_string_dtor(root_name);

  process_node(scene->mRootNode, scene, sc, root_node, load_mesh);

  sm_string path = sm_string_from("assets/scene/scene.smscene");
  if (!sm_scene_save(sc, path)) {
    SM_LOG_ERROR("StrangeMachine: Failed to save scene");
    exit(EXIT_FAILURE);
  }

  sm_scene_s *dummy = sm_scene_open(path);

  printf("==========================\n");
  sm_scene_graph_for_each(sc->scene_graph, 0, print_cb, NULL);
  printf("==========================\n");
  sm_scene_graph_for_each(dummy->scene_graph, 0, print_cb, NULL);
  printf("==========================\n");

  sm_scene_dtor(dummy);
  aiReleaseImport(scene);
  sm_string_dtor(path);
  sm_scene_dtor(sc);

  return 0;
}

void process_node(struct aiNode *node, const struct aiScene *scene, sm_scene_s *sm_scene, u32 sm_node, caller call) {
  /* process each mesh located at the current node */
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {

    struct aiMesh *ai_mesh = scene->mMeshes[node->mMeshes[i]];
    /* the node object only contains indices to index the actual objects in the scene. */
    /* the scene contains all the data, node is just to keep stuff organized (like relations between nodes). */

    struct aiMatrix4x4 mat = node->mTransformation;
    sm_mat4 m;
    m.data[0][0] = mat.a1;
    m.data[0][1] = mat.b1;
    m.data[0][2] = mat.c1;
    m.data[0][3] = mat.d1;
    m.data[1][0] = mat.a2;
    m.data[1][1] = mat.b2;
    m.data[1][2] = mat.c2;
    m.data[1][3] = mat.d2;
    m.data[2][0] = mat.a3;
    m.data[2][1] = mat.b3;
    m.data[2][2] = mat.c3;
    m.data[2][3] = mat.d3;
    m.data[3][0] = mat.a4;
    m.data[3][1] = mat.b4;
    m.data[3][2] = mat.c4;
    m.data[3][3] = mat.d4;

    call(ai_mesh, sm_scene, sm_node, m);
  }

  if (node->mNumMeshes == 0) {

    printf("%s\n", node->mName.data);

    struct aiMatrix4x4 mat = node->mTransformation;
    sm_mat4 m;
    m.data[0][0] = mat.a1;
    m.data[0][1] = mat.b1;
    m.data[0][2] = mat.c1;
    m.data[0][3] = mat.d1;
    m.data[1][0] = mat.a2;
    m.data[1][1] = mat.b2;
    m.data[1][2] = mat.c2;
    m.data[1][3] = mat.d2;
    m.data[2][0] = mat.a3;
    m.data[2][1] = mat.b3;
    m.data[2][2] = mat.c3;
    m.data[2][3] = mat.d3;
    m.data[3][0] = mat.a4;
    m.data[3][1] = mat.b4;
    m.data[3][2] = mat.c4;
    m.data[3][3] = mat.d4;
    sm_transform_s t = transform_mat4_to_transform(m);

    sm_entity_s entity = sm_scene_new_entity(sm_scene, SM_TRANSFORM_COMP);
    sm_scene_set_component_data(sm_scene, entity, SM_TRANSFORM_COMP, &t);

    sm_string name;
    if (node->mName.length > 0)
      name = sm_string_from(node->mName.data);
    else
      name = sm_string_from("DEFAULT NODE NAME");

    sm_scene_graph_set_name(sm_scene->scene_graph, sm_node, name);
    sm_scene_graph_set_entity(sm_scene->scene_graph, sm_node, entity);
    sm_string_dtor(name);
  }

  /* after we've processed all of the meshes (if any) we then recursively process each of the children nodes */
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    u32 child_node = sm_scene_graph_new_node(sm_scene->scene_graph);
    sm_scene_graph_add_child(sm_scene->scene_graph, sm_node, child_node);
    process_node(node->mChildren[i], scene, sm_scene, child_node, call);
  }
}

void load_mesh(const struct aiMesh *ai_mesh, sm_scene_s *sm_scene, u32 sm_node, sm_mat4 mat) {

  sm_mesh_s mesh = {0};

  if (ai_mesh->mVertices != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_LEN(mesh.positions, ai_mesh->mNumVertices);
    memcpy(mesh.positions, ai_mesh->mVertices, ai_mesh->mNumVertices * sizeof(float) * 3);
  }

  if (ai_mesh->mNormals != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_LEN(mesh.normals, ai_mesh->mNumVertices);
    memcpy(mesh.normals, ai_mesh->mNormals, ai_mesh->mNumVertices * sizeof(float) * 3);
  }

  if (ai_mesh->mTextureCoords[0] != NULL && ai_mesh->mNumVertices > 0) {
    SM_ARRAY_SET_LEN(mesh.uvs, ai_mesh->mNumVertices);
    for (u32 uv = 0; uv < ai_mesh->mNumVertices; ++uv) {
      mesh.uvs[uv].x = ai_mesh->mTextureCoords[0][uv].x;
      mesh.uvs[uv].y = ai_mesh->mTextureCoords[0][uv].y;
    }
  } else {
    SM_ARRAY_SET_LEN(mesh.uvs, ai_mesh->mNumVertices);
    for (u32 uv = 0; uv < ai_mesh->mNumVertices; ++uv) {
      mesh.uvs[uv].x = 1.0f;
      mesh.uvs[uv].y = 1.0f;
    }
  }

  /* get the transformation matrix */

  if (ai_mesh->mColors[0] != NULL && ai_mesh->mNumVertices > 0) {
    /* SM_ARRAY_SET_LEN(mesh.colors, ai_mesh->mNumVertices); */
    SM_ALIGNED_ARRAY_NEW(mesh.colors, 16, ai_mesh->mNumVertices);
    for (u32 color = 0; color < ai_mesh->mNumVertices; ++color) {
      mesh.colors[color].r = ai_mesh->mColors[0][color].r;
      mesh.colors[color].g = ai_mesh->mColors[0][color].g;
      mesh.colors[color].b = ai_mesh->mColors[0][color].b;
      mesh.colors[color].a = ai_mesh->mColors[0][color].a;
    }
  } else {
    SM_ARRAY_SET_LEN(mesh.colors, ai_mesh->mNumVertices);
    for (u32 color = 0; color < ai_mesh->mNumVertices; ++color) {
      mesh.colors[color] = MAROON;
    }
  }

  /* load indices */
  if (ai_mesh->mFaces != NULL && ai_mesh->mNumFaces > 0) {
    for (u32 i = 0; i < ai_mesh->mNumFaces; ++i) {
      struct aiFace face = ai_mesh->mFaces[i];
      for (u32 j = 0; j < face.mNumIndices; ++j) {
        SM_ARRAY_PUSH(mesh.indices, face.mIndices[j]);
      }
    }
  }

  sm_transform_s transform = transform_mat4_to_transform(mat);

  sm_entity_s entity = sm_scene_new_entity(sm_scene, SM_TRANSFORM_COMP | SM_SPEED_COMP | SM_MESH_COMP);
  sm_scene_set_component_data(sm_scene, entity, SM_TRANSFORM_COMP, &transform);
  sm_scene_set_component_data(sm_scene, entity, SM_MESH_COMP, &mesh);
  sm_scene_set_component_data(sm_scene, entity, SM_SPEED_COMP, &(sm_speed_s){.speed = .5f});

  sm_string name;
  if (ai_mesh->mName.length > 0)
    name = sm_string_from(ai_mesh->mName.data);
  else
    name = sm_string_from("DEFAULT NAME");

  sm_scene_graph_set_name(sm_scene->scene_graph, sm_node, name);
  sm_scene_graph_set_entity(sm_scene->scene_graph, sm_node, entity);
  sm_string_dtor(name);
}
