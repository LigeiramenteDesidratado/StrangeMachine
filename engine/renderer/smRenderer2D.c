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

#define DRAW_VERTEX_COLOR 0.0f
#define DRAW_VERTEX_TEX0  1.0f
#define DRAW_VERTEX_TEX1  2.0f
#define DRAW_VERTEX_TEX2  3.0f
#define DRAW_VERTEX_TEX3  4.0f
#define DRAW_VERTEX_TEX4  5.0f
#define DRAW_VERTEX_TEX5  6.0f
#define DRAW_VERTEX_TEX6  7.0f
#define DRAW_VERTEX_TEX7  8.0f
#define MAX_TEXTURES      8

struct {

  uint32_t draw_call_count, previous_cc;
  uint32_t quad_count, previous_qc;

} __stats = {0, 0, 0, 0};

typedef union {
  struct {
    vec3 position;
    vec4 color;
    vec2 tex_coord;
    float tex_id;
  };

  float data[10];

} vertex_s;

typedef struct {

  device_s device;

  struct index_buffer_s *EBO;  /* element buffer object */
  struct vertex_buffer_s *VBO; /* vertex buffer object */
  struct shader_s *program;    /* shader program */

  const size_t max_quads;
  const size_t max_vertices;
  const size_t max_indices;

  vertex_s *vertices;
  vertex_s *__vertex_buffer;

  size_t index_count;
  uint32_t *indices;

  uint8_t texture_size;
  texture_handler_s textures[MAX_TEXTURES];

} renderer2D_s;

renderer2D_s *renderer2D_new(void) {

  renderer2D_s *renderer = SM_CALLOC(1, sizeof(renderer2D_s));
  SM_ASSERT(renderer);

  return renderer;
}

bool renderer2D_ctor(renderer2D_s *renderer, device_api_e device) {

  SM_ASSERT(device == OPENGL21 && "Only OpenGL 2.1 is supported for now");

  size_t max_quads = 1024;             /* TODO: make this configurable */
  size_t max_vertices = max_quads * 4; /* each quad is 4 vertices */
  size_t max_indices = max_quads * 6;  /* each quad is 6 indices */

  memcpy((void *)&renderer->max_quads, &max_quads, sizeof(size_t));
  memcpy((void *)&renderer->max_vertices, &max_vertices, sizeof(size_t));
  memcpy((void *)&renderer->max_indices, &max_indices, sizeof(size_t));

  renderer->vertices = SM_CALLOC(1, sizeof(vertex_s) * renderer->max_vertices);

  if (!device_ctor(&renderer->device, device))
    return false;

  attribute_loc_desc_s attribute_loc[4] = {
      {.name = "position", .location = 0},
      {.name = "color", .location = 1},
      {.name = "tex_coord", .location = 2},
      {.name = "tex_id", .location = 3},
  };

  renderer->program = renderer->device.shader_new();
  if (!renderer->device.shader_ctor(renderer->program, "engine/glsl/renderer2D.vs", "engine/glsl/renderer2D.fs",
                                    attribute_loc, 4)) {
    return false;
  }
  renderer->device.shader_bind(renderer->program);

  buffer_desc_s vbo_desc = {
      .dynamic = true,
      .buffer_size = sizeof(vertex_s) * renderer->max_vertices,
      .data = NULL,
  };

  renderer->VBO = renderer->device.vertex_buffer_new();
  if (!renderer->device.vertex_buffer_ctor(renderer->VBO, &vbo_desc))
    return false;

  attribute_desc_s attr_desc[4] = {
      {
          .index = 0,
          .size = sizeof(vec3) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, position),
      },
      {
          .index = 1,
          .size = sizeof(vec4) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, color),
      },
      {
          .index = 2,
          .size = sizeof(vec2) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, tex_coord),
      },
      {
          .index = 3,
          .size = sizeof(float) / sizeof(float),
          .type = 0x1406, /* GL_FLOAT */
          .stride = sizeof(vertex_s),
          .pointer = (const void *)offsetof(vertex_s, tex_id),
      },
  };

  renderer->device.vertex_buffer_set_pointer(renderer->VBO, attr_desc, 4);

  int32_t val = 0;
  renderer->device.shader_set_uniform(renderer->program, "u_tex0", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex1", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex2", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex3", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex4", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex5", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex6", &val, SM_INT);
  val++;
  renderer->device.shader_set_uniform(renderer->program, "u_tex7", &val, SM_INT);

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

  renderer->EBO = renderer->device.index_buffer_new();
  if (!renderer->device.index_buffer_ctor(renderer->EBO, &ebo_desc))
    return false;

  camera_init(vec3_new(0.0f, 3.0f, 8.0f), vec3_new(0.0f, 2.0f, 0.0f), vec3_new(0.0f, 1.0f, 0.0f), THIRD_PERSON,
              ORTHOGONAL);

  renderer->device.shader_unbind(renderer->program);

  return true;
}

void renderer2D_dtor(renderer2D_s *renderer) {

  camera_tear_down();
  renderer->device.index_buffer_dtor(renderer->EBO);
  renderer->device.vertex_buffer_dtor(renderer->VBO);
  renderer->device.shader_dtor(renderer->program);

  SM_FREE(renderer->indices);
  SM_FREE(renderer->vertices);
  SM_FREE(renderer);
}

void renderer2D_set_clear_color(renderer2D_s *renderer, vec4 color) {

  SM_ASSERT(renderer);

  renderer->device.clear_color(color[0], color[1], color[2], color[3]);
}

void renderer2D_clear(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  renderer->device.clear();
}

void renderer2D_set_viewport(renderer2D_s *renderer, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  SM_ASSERT(renderer);

  renderer->device.set_viewport(x, y, width, height);
}

vertex_s *new_quad(vertex_s *quad, vec2 position, vec2 size, vec4 color, float tex_id) {

  vertex_s v1;
  glm_vec3_copy(vec3_new(position[0], position[1], 0.0f), v1.position);
  glm_vec4_copy(color, v1.color);
  glm_vec2_copy(vec2_new(0.0f, 0.0f), v1.tex_coord);
  v1.tex_id = tex_id;

  vertex_s v2;
  glm_vec3_copy(vec3_new(size[0] + position[0], position[1], 0.0f), v2.position);
  glm_vec4_copy(color, v2.color);
  glm_vec2_copy(vec2_new(1.0f, 0.0f), v2.tex_coord);
  v2.tex_id = tex_id;

  vertex_s v3;
  glm_vec3_copy(vec3_new(size[0] + position[0], size[1] + position[1], 0.0f), v3.position);
  glm_vec4_copy(color, v3.color);
  glm_vec2_copy(vec2_new(1.0f, 1.0f), v3.tex_coord);
  v3.tex_id = tex_id;

  vertex_s v4;
  glm_vec3_copy(vec3_new(position[0], size[1] + position[1], 0.0f), v4.position);
  glm_vec4_copy(color, v4.color);
  glm_vec2_copy(vec2_new(0.0f, 1.0f), v4.tex_coord);
  v4.tex_id = tex_id;

  quad[0] = v1;
  quad[1] = v2;
  quad[2] = v3;
  quad[3] = v4;

  return quad + 4;
}

void renderer2D_flush(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  if (renderer->index_count == 0)
    return;

  uint32_t data_size = (uint32_t)((uint8_t *)renderer->__vertex_buffer - (uint8_t *)renderer->vertices);

  /* renderer->api.vertex_buffer_set_data(renderer->VBO, renderer->vertices, renderer->vertex_count * sizeof(vertex_s));
   */
  renderer->device.vertex_buffer_set_data(renderer->VBO, renderer->vertices, data_size);

  renderer->device.shader_bind(renderer->program);

  for (uint8_t i = 0; i < renderer->texture_size; ++i) {
    /* renderer->api.texture_bind(renderer->textures[i], i); */
    texture_res_bind(renderer->textures[i], i);
  }

  renderer->device.vertex_buffer_bind(renderer->VBO);
  renderer->device.index_buffer_bind(renderer->EBO);

  renderer->device.draw_indexed(renderer->index_count);

  renderer->device.index_buffer_unbind(renderer->EBO);
  renderer->device.vertex_buffer_unbind(renderer->VBO);

  for (uint8_t i = 0; i < renderer->texture_size; ++i) {
    /* renderer->api.texture_unbind(renderer->textures[i], i); */
    texture_res_unbind(renderer->textures[i], i);
  }

  renderer->device.shader_unbind(renderer->program);

  __stats.draw_call_count++;
}

void renderer2D_start_batch(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  renderer->index_count = 0;
  renderer->__vertex_buffer = renderer->vertices;
}

void renderer2D_begin(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  resource_set_device_reference(&renderer->device);

  mat4 view, proj;
  camera_get_view(view);
  camera_get_projection_matrix(800 / (float)600, proj);

  renderer->device.shader_bind(renderer->program);

  renderer->device.shader_set_uniform(renderer->program, "u_view", view, SM_MAT4);
  renderer->device.shader_set_uniform(renderer->program, "u_projection", proj, SM_MAT4);
  renderer->device.shader_unbind(renderer->program);

  renderer2D_start_batch(renderer);
}

void renderer2D_end(renderer2D_s *renderer) {

  SM_ASSERT(renderer);

  renderer2D_flush(renderer);

  resource_unset_device_reference();

  __stats.previous_cc = __stats.draw_call_count;
  __stats.previous_qc = __stats.quad_count;
  __stats.draw_call_count = 0;
  __stats.quad_count = 0;
}

#define QUAD_SIZE 4

void renderer2D_draw_quad(renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id) {

  /* check if this new quad will not exceed the vertex buffer */
  if (renderer->index_count + 6 > renderer->max_indices) {
    renderer2D_flush(renderer);
    renderer2D_start_batch(renderer);
  }
  renderer->__vertex_buffer = new_quad(renderer->__vertex_buffer, position, size, color, tex_id);
  renderer->index_count += 6;
  __stats.quad_count++;
}

void renderer2D_draw_sprite(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler) {

  SM_UNUSED(renderer);
  SM_UNUSED(position);
  SM_UNUSED(size);

  /* check if this new quad will not exceed the vertex buffer */
  if (renderer->index_count + 6 > renderer->max_indices) {
    renderer2D_flush(renderer);
    renderer2D_start_batch(renderer);
  }

  float tex_id = 0.0f;

  for (uint8_t i = 0; i < MAX_TEXTURES; ++i) {
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

  renderer->__vertex_buffer = new_quad(renderer->__vertex_buffer, position, size, SM_RED_COLOR, tex_id);
  renderer->index_count += 6;
  __stats.quad_count++;
}

void renderer2D_draw_quad_rotated(renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id,
                                  float deg_angle);

void renderer2D_draw_sprite_rotated(renderer2D_s *renderer, vec2 position, vec2 size, texture_handler_s handler,
                                    float deg_angle) {

  SM_UNUSED(renderer);
  SM_UNUSED(position);
  SM_UNUSED(size);

  /* check if this new quad will not exceed the vertex buffer */
  if (renderer->index_count + 6 > renderer->max_indices) {
    renderer2D_flush(renderer);
    renderer2D_start_batch(renderer);
  }

  float tex_id = 0.0f;

  for (uint8_t i = 0; i < MAX_TEXTURES; ++i) {
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

  renderer2D_draw_quad_rotated(renderer, position, size, SM_RED_COLOR, tex_id, deg_angle);

  /* renderer->__vertex_buffer = new_quad(renderer->__vertex_buffer, position, size, SM_RED_COLOR, tex_id); */
  /* renderer->index_count += 6; */
  /* __stats.quad_count++; */
}

void renderer2D_draw_quad_rotated(renderer2D_s *renderer, vec2 position, vec2 size, vec4 color, float tex_id,
                                  float deg_angle) {

  SM_ASSERT(renderer);

  /* check if this new quad will not exceed the vertex buffer */
  if (renderer->index_count + 6 > renderer->max_indices) {
    renderer2D_flush(renderer);
    renderer2D_start_batch(renderer);
  }

  if (deg_angle == 0.0f || deg_angle == 360.0f) {
    renderer->__vertex_buffer = new_quad(renderer->__vertex_buffer, position, size, color, tex_id);
    renderer->index_count += 6;
    __stats.quad_count++;
    return;
  }

  vec3 center = {position[0] + size[0] * 0.5f, position[1] + size[1] * 0.5f, 0.0f};

  mat4 transform = GLM_MAT4_IDENTITY_INIT;

  /* use TRS to rotate around the center */
  glm_translate(transform, center);
  glm_rotate(transform, glm_rad(deg_angle), vec3_new(0.0f, 0.0f, 1.0f));
  glm_scale(transform, vec3_new(size[0], size[1], 1.0f));

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
    glm_vec4_ucopy(color, renderer->__vertex_buffer[i].color);
    glm_vec2_copy(tex_coords[i], renderer->__vertex_buffer[i].tex_coord);
    renderer->__vertex_buffer[i].tex_id = tex_id;
  }

  renderer->__vertex_buffer += QUAD_SIZE;
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
