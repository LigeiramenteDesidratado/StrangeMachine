#include "scene/smComponents.h"
#include "scene/smScene.h"
#include "smpch.h"

#include "core/smCore.h"
#include "math/smMath.h"

#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"
#include "renderer/smDevicePub.h"

#include "smCamera.h"
#include "util/colors.h"

typedef struct sm__renderer3D_vertex_s {
  sm_vec3 position;
  sm_vec2 tex_coord;
  f32 color[4];
  sm_vec3 normal;

} sm_renderer3D_vertex_s;

typedef struct sm__renderer3D_s {

  const size_t max_triangles;
  const size_t max_vertices;
  const size_t max_indices;

  struct index_buffer_s *EBO;  /* element buffer object */
  struct vertex_buffer_s *VBO; /* vertex buffer object */
  struct shader_s *program;    /* shader program */

  const sm_renderer3D_vertex_s *vertices;
  sm_renderer3D_vertex_s *__vertex_buffer;

  size_t index_count;
  u32 *indices;

} sm_renderer3D_s;

SM_PRIVATE enum { POSITION_LOC = 0, TEX_COORD_LOC = 1, COLOR_LOC = 2, NORMAL_LOC = 3, MAX_LOCS } sm__rederer3D_loc_e;

sm_renderer3D_s *RENDERER3D = NULL;

void sm_renderer3D_init(void) {

  SM_ASSERT(RENDERER3D == NULL && "Renderer 3D already initialized");

  sm_renderer3D_s *renderer = (sm_renderer3D_s *)SM_CALLOC(1, sizeof(sm_renderer3D_s));
  SM_ASSERT(renderer);

  size_t max_triangles = 4096; /* TODO: make this configurable */
  size_t max_vertices = max_triangles * 3;
  size_t max_indices = max_vertices * 3;

  memcpy((void *)&renderer->max_triangles, &max_triangles, sizeof(size_t));
  memcpy((void *)&renderer->max_vertices, &max_vertices, sizeof(size_t));
  memcpy((void *)&renderer->max_indices, &max_indices, sizeof(size_t));

  renderer->vertices = SM_CALLOC(1, sizeof(sm_renderer3D_vertex_s) * renderer->max_vertices);

  sm_string shader_path = sm_string_from("assets/shaders/renderer3D.shader");
  sm_shader_resource_handler_s shader = sm_shader_resource_new(shader_path);
  sm_string_dtor(shader_path);

  renderer->program = DEVICE->shader_new();
  if (!DEVICE->shader_ctor(renderer->program, shader)) {
    return;
  }

  DEVICE->shader_bind(renderer->program);

  buffer_desc_s vbo_desc = {
      .dynamic = false,
      .buffer_size = sizeof(sm_renderer3D_vertex_s) * renderer->max_vertices,
      .data = NULL,
  };

  renderer->VBO = DEVICE->vertex_buffer_new();
  if (!DEVICE->vertex_buffer_ctor(renderer->VBO, &vbo_desc))
    return;

  attribute_desc_s attr_desc[4] = {{
                                       .index = POSITION_LOC,
                                       .size = sizeof(sm_vec3) / sizeof(f32),
                                       .type = SM_F32,
                                       .stride = sizeof(sm_renderer3D_vertex_s),
                                       .pointer = (const void *)offsetof(sm_renderer3D_vertex_s, position),
                                   },
                                   {
                                       .index = TEX_COORD_LOC,
                                       .size = sizeof(vec2) / sizeof(f32),
                                       .type = SM_F32,
                                       .stride = sizeof(sm_renderer3D_vertex_s),
                                       .pointer = (const void *)offsetof(sm_renderer3D_vertex_s, tex_coord),
                                   },
                                   {
                                       .index = COLOR_LOC,
                                       .size = sizeof(vec4) / sizeof(f32),
                                       .type = SM_F32,
                                       .stride = sizeof(sm_renderer3D_vertex_s),
                                       .pointer = (const void *)offsetof(sm_renderer3D_vertex_s, color),
                                   },
                                   {
                                       .index = NORMAL_LOC,
                                       .size = sizeof(sm_vec3) / sizeof(f32),
                                       .type = SM_F32,
                                       .stride = sizeof(sm_renderer3D_vertex_s),
                                       .pointer = (const void *)offsetof(sm_renderer3D_vertex_s, normal),
                                   }};

  DEVICE->vertex_buffer_set_pointer(renderer->VBO, attr_desc, 4);

  buffer_desc_s ebo_desc = {
      .dynamic = false,
      .buffer_size = sizeof(u32) * renderer->max_indices,
      .data = NULL,
  };

  renderer->EBO = DEVICE->index_buffer_new();
  if (!DEVICE->index_buffer_ctor(renderer->EBO, &ebo_desc))
    return;

  renderer->indices = SM_CALLOC(1, sizeof(u32) * renderer->max_indices);

  DEVICE->shader_unbind(renderer->program);

  /* camera_init(sm_vec3_new(0.0f, 0.0f, 1.0f), sm_vec3_new(0.0f, 0.0f, 0.0f), sm_vec3_new(0.0f, 1.0f, 0.0f),
   * THIRD_PERSON, */
  /*             PERSPECTIVE); */

  SM_LOG_TRACE("Renderer 3D initialized");

  RENDERER3D = renderer;
}

void sm_renderer3D_teardown(void) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  DEVICE->index_buffer_dtor(renderer->EBO);
  DEVICE->vertex_buffer_dtor(renderer->VBO);
  DEVICE->shader_dtor(renderer->program);
  SM_FREE(renderer->indices);
  SM_FREE((sm_renderer3D_vertex_s *)renderer->vertices);

  SM_FREE(renderer);
}

void sm_renderer3D_flush(void) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  if (renderer->index_count == 0)
    return;

  u32 data_size = (u32)((u8 *)renderer->__vertex_buffer - (u8 *)renderer->vertices);

  DEVICE->shader_bind(renderer->program);

  DEVICE->vertex_buffer_set_data(renderer->VBO, renderer->vertices, data_size);
  DEVICE->index_buffer_set_data(renderer->EBO, renderer->indices, renderer->index_count);

  SM_PRIVATE u32 locations[MAX_LOCS] = {POSITION_LOC, TEX_COORD_LOC, COLOR_LOC, NORMAL_LOC};

  DEVICE->vertex_buffer_bind(renderer->VBO, locations, MAX_LOCS);
  DEVICE->index_buffer_bind(renderer->EBO);

  DEVICE->draw_indexed(renderer->index_count);

  DEVICE->index_buffer_unbind(renderer->EBO);
  DEVICE->vertex_buffer_unbind(renderer->VBO, locations, MAX_LOCS);

  DEVICE->shader_unbind(renderer->program);
}

void sm_renderer3D_start_batch(void) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  renderer->index_count = 0;
  renderer->__vertex_buffer = (sm_renderer3D_vertex_s *)renderer->vertices;
}

void sm_renderer3D_begin(sm_mat4 pv_matrix) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  /* mat4 view, proj; */
  /* camera_get_view(view); */
  /* camera_get_projection_matrix(800.0f / 600.0f, proj); */

  DEVICE->shader_bind(renderer->program);

  DEVICE->enable(SM_DEPTH_TEST);

  sm_string pv_loc = sm_string_from("u_pv");
  DEVICE->shader_set_uniform(renderer->program, pv_loc, pv_matrix.data, SM_MAT4);
  sm_string_dtor(pv_loc);

  DEVICE->shader_unbind(renderer->program);

  sm_renderer3D_start_batch();
}

void sm_renderer3D_end(void) {

  sm_renderer3D_flush();
}

void sm_renderer3D_set_clear_color(sm_vec4 color) {

  DEVICE->clear_color(color.x, color.y, color.z, color.w);
}

void sm_renderer3D_clear(void) {

  DEVICE->clear(SM_DEPTH_BUFFER_BIT | SM_COLOR_BUFFER_BIT);
}

void sm_renderer3D_draw_cube_transform(sm_transform_s transform, sm_vec4 color) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  sm_mat4 model;
  transform_to_mat4(transform, model.data);

  DEVICE->shader_bind(renderer->program);
  /* DEVICE->shader_set_uniform(renderer->program, "u_model", model.data, SM_MAT4); */

  sm_string u_model = sm_string_from("u_model");
  DEVICE->shader_set_uniform(renderer->program, u_model, model.data, SM_MAT4);
  sm_string_dtor(u_model);

  DEVICE->shader_unbind(renderer->program);

  static u32 indices[6 * 6] = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                               4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

  static sm_vec3 vertices[8] = {sm_vec3_new(-1.0f, -1.0f, -1.0f), sm_vec3_new(1.0f, -1.0f, -1.0f),
                                sm_vec3_new(1.0f, 1.0f, -1.0f),   sm_vec3_new(-1.0f, 1.0f, -1.0f),
                                sm_vec3_new(-1.0f, -1.0f, 1.0f),  sm_vec3_new(1.0f, -1.0f, 1.0f),
                                sm_vec3_new(1.0f, 1.0f, 1.0f),    sm_vec3_new(-1.0f, 1.0f, 1.0f)};

  static sm_vec2 tex_coords[8] = {sm_vec2_new(0.0f, 0.0f), sm_vec2_new(1.0f, 0.0f), sm_vec2_new(1.0f, 1.0f),
                                  sm_vec2_new(0.0f, 1.0f)};

  static sm_vec3 normals[6] = {sm_vec3_new(0.0f, 0.0f, 1.0f),  sm_vec3_new(1.0f, 0.0f, 0.0f),
                               sm_vec3_new(0.0f, 0.0f, -1.0f), sm_vec3_new(-1.0f, 0.0f, 0.0f),
                               sm_vec3_new(0.0f, 1.0f, 0.0f),  sm_vec3_new(0.0f, -1.0f, 0.0f)};

  for (size_t i = 0; i < 8; i++) {
    glm_vec3_copy(vertices[i].data, renderer->__vertex_buffer->position.data);
    glm_vec2_copy(tex_coords[i].data, renderer->__vertex_buffer->tex_coord.data);
    glm_vec4_ucopy(color.data, renderer->__vertex_buffer->color);
    glm_vec3_copy(normals[i / 6].data, renderer->__vertex_buffer->normal.data);
    renderer->__vertex_buffer++;
  }

  memcpy(renderer->indices + renderer->index_count, indices, sizeof(indices));
  renderer->index_count += 6 * 6;
}

void sm_renderer3D_draw_mesh(const sm_mesh_s *mesh, sm_transform_s transform) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  sm_mat4 model;
  transform_to_mat4(transform, model.data);

  DEVICE->shader_bind(renderer->program);
  sm_string u_model = sm_string_from("u_model");
  DEVICE->shader_set_uniform(renderer->program, u_model, model.data, SM_MAT4);
  sm_string_dtor(u_model);
  DEVICE->shader_unbind(renderer->program);

  for (size_t i = 0; i < SM_ARRAY_LEN(mesh->positions); ++i) {
    glm_vec3_copy(mesh->positions[i].data, renderer->__vertex_buffer->position.data);
    glm_vec2_copy(mesh->uvs[i].data, renderer->__vertex_buffer->tex_coord.data);
    glm_vec4_ucopy(mesh->colors[i].data, renderer->__vertex_buffer->color);
    glm_vec3_copy(mesh->normals[i].data, renderer->__vertex_buffer->normal.data);
    renderer->__vertex_buffer++;
  }
  memcpy(renderer->indices + renderer->index_count, mesh->indices, sizeof(u32) * SM_ARRAY_LEN(mesh->indices));
  renderer->index_count += SM_ARRAY_LEN(mesh->indices);
}

void sm_renderer3D_draw_scene(const sm_scene_s *scene) {

  sm_renderer3D_s *renderer = RENDERER3D;

  SM_ASSERT(renderer);

  /* for (size_t i = 0; i < SM_ARRAY_LEN(scene->meshes); ++i) { */
  /*   sm_renderer3D_draw_mesh(scene->meshes + i, scene->transforms + i); */
  /* } */
}
