#include "smpch.h"

#include "core/smCore.h"
#include "math/smMath.h"

#include "renderer/api/smDescriptor.h"
#include "renderer/smDevicePub.h"

#include "smCamera.h"
#include "util/colors.h"

#include "vendor/gladGL21/glad.h"

typedef struct sm__vertex_s {
  vec3 position;
  vec2 tex_coord;
  float color[4];
  vec3 normal;

} sm_vertex_s;

typedef struct sm__renderer3D_s {

  const size_t max_triangles;
  const size_t max_vertices;
  const size_t max_indices;

  struct index_buffer_s *EBO;  /* element buffer object */
  struct vertex_buffer_s *VBO; /* vertex buffer object */
  struct shader_s *program;    /* shader program */

  const sm_vertex_s *vertices;
  sm_vertex_s *__vertex_buffer;

  size_t index_count;
  uint32_t *indices;

} sm_renderer3D_s;

sm_renderer3D_s *renderer3D_new(void) {

  sm_renderer3D_s *renderer = (sm_renderer3D_s *)SM_CALLOC(1, sizeof(sm_renderer3D_s));
  SM_ASSERT(renderer);

  return renderer;
}

SM_PRIVATE enum { POSITION_LOC = 0, TEX_COORD_LOC = 1, COLOR_LOC = 2, NORMAL_LOC = 3, MAX_LOCS } sm__rederer3D_loc_e;

bool renderer3D_ctor(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  size_t max_triangles = 4096; /* TODO: make this configurable */
  size_t max_vertices = max_triangles * 3;
  size_t max_indices = max_vertices * 3;

  memcpy((void *)&renderer->max_triangles, &max_triangles, sizeof(size_t));
  memcpy((void *)&renderer->max_vertices, &max_vertices, sizeof(size_t));
  memcpy((void *)&renderer->max_indices, &max_indices, sizeof(size_t));

  renderer->vertices = SM_CALLOC(1, sizeof(sm_vertex_s) * renderer->max_vertices);

  attribute_loc_desc_s attribute_loc[4] = {
      {.name = "a_position", .location = POSITION_LOC},
      {.name = "a_tex_coord", .location = TEX_COORD_LOC},
      {.name = "a_color", .location = COLOR_LOC},
      {.name = "a_normal", .location = NORMAL_LOC},
  };

  renderer->program = DEVICE->shader_new();
  if (!DEVICE->shader_ctor(renderer->program, "StrangeMachine/glsl/renderer3D.vs", "StrangeMachine/glsl/renderer3D.fs",
                           attribute_loc, 4)) {
    return false;
  }
  DEVICE->shader_bind(renderer->program);

  buffer_desc_s vbo_desc = {
      .dynamic = false,
      .buffer_size = sizeof(sm_vertex_s) * renderer->max_vertices,
      .data = NULL,
  };

  renderer->VBO = DEVICE->vertex_buffer_new();
  if (!DEVICE->vertex_buffer_ctor(renderer->VBO, &vbo_desc))
    return false;

  renderer->VBO = DEVICE->vertex_buffer_new();
  if (!DEVICE->vertex_buffer_ctor(renderer->VBO, &vbo_desc))
    return false;

  attribute_desc_s attr_desc[4] = {{
                                       .index = POSITION_LOC,
                                       .size = sizeof(vec3) / sizeof(float),
                                       .type = 0x1406, /* GL_FLOAT */
                                       .stride = sizeof(sm_vertex_s),
                                       .pointer = (const void *)offsetof(sm_vertex_s, position),
                                   },
                                   {
                                       .index = TEX_COORD_LOC,
                                       .size = sizeof(vec2) / sizeof(float),
                                       .type = 0x1406, /* GL_FLOAT */
                                       .stride = sizeof(sm_vertex_s),
                                       .pointer = (const void *)offsetof(sm_vertex_s, tex_coord),
                                   },
                                    {
                                        .index = COLOR_LOC,
                                        .size = sizeof(vec4) / sizeof(float),
                                        .type = 0x1406, /* GL_FLOAT */
                                        .stride = sizeof(sm_vertex_s),
                                        .pointer = (const void *)offsetof(sm_vertex_s, color),
                                    },
                                   {
                                       .index = NORMAL_LOC,
                                       .size = sizeof(vec3) / sizeof(float),
                                       .type = 0x1406, /* GL_FLOAT */
                                       .stride = sizeof(sm_vertex_s),
                                       .pointer = (const void *)offsetof(sm_vertex_s, normal),
                                   }};

  DEVICE->vertex_buffer_set_pointer(renderer->VBO, attr_desc, 4);

  /* int32_t val = 0; */
  /* DEVICE->shader_set_uniform(renderer->program, "u_tex0", &val, SM_INT); */

  buffer_desc_s ebo_desc = {
      .dynamic = false,
      .buffer_size = sizeof(uint32_t) * renderer->max_indices,
      .data = NULL,
  };

  renderer->EBO = DEVICE->index_buffer_new();
  if (!DEVICE->index_buffer_ctor(renderer->EBO, &ebo_desc))
    return false;

  renderer->indices = SM_CALLOC(1, sizeof(uint32_t) * renderer->max_indices);

  DEVICE->shader_unbind(renderer->program);

  camera_init(sm_vec3_new(0.0f, 0.0f, 1.0f), sm_vec3_new(0.0f, 0.0f, 0.0f), sm_vec3_new(0.0f, 1.0f, 0.0f), THIRD_PERSON,
              PERSPECTIVE);

  return true;
}

void renderer3D_dtor(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  DEVICE->index_buffer_dtor(renderer->EBO);
  DEVICE->vertex_buffer_dtor(renderer->VBO);
  DEVICE->shader_dtor(renderer->program);

  SM_FREE(renderer->indices);
  SM_FREE((sm_vertex_s *)renderer->vertices);

  SM_FREE(renderer);
}

void renderer3D_flush(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  if (renderer->index_count == 0)
    return;

  uint32_t data_size = (uint32_t)((uint8_t *)renderer->__vertex_buffer - (uint8_t *)renderer->vertices);

  DEVICE->shader_bind(renderer->program);

  DEVICE->vertex_buffer_set_data(renderer->VBO, renderer->vertices, data_size);
  DEVICE->index_buffer_set_data(renderer->EBO, renderer->indices, renderer->index_count);

  SM_PRIVATE uint32_t locations[MAX_LOCS] = {POSITION_LOC, TEX_COORD_LOC, COLOR_LOC, NORMAL_LOC};

  DEVICE->vertex_buffer_bind(renderer->VBO, locations, MAX_LOCS);
  DEVICE->index_buffer_bind(renderer->EBO);

  DEVICE->draw_indexed(renderer->index_count);

  DEVICE->index_buffer_unbind(renderer->EBO);
  DEVICE->vertex_buffer_unbind(renderer->VBO, locations, MAX_LOCS);

  DEVICE->shader_unbind(renderer->program);
}

void renderer3D_start_batch(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  renderer->index_count = 0;
  renderer->__vertex_buffer = (sm_vertex_s *)renderer->vertices;
}

void renderer3D_begin(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  mat4 view, proj;
  camera_get_view(view);
  camera_get_projection_matrix(800.0f / 600.0f, proj);

  DEVICE->shader_bind(renderer->program);

  glEnable(GL_DEPTH_TEST);

  DEVICE->shader_set_uniform(renderer->program, "u_view", view, SM_MAT4);
  DEVICE->shader_set_uniform(renderer->program, "u_projection", proj, SM_MAT4);

  DEVICE->shader_unbind(renderer->program);

  renderer3D_start_batch(renderer);
}

void renderer3D_end(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  renderer3D_flush(renderer);
}

void renderer3D_set_clear_color(sm_renderer3D_s *renderer, sm_vec4 color) {

  SM_ASSERT(renderer);

  DEVICE->clear_color(color.x, color.y, color.z, color.w);
}

void renderer3D_clear(sm_renderer3D_s *renderer) {

  SM_ASSERT(renderer);

  DEVICE->clear(0x00004000 | 0x00000100 | 0x00000400);
}

void renderer3D_draw_cube_transform(sm_renderer3D_s *renderer, sm_transform_s transform, sm_vec4 color) {

  SM_ASSERT(renderer);

  sm_mat4 model;
  transform_to_mat4(transform, model.data);

  DEVICE->shader_bind(renderer->program);
  DEVICE->shader_set_uniform(renderer->program, "u_model", model.data, SM_MAT4);
  DEVICE->shader_unbind(renderer->program);

  static uint32_t indices[6 * 6] = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                                    4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

  static sm_vec3 vertices[8] = {sm_vec3_new(-1, -1, -1), sm_vec3_new(1, -1, -1), sm_vec3_new(1, 1, -1),
                                sm_vec3_new(-1, 1, -1),  sm_vec3_new(-1, -1, 1), sm_vec3_new(1, -1, 1),
                                sm_vec3_new(1, 1, 1),    sm_vec3_new(-1, 1, 1)};

  static sm_vec2 tex_coords[8] = {sm_vec2_new(0, 0), sm_vec2_new(1, 0), sm_vec2_new(1, 1), sm_vec2_new(0, 1)};

  static sm_vec3 normals[6] = {sm_vec3_new(0, 0, 1),  sm_vec3_new(1, 0, 0), sm_vec3_new(0, 0, -1),
                               sm_vec3_new(-1, 0, 0), sm_vec3_new(0, 1, 0), sm_vec3_new(0, -1, 0)};

  for (size_t i = 0; i < 8; i++) {
    glm_vec3_copy(vertices[i].data, renderer->__vertex_buffer->position);
    glm_vec2_copy(tex_coords[i].data, renderer->__vertex_buffer->tex_coord);
    glm_vec4_ucopy(color.data, renderer->__vertex_buffer->color);
    glm_vec3_copy(normals[i / 6].data, renderer->__vertex_buffer->normal);
    renderer->__vertex_buffer++;
  }

  memcpy(renderer->indices + renderer->index_count, indices, sizeof(indices));
  renderer->index_count += 6 * 6;
}
