#include "smpch.h"

#include "core/smCore.h"

#include "renderer/api/smDeviceAPIs.h"
#include "renderer/smDevicePub.h"

bool device_ctor(device_s *device, device_api_e api) {

  SM_ASSERT(device);

  device->device_api = api;

  switch (device->device_api) {
  case OPENGL21:

    device->clear_color = GL21clear_color;
    device->clear = GL21clear;
    device->set_viewport = GL21set_viewport;
    device->draw_indexed = GL21draw_indexed;

    device->shader_new = GL21shader_new;
    device->shader_ctor = GL21shader_ctor;
    device->shader_dtor = GL21shader_dtor;
    device->shader_bind = GL21shader_bind;
    device->shader_unbind = GL21shader_unbind;
    device->shader_set_uniform = GL21shader_set_uniform;

    device->index_buffer_new = GL21index_buffer_new;
    device->index_buffer_ctor = GL21index_buffer_ctor;
    device->index_buffer_dtor = GL21index_buffer_dtor;
    device->index_buffer_set_data = GL21index_buffer_set_data;
    device->index_buffer_bind = GL21index_buffer_bind;
    device->index_buffer_unbind = GL21index_buffer_unbind;

    device->vertex_buffer_new = GL21vertex_buffer_new;
    device->vertex_buffer_ctor = GL21vertex_buffer_ctor;
    device->vertex_buffer_dtor = GL21vertex_buffer_dtor;
    device->vertex_buffer_set_pointer = GL21vertex_buffer_set_pointer;
    device->vertex_buffer_set_data = GL21vertex_buffer_set_data;
    device->vertex_buffer_bind = GL21vertex_buffer_bind;
    device->vertex_buffer_unbind = GL21vertex_buffer_unbind;

    device->texture_new = GL21texture_new;
    device->texture_ctor = GL21texture_ctor;
    device->texture_dtor = GL21texture_dtor;
    device->texture_bind = GL21texture_bind;
    device->texture_unbind = GL21texture_unbind;

    break;

  case OPENGL33: /* fallthrough */
  case VULKAN:   /* fallthrough */
  case D3D11:    /* fallthrough */
  default:
    SM_ASSERT(0 && "Not implemented yet");
    break;
  }

  return true;
}
