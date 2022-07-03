#ifndef SM_DEVICE_PUB_H
#define SM_DEVICE_PUB_H

#include "smpch.h"

#include "renderer/api/smDescriptor.h"
#include "renderer/api/smTypes.h"
#include "renderer/smDeviceDefs.h"

#include "resource/smShaderResource.h"
#include "resource/smTextureResourcePub.h"

typedef void *(*loadproc_f)(const char *name);

/* Polymorphic behavior functions */
/* Shader */
typedef struct shader_s *(*shader_new_f)(void);
typedef b8 (*shader_ctor_f)(struct shader_s *shader, sm_shader_resource_handler_s handler);
typedef void (*shader_dtor_f)(struct shader_s *shader);
typedef void (*shader_bind_f)(struct shader_s *shader);
typedef void (*shader_unbind_f)(struct shader_s *shader);
typedef void (*shader_set_uniform_f)(struct shader_s *shader, sm_string name, void *value, types_e type);
typedef void (*shader_set_uniform_array_f)(struct shader_s *shader, const char *name, void *value, u32 size,
                                           types_e type);
typedef i32 (*shader_get_location_f)(struct shader_s *shader, sm_string name);
typedef types_e (*shader_get_type_f)(struct shader_s *shader, sm_string name);

/* Index buffer */
typedef struct index_buffer_s *(*index_buffer_new_f)(void);
typedef b8 (*index_buffer_ctor_f)(struct index_buffer_s *buffer, buffer_desc_s *desc);
typedef void (*index_buffer_dtor_f)(struct index_buffer_s *buffer);
typedef void (*index_buffer_set_data_f)(struct index_buffer_s *index_buffer, u32 *data, size_t length);
typedef void (*index_buffer_bind_f)(struct index_buffer_s *buffer);
typedef void (*index_buffer_unbind_f)(struct index_buffer_s *buffer);

/* Vertex buffer */
typedef struct vertex_buffer_s *(*vertex_buffer_new_f)(void);
typedef b8 (*vertex_buffer_ctor_f)(struct vertex_buffer_s *vertex_buffer, buffer_desc_s *desc);
typedef void (*vertex_buffer_dtor_f)(struct vertex_buffer_s *vertex_buffer);
typedef void (*vertex_buffer_set_pointer_f)(struct vertex_buffer_s *vertex_buffer, attribute_desc_s *attributes,
                                            size_t length);
typedef void (*vertex_buffer_set_data_f)(struct vertex_buffer_s *vertex_buffer, const void *data, size_t length);
typedef void (*vertex_buffer_bind_f)(struct vertex_buffer_s *vertex_buffer, u32 *locations, size_t length);
typedef void (*vertex_buffer_unbind_f)(struct vertex_buffer_s *vertex_buffer, u32 *locations, size_t length);

/* Texture */
typedef struct texture_s *(*texture_new_f)(void);
typedef b8 (*texture_ctor_f)(struct texture_s *texture, sm_texture_resource_handler_s handler);
typedef void (*texture_dtor_f)(struct texture_s *texture);
typedef void (*texture_bind_f)(struct texture_s const *texture, u32 tex_index);
typedef void (*texture_unbind_f)(struct texture_s const *texture, u32 tex_index);

typedef void (*clear_f)(buffer_bit_e mask);
typedef void (*clear_color_f)(f32 r, f32 g, f32 b, f32 a);
typedef void (*set_viewport_f)(u32 x, u32 y, u32 width, u32 height);
typedef void (*draw_indexed_f)(u32 index_count);
typedef void (*enable_f)(enable_flags_e mask);
typedef b8 (*loader_f)(loadproc_f load);

/* The scene_s "class" has three behavior functions. The draw_f, do_f and get_look_at_f functions
 * are supposed to be polymorphic and can be overridden by the child classes. */
typedef struct {

  /* api will works like a runtime type check */
  device_api_e device_api;

  loader_f loader;

  clear_f clear;
  clear_color_f clear_color;
  set_viewport_f set_viewport;
  draw_indexed_f draw_indexed;

  enable_f enable;

  /* Shader */
  shader_new_f shader_new;
  shader_ctor_f shader_ctor;
  shader_dtor_f shader_dtor;
  shader_bind_f shader_bind;
  shader_unbind_f shader_unbind;
  shader_set_uniform_f shader_set_uniform;
  shader_set_uniform_array_f *shader_set_uniform_array;
  shader_get_location_f shader_get_location;
  shader_get_type_f shader_get_type;

  /* Index buffer */
  index_buffer_new_f index_buffer_new;
  index_buffer_ctor_f index_buffer_ctor;
  index_buffer_dtor_f index_buffer_dtor;
  index_buffer_set_data_f index_buffer_set_data;
  index_buffer_bind_f index_buffer_bind;
  index_buffer_unbind_f index_buffer_unbind;

  /* Vertex buffer */
  vertex_buffer_new_f vertex_buffer_new;
  vertex_buffer_ctor_f vertex_buffer_ctor;
  vertex_buffer_dtor_f vertex_buffer_dtor;
  vertex_buffer_set_pointer_f vertex_buffer_set_pointer;
  vertex_buffer_set_data_f vertex_buffer_set_data;
  vertex_buffer_bind_f vertex_buffer_bind;
  vertex_buffer_unbind_f vertex_buffer_unbind;

  /* Texture */
  texture_new_f texture_new;
  texture_ctor_f texture_ctor;
  texture_dtor_f texture_dtor;
  texture_bind_f texture_bind;
  texture_unbind_f texture_unbind;

} device_s;

extern device_s *DEVICE;

b8 device_init(device_api_e api);
void device_teardown(void);

#endif /* SM_DEVICE_PUB_H */
