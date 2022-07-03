#include "smpch.h"

#include "core/smCore.h"

#include "renderer/api/smDeviceAPIs.h"
#include "renderer/smDevicePub.h"

device_s *DEVICE = NULL;

bool device_init(device_api_e api) {

  SM_ASSERT(!DEVICE && "Device already initialized");

  DEVICE = SM_CALLOC(1, sizeof(device_s));
  SM_ASSERT(DEVICE);

  DEVICE->device_api = api;

  switch (DEVICE->device_api) {
  case OPENGL21: {

    DEVICE->loader = GL21loader;

    DEVICE->clear_color = GL21clear_color;
    DEVICE->clear = GL21clear;
    DEVICE->set_viewport = GL21_viewport;
    DEVICE->draw_indexed = GL21draw_indexed;

    DEVICE->enable = GL21enable;

    DEVICE->shader_new = GL21shader_new;
    DEVICE->shader_ctor = GL21shader_ctor;
    DEVICE->shader_dtor = GL21shader_dtor;
    DEVICE->shader_bind = GL21shader_bind;
    DEVICE->shader_unbind = GL21shader_unbind;
    DEVICE->shader_set_uniform = GL21shader_set_uniform;
    DEVICE->shader_get_location = GL21shader_get_location;
    DEVICE->shader_get_type = GL21shader_get_type;

    DEVICE->index_buffer_new = GL21index_buffer_new;
    DEVICE->index_buffer_ctor = GL21index_buffer_ctor;
    DEVICE->index_buffer_dtor = GL21index_buffer_dtor;
    DEVICE->index_buffer_set_data = GL21index_buffer_set_data;
    DEVICE->index_buffer_bind = GL21index_buffer_bind;
    DEVICE->index_buffer_unbind = GL21index_buffer_unbind;

    DEVICE->vertex_buffer_new = GL21vertex_buffer_new;
    DEVICE->vertex_buffer_ctor = GL21vertex_buffer_ctor;
    DEVICE->vertex_buffer_dtor = GL21vertex_buffer_dtor;
    DEVICE->vertex_buffer_set_pointer = GL21vertex_buffer_set_pointer;
    DEVICE->vertex_buffer_set_data = GL21vertex_buffer_set_data;
    DEVICE->vertex_buffer_bind = GL21vertex_buffer_bind;
    DEVICE->vertex_buffer_unbind = GL21vertex_buffer_unbind;

    DEVICE->texture_new = GL21texture_new;
    DEVICE->texture_ctor = GL21texture_ctor;
    DEVICE->texture_dtor = GL21texture_dtor;
    DEVICE->texture_bind = GL21texture_bind;
    DEVICE->texture_unbind = GL21texture_unbind;

    break;
  };

  case OPENGL33: /* fallthrough */
  case VULKAN:   /* fallthrough */
  case D3D11:    /* fallthrough */
  default:
    SM_ASSERT(0 && "Not implemented yet");
    break;
  }

  return true;
}

void device_teardown() {

  SM_ASSERT(DEVICE && "Device not initialized");

  SM_FREE(DEVICE);
  DEVICE = NULL;
}
