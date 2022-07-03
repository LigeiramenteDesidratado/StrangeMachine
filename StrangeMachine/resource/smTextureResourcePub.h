#ifndef SM_TEXTURE_RESOURCE_PUBLIC_H
#define SM_TEXTURE_RESOURCE_PUBLIC_H

#include "smpch.h"

#include "core/util/smString.h"

typedef struct {
  u32 handle;

} sm_texture_resource_handler_s;

sm_texture_resource_handler_s sm_texture_resource_new(sm_string file);
void sm_texture_resource_dtor(sm_texture_resource_handler_s handler);

sm_string sm_texture_resource_get_name(sm_texture_resource_handler_s handler);
u32 sm_texture_resource_get_channels(sm_texture_resource_handler_s handler);
u32 sm_texture_resource_get_height(sm_texture_resource_handler_s handler);
u32 sm_texture_resource_get_width(sm_texture_resource_handler_s handler);
const void *sm_texture_resource_get_data(sm_texture_resource_handler_s handler);

b8 sm_texture_resource_load_data(sm_texture_resource_handler_s handler);
b8 sm_texture_resource_has_data(sm_texture_resource_handler_s handler);

#endif /* SM_TEXTURE_RESOURCE_PUBLIC_H */
