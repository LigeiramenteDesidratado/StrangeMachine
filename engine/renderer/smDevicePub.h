#ifndef SM_DEVICE_PUB_H
#define SM_DEVICE_PUB_H

#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"
#include "renderer/smDeviceDefs.h"

/* Polymorphic behavior functions */
/* Shader */
typedef struct shader_s *(shader_new_f)(void);
typedef bool(shader_ctor_f)(struct shader_s *shader, const char *vertex_shader, const char *fragment_shader,
                            attribute_loc_desc_s *desc, size_t size);
typedef void(shader_dtor_f)(struct shader_s *shader);
typedef void(shader_bind_f)(struct shader_s *shader);
typedef void(shader_unbind_f)(struct shader_s *shader);
typedef void(shader_set_uniform_f)(struct shader_s *shader, const char *name, void *value, types_e type);

/* Index buffer */
typedef struct index_buffer_s *(index_buffer_new_f)(void);
typedef bool(index_buffer_ctor_f)(struct index_buffer_s *buffer, buffer_desc_s *desc);
typedef void(index_buffer_dtor_f)(struct index_buffer_s *buffer);
typedef void(index_buffer_set_data_f)(struct index_buffer_s *index_buffer, uint32_t *data, size_t length);
typedef void(index_buffer_bind_f)(struct index_buffer_s *buffer);
typedef void(index_buffer_unbind_f)(struct index_buffer_s *buffer);

/* Vertex buffer */
typedef struct vertex_buffer_s *(vertex_buffer_new_f)(void);
typedef bool(vertex_buffer_ctor_f)(struct vertex_buffer_s *vertex_buffer, buffer_desc_s *desc);
typedef void(vertex_buffer_dtor_f)(struct vertex_buffer_s *vertex_buffer);
typedef void(vertex_buffer_set_pointer_f)(struct vertex_buffer_s *vertex_buffer, attribute_desc_s *attributes,
                                          size_t length);
typedef void(vertex_buffer_set_data_f)(struct vertex_buffer_s *vertex_buffer, void *data, size_t length);
typedef void(vertex_buffer_bind_f)(struct vertex_buffer_s *vertex_buffer);
typedef void(vertex_buffer_unbind_f)(struct vertex_buffer_s *vertex_buffer);

typedef void(clear_f)(void);
typedef void(clear_color_f)(float r, float g, float b, float a);
typedef void(set_viewport_f)(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
typedef void(draw_indexed_f)(uint32_t index_count);

/* The scene_s "class" has three behavior functions. The draw_f, do_f and get_look_at_f functions
 * are supposed to be polymorphic and can be overridden by the child classes. */
typedef struct {

  /* api will works like a runtime type check */
  device_api_e device_api;

  clear_f *clear;
  clear_color_f *clear_color;
  set_viewport_f *set_viewport;
  draw_indexed_f *draw_indexed;

  /* Shader */
  shader_new_f *shader_new;
  shader_ctor_f *shader_ctor;
  shader_dtor_f *shader_dtor;
  shader_bind_f *shader_bind;
  shader_unbind_f *shader_unbind;
  shader_set_uniform_f *shader_set_uniform;

  /* Index buffer */
  index_buffer_new_f *index_buffer_new;
  index_buffer_ctor_f *index_buffer_ctor;
  index_buffer_dtor_f *index_buffer_dtor;
  index_buffer_set_data_f *index_buffer_set_data;
  index_buffer_bind_f *index_buffer_bind;
  index_buffer_unbind_f *index_buffer_unbind;

  /* Vertex buffer */
  vertex_buffer_new_f *vertex_buffer_new;
  vertex_buffer_ctor_f *vertex_buffer_ctor;
  vertex_buffer_dtor_f *vertex_buffer_dtor;
  vertex_buffer_set_pointer_f *vertex_buffer_set_pointer;
  vertex_buffer_set_data_f *vertex_buffer_set_data;
  vertex_buffer_bind_f *vertex_buffer_bind;
  vertex_buffer_unbind_f *vertex_buffer_unbind;

} device_s;

bool device_ctor(device_s *device, device_api_e api);

#endif /* SM_DEVICE_PUB_H */
