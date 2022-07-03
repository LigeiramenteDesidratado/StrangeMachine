#ifndef SM_SHADER_RESOURCE_H
#define SM_SHADER_RESOURCE_H

#include "smpch.h"

#include "core/util/smString.h"

typedef struct sm__shader_handler_s {
  u32 handle;

} sm_shader_resource_handler_s;

b8 sm_shader_resource_init(size_t capacity);
void sm_shader_resource_teardown();

sm_shader_resource_handler_s sm_shader_resource_new(sm_string shader_name);
void sm_shader_resource_unload_data(sm_shader_resource_handler_s handler);
b8 sm_shader_resource_has_data(sm_shader_resource_handler_s handler);
sm_string sm_shader_resource_get_fragment_data(sm_shader_resource_handler_s handler);
sm_string sm_shader_resource_get_vertex_data(sm_shader_resource_handler_s handler);

#endif /* SM_SHADER_RESOURCE_H */
