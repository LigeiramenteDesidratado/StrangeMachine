#include "smpch.h"

#include "renderer/api/smTypes.h"
#include "renderer/smDeviceDefs.h"
#include "renderer/smDevicePub.h"

#include "resource/smResource.h"

#include "util/colors.h"

#include "math/smMath.h"

#include "core/smCore.h"

#include "smCamera.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "RENDERER2D"

#define SM_DRAW_VERTEX_COLOR 0.0f
#define SM_DRAW_VERTEX_TEX0  1.0f
#define SM_DRAW_VERTEX_TEX1  2.0f
#define SM_DRAW_VERTEX_TEX2  3.0f
#define SM_DRAW_VERTEX_TEX3  4.0f
#define SM_DRAW_VERTEX_TEX4  5.0f
#define SM_DRAW_VERTEX_TEX5  6.0f
#define SM_DRAW_VERTEX_TEX6  7.0f
#define SM_DRAW_VERTEX_TEX7  8.0f
#define SM_MAX_TEXTURES      8

struct sm__stats {

  uint32_t draw_call_count, previous_cc;
  uint32_t quad_count, previous_qc;

} __stats = {0, 0, 0, 0};

typedef struct vertex_s {
  vec3 position;
  vec4 color;
  vec2 tex_coord;
  float tex_id;

} vertex_s;

typedef struct renderer2D_s {

  struct index_buffer_s *EBO;  /* element buffer object */
  struct vertex_buffer_s *VBO; /* vertex buffer object */
  struct shader_s *program;    /* shader program */

  const size_t max_quads;
  const size_t max_vertices;
  const size_t max_indices;

  const vertex_s *vertices;
  vertex_s *__vertex_buffer;

  size_t index_count;
  uint32_t *indices;

  uint8_t texture_size;
  texture_handler_s textures[SM_MAX_TEXTURES];

} renderer2D_s;

/* private functions */
SM_PRIVATE
vertex_s *sm__renderer2D_new_quad(vertex_s *quad, vec2 position, vec2 size, vec4 color, float tex_id);
SM_PRIVATE
void sm__renderer2D_draw_quad_pro(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color, float tex_id,
                                  float deg_angle);

/* public functions */
void renderer2D_draw_quad(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color);
void renderer2D_draw_quad_rotated(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color, float deg_angle);
void renderer2D_draw_sprite(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler);
void renderer2D_draw_sprite_rotated(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler,
                                    float deg_angle);

renderer2D_s *renderer2D_new(void) {

  renderer2D_s *renderer = SM_CALLOC(1, sizeof(renderer2D_s));
  SM_ASSERT(renderer);

  return renderer;
}

SM_PRIVATE enum { POSITION_LOC = 0, COLOR_LOC = 1, TEX_COORD_LOC = 2, TEX_ID_LOC = 3, MAX_LOCS } sm__rederer2D_loc_e;

bool renderer2D_ctor(renderer2D_s *renderer) {

  size_t max_quads = 1024;             /* TODO: make this configurable */
  size_t max_vertices = max_quads * 4; /* each quad is 4 vertices */
  size_t max_indices = max_quads * 6;  /* each quad is 6 indices */

  memcpy((void *)&renderer->max_quads, &max_quads, sizeof(size_t));
  memcpy((void *)&renderer->max_vertices, &max_vertices, sizeof(size_t));
  memcpy((void *)&renderer->max_indices, &max_indices, sizeof(size_t));

  renderer->vertices = SM_CALLOC(1, sizeof(vertex_s) * renderer->max_vertices);

  attribute_loc_desc_s attribute_loc[4] = {
      {.name = "position", .location = POSITION_LOC},
      {.name = "color", .location = COLOR_LOC},
      {.name = "tex_coord", .location = TEX_COORD_LOC},
      {.name = "tex_id", .location = TEX_ID_LOC},
  };

  renderer->program = DEVICE->shader_new();
  if (!DEVICE->shader_ctor(renderer->program, "StrangeMachine/glsl/renderer2D.vs", "StrangeMachine/glsl/renderer2D.fs",
                           attribute_loc, 4)) {
    return false;
  }
  DEVICE->shader_bind(renderer->program);

  buffer_desc_s vbo_desc = {
      .dynamic = true,
      .buffer_size = sizeof(vertex_s) * renderer->max_vertices,
      .data = NULL,
  };

  renderer->VBO = DEVICE->vertex_buffer_new();
  if (!DEVICE->vertex_buffer_ctor(renderer->VBO, &vbo_desc))
    return false;

  attribute_desc_s attr_desc[4] = {
      {
          .index = POSITION_LOC,
          .size = sizeof(vec3) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, position),
      },
      {
          .index = COLOR_LOC,
          .size = sizeof(vec4) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, color),
      },
      {
          .index = TEX_COORD_LOC,
          .size = sizeof(vec2) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, tex_coord),
      },
      {
          .index = TEX_ID_LOC,
          .size = sizeof(float) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, tex_id),
      },
  };

  DEVICE->vertex_buffer_set_pointer(renderer->VBO, attr_desc, 4);

  int32_t val = 0;
  DEVICE->shader_set_uniform(renderer->program, "u_tex0", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex1", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex2", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex3", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex4", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex5", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex6", &val, SM_INT);
  val++;
  DEVICE->shader_set_uniform(renderer->program, "u_tex7", &val, SM_INT);

  uint32_t indices[renderer->max_indices];
  uint32_t offset = 0;
  SM_ASSERT(renderer->max_indices % 6 == 0);
  for (size_t i = 0; i < renderer->max_indices; i += 6) {
    indices[i + 0] = 0 + offset;
    indices[i + 1] = 1 + offset;
    indices[i + 2] = 2 + offset;

    indices[i + 3] = 2 + offset;
    indices[i + 4] = 3 + offset;
    indices[i + 5] = 0 + offset;

    offset += 4;
  }

  buffer_desc_s ebo_desc = {
      .dynamic = false,
      .buffer_size = sizeof(uint32_t) * renderer->max_indices,
      .data = indices,
  };

  renderer->EBO = DEVICE->index_buffer_new();
  if (!DEVICE->index_buffer_ctor(renderer->EBO, &ebo_desc))
    return false;

  camera_init(sm_vec3_new(0.0f, 0.0f, -1.0f), sm_vec3_new(0.0f, 0.0f, 0.0f), sm_vec3_new(0.0f, 1.0f, 0.0f),
              THIRD_PERSON, PERSPECTIVE);

  DEVICE->shader_unbind(renderer->program);

  return true;
}

void renderer2D_dtor(renderer2D_s *renderer) {

  camera_tear_down();
  DEVICE->index_buffer_dtor(renderer->EBO);
  DEVICE->vertex_buffer_dtor(renderer->VBO);
  DEVICE->shader_dtor(renderer->program);

  SM_FREE(renderer->indices);
  SM_FREE((vertex_s *)renderer->vertices);
  SM_FREE(renderer);
}

void renderer2D_flush(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  if (renderer->index_count == 0)
    return;

  uint32_t data_size = (uint32_t)((uint8_t *)renderer->__vertex_buffer - (uint8_t *)renderer->vertices);
  DEVICE->vertex_buffer_set_data(renderer->VBO, renderer->vertices, data_size);

  DEVICE->shader_bind(renderer->program);

  for (uint8_t i = 0; i < renderer->texture_size; ++i)
    texture_res_bind(renderer->textures[i], i);

  SM_PRIVATE uint32_t locations[MAX_LOCS] = {POSITION_LOC, COLOR_LOC, TEX_COORD_LOC, TEX_ID_LOC};

  DEVICE->vertex_buffer_bind(renderer->VBO, locations, MAX_LOCS);
  DEVICE->index_buffer_bind(renderer->EBO);

  DEVICE->draw_indexed(renderer->index_count);

  DEVICE->index_buffer_unbind(renderer->EBO);
  DEVICE->vertex_buffer_unbind(renderer->VBO, locations, MAX_LOCS);

  for (uint8_t i = 0; i < renderer->texture_size; ++i)
    texture_res_unbind(renderer->textures[i], i);

  DEVICE->shader_unbind(renderer->program);

  __stats.draw_call_count++;
}

void renderer2D_start_batch(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  renderer->index_count = 0;
  renderer->texture_size = 0;
  renderer->__vertex_buffer = (vertex_s *)renderer->vertices;
}

void renderer2D_begin(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  mat4 view, proj;
  camera_get_view(view);
  camera_get_projection_matrix(800 / (float)600, proj);

  DEVICE->shader_bind(renderer->program);

  DEVICE->shader_set_uniform(renderer->program, "u_view", view, SM_MAT4);
  DEVICE->shader_set_uniform(renderer->program, "u_projection", proj, SM_MAT4);

  DEVICE->shader_unbind(renderer->program);

  renderer2D_start_batch(renderer);
}

void renderer2D_end(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  renderer2D_flush(renderer);

  __stats.previous_cc = __stats.draw_call_count;
  __stats.previous_qc = __stats.quad_count;
  __stats.draw_call_count = 0;
  __stats.quad_count = 0;
}

#define QUAD_SIZE 4

void renderer2D_draw_quad(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color) {

  sm__renderer2D_draw_quad_pro(renderer, position, size, color, SM_DRAW_VERTEX_COLOR, 0.0f);
}

void renderer2D_draw_quad_rotated(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color, float deg_angle) {

  sm__renderer2D_draw_quad_pro(renderer, position, size, color, SM_DRAW_VERTEX_COLOR, deg_angle);
}

void renderer2D_draw_sprite(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler) {

  renderer2D_draw_sprite_rotated(renderer, position, size, handler, 0.0f);
}

void renderer2D_draw_sprite_rotated(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler,
                                    float deg_angle) {

  float tex_id = 0.0f;

  for (uint8_t i = 0; i < renderer->texture_size; ++i) {
    if (handler.handle == renderer->textures[i].handle) {
      tex_id = (float)i + 1;
      break;
    }
  }

  if (tex_id == 0.0f) {
    renderer->textures[renderer->texture_size] = handler;
    renderer->texture_size++; /* TODO: check array overflow and batch it */
    tex_id = renderer->texture_size;
  }

  sm__renderer2D_draw_quad_pro(renderer, position, size, SM_RED_COLOR, tex_id, deg_angle);
}

void renderer2D_set_clear_color(renderer2D_s *renderer, sm_vec4 color) {

  SM_ASSERT(renderer);

  DEVICE->clear_color(color.x, color.y, color.z, color.w);
}

void renderer2D_clear(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  DEVICE->clear();
}

void renderer2D_set_viewport(renderer2D_s *renderer, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  SM_ASSERT(renderer);

  DEVICE->set_viewport(x, y, width, height);
}

SM_PRIVATE
vertex_s *sm__renderer2D_new_quad(vertex_s *quad, vec2 position, vec2 size, vec4 color, float tex_id) {

  sm_vec3 pos_data[QUAD_SIZE] = {sm_vec3_new(position[0], position[1], 0.0f),
                                 sm_vec3_new(size[0] + position[0], position[1], 0.0f),
                                 sm_vec3_new(size[0] + position[0], size[1] + position[1], 0.0f),
                                 sm_vec3_new(position[0], size[1] + position[1], 0.0f)};

  sm_vec2 tex_data[QUAD_SIZE] = {sm_vec2_new(0.0f, 0.0f), sm_vec2_new(1.0f, 0.0f), sm_vec2_new(1.0f, 1.0f),
                                 sm_vec2_new(0.0f, 1.0f)};

  /* vertex_s v1; */
  glm_vec3_copy(pos_data[0].data, quad[0].position);
  glm_vec4_ucopy(color, quad[0].color);
  glm_vec2_copy(tex_data[0].data, quad[0].tex_coord);
  quad[0].tex_id = tex_id;

  /* vertex_s v2; */
  glm_vec3_copy(pos_data[1].data, quad[1].position);
  glm_vec4_ucopy(color, quad[1].color);
  glm_vec2_copy(tex_data[1].data, quad[1].tex_coord);
  quad[1].tex_id = tex_id;

  /* vertex_s v3; */
  glm_vec3_copy(pos_data[2].data, quad[2].position);
  glm_vec4_ucopy(color, quad[2].color);
  glm_vec2_copy(tex_data[2].data, quad[2].tex_coord);
  quad[2].tex_id = tex_id;

  /* vertex_s v4; */
  glm_vec3_copy(pos_data[3].data, quad[3].position);
  glm_vec4_ucopy(color, quad[3].color);
  glm_vec2_copy(tex_data[3].data, quad[3].tex_coord);
  quad[3].tex_id = tex_id;

  /* quad[0] = v1; */
  /* quad[1] = v2; */
  /* quad[2] = v3; */
  /* quad[3] = v4; */

  return quad + 4;
}

SM_PRIVATE
void sm__renderer2D_draw_quad_pro(renderer2D_s *renderer, vec2 position, vec2 size, sm_vec4 color, float tex_id,
                                  float deg_angle) {

  SM_ASSERT(renderer);

  /* check if this new quad will not exceed the vertex buffer */
  if (renderer->index_count + 6 > renderer->max_indices) {
    /* in case of overflow, flush the current batch */
    renderer2D_flush(renderer);
    /* and start a new batch */
    renderer2D_start_batch(renderer);
  }

  if (deg_angle == 0.0f || deg_angle == 360.0f) {
    renderer->__vertex_buffer = sm__renderer2D_new_quad(renderer->__vertex_buffer, position, size, color.data, tex_id);
    goto increment;
  }

  vec3 center = {position[0] + size[0] * 0.5f, position[1] + size[1] * 0.5f, 0.0f};

  mat4 transform = GLM_MAT4_IDENTITY_INIT;

  /* use TRS to rotate around the center */
  glm_translate(transform, center);

  sm_vec3 rotation = sm_vec3_new(0.0f, 0.0f, 1.0f);
  glm_rotate(transform, glm_rad(deg_angle), rotation.data);

  sm_vec3 scale = sm_vec3_new(size[0], size[1], 1.0f);
  glm_scale(transform, scale.data);

  vec2 tex_coords[QUAD_SIZE] = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
  };

  vec4 quad_vertex_positions[QUAD_SIZE] = {
      {-0.5f, -0.5f, 0.0f, 1.0f},
      {0.5f, -0.5f, 0.0f, 1.0f},
      {0.5f, 0.5f, 0.0f, 1.0f},
      {-0.5f, 0.5f, 0.0f, 1.0f},
  };

  for (int i = 0; i < QUAD_SIZE; i++) {

    glm_mat4_mulv3(transform, quad_vertex_positions[i], 1.0f, renderer->__vertex_buffer[i].position);
    glm_vec4_ucopy(color.data, renderer->__vertex_buffer[i].color);
    glm_vec2_copy(tex_coords[i], renderer->__vertex_buffer[i].tex_coord);
    renderer->__vertex_buffer[i].tex_id = tex_id;
  }

  renderer->__vertex_buffer += QUAD_SIZE;

increment:
  renderer->index_count += 6;
  __stats.quad_count++;
}

uint32_t renderer2D_stats_get_draw_call_count(void) {
  return __stats.previous_cc;
}

uint32_t renderer2D_stats_get_quad_count(void) {
  return __stats.previous_qc;
}

#undef SM_MODULE_NAME
