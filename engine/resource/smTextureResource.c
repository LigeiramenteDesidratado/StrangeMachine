#include "smpch.h"

#include "core/smCore.h"

#include "core/data/smArray.h"
#include "core/smPool.h"

#include "renderer/api/smTypes.h"
#include "renderer/smDevicePub.h"

#include "resource/smTextureResourcePub.h"

#include "vendor/stbi/stb_image.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "RES. TEXTURE"

typedef struct {
  char *name;
  uint32_t width, height;
  uint32_t channels;
  uint8_t *cpu_data;
  struct texture_s *gpu_data; /* TODO: the type can be a bit anbiguous  */

} texture_s;

typedef struct texture_resource_s {

  struct handle_pool_s *handle_pool;
  texture_s *textures;
  const device_s *device_reference; /* TODO: renderer context */

} texture_resource_s;

texture_resource_s *TEXTURE_RESOURCE = NULL;

bool texture_res_init(size_t capacity) {

  SM_ASSERT(TEXTURE_RESOURCE == NULL && "TEXTURE_RESOURCE already initialized");

  TEXTURE_RESOURCE = SM_CALLOC(1, sizeof(texture_resource_s));
  SM_ASSERT(TEXTURE_RESOURCE);

  struct handle_pool_s *handle_pool = pool_new();
  if (!pool_ctor(handle_pool, capacity)) {
    SM_LOG_ERROR("failed to create texture handle pool");
    return false;
  }
  TEXTURE_RESOURCE->handle_pool = handle_pool;

  SM_ARRAY_NEW(TEXTURE_RESOURCE->textures, capacity);
  SM_ARRAY_SET_SIZE(TEXTURE_RESOURCE->textures, capacity);
  memset(TEXTURE_RESOURCE->textures, 0x0, sizeof(texture_s) * capacity);

  return true;
}

void texture_res_teardown(void) {

  SM_ASSERT(TEXTURE_RESOURCE);

  for (size_t i = 0; i < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures); i++) {
    texture_s *texture = &TEXTURE_RESOURCE->textures[i];
    if (texture->name)
      free(texture->name);
    if (texture->cpu_data) {
      stbi_image_free(texture->cpu_data);
      texture->cpu_data = NULL;
    }
  }

  SM_ARRAY_DTOR(TEXTURE_RESOURCE->textures);
  pool_dtor(TEXTURE_RESOURCE->handle_pool);
  SM_FREE(TEXTURE_RESOURCE);
}

texture_handler_s texture_res_new(const char *file) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(file);

  if (handle_full(TEXTURE_RESOURCE->handle_pool)) {
    SM_LOG_ERROR("texture handle pool is full");
    return (texture_handler_s){SM_INVALID_HANDLE};
  }

  uint32_t handle = handle_new(TEXTURE_RESOURCE->handle_pool);
  if (handle == SM_INVALID_HANDLE) {
    SM_LOG_ERROR("failed to add texture");
    return (texture_handler_s){SM_INVALID_HANDLE};
  }

  texture_s *texture = &TEXTURE_RESOURCE->textures[sm_handle_index(handle)];

  int32_t width, height, channels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  uint8_t *data = stbi_load(file, &width, &height, &channels, 4);
  if (data == NULL) {
    SM_LOG_ERROR("[%s] failed to load image", file);
    return (texture_handler_s){SM_INVALID_HANDLE};
  }
  SM_LOG_INFO("[%s] image successfully loaded", file);

  texture->name = strdup(file);
  texture->width = width;
  texture->height = height;
  texture->channels = channels;
  texture->cpu_data = data;

  return (texture_handler_s){handle};
}

void texture_res_dtor(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  texture_res_unload_cpu_data(handler);
  /* texture_unload_gpu_data(handler, device); */
  handle_del(TEXTURE_RESOURCE->handle_pool, handler.handle);
}

uint32_t texture_res_get_width(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->width;
}

uint32_t texture_res_get_height(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->height;
}

uint32_t texture_res_get_channels(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->channels;
}

const char *texture_res_get_name(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->name;
}

void texture_res_unload_cpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  stbi_image_free(texture->cpu_data);
  texture->cpu_data = NULL;
}

void texture_res_unload_gpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));
  SM_ASSERT(TEXTURE_RESOURCE->device_reference && "device reference not set");

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->gpu_data == NULL) {
    return;
  }

  TEXTURE_RESOURCE->device_reference->texture_dtor(texture->gpu_data);
  texture->gpu_data = NULL;
}

bool texture_res_has_cpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  return texture->cpu_data != NULL;
}

bool texture_res_has_gpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  return texture->gpu_data != NULL;
}

bool texture_res_gpu_load_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));
  SM_ASSERT(TEXTURE_RESOURCE->device_reference && "device reference not set");

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->gpu_data)
    return true;

  if (!texture->cpu_data) {
    SM_LOG_ERROR("[%s] texture has no data", texture->name);
    return false;
  }

  struct texture_s *gpu_texture = TEXTURE_RESOURCE->device_reference->texture_new();
  if (!TEXTURE_RESOURCE->device_reference->texture_ctor(gpu_texture, texture->width, texture->height,
                                                        texture->cpu_data)) {
    SM_LOG_ERROR("[%s] failed to upload texture to GPU", texture->name);
    return false;
  }
  texture->gpu_data = gpu_texture;

  SM_LOG_INFO("[%s] texture successfully uploaded to GPU", texture->name);

  return true;
}

void texture_res_bind(texture_handler_s handler, uint32_t tex_index) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (!texture->gpu_data) {
    if (!texture_res_gpu_load_data(handler)) {
      SM_LOG_ERROR("[%s] failed to load texture to GPU", texture->name);
      return;
    }
  }

  TEXTURE_RESOURCE->device_reference->texture_bind(texture->gpu_data, tex_index);
}

void texture_res_unbind(texture_handler_s handler, uint32_t tex_index) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(TEXTURE_RESOURCE->device_reference && "device reference not set");
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_SIZE(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  TEXTURE_RESOURCE->device_reference->texture_unbind(texture->gpu_data, tex_index);
}

void texture_res_set_device(const device_s *device) {

  SM_ASSERT(TEXTURE_RESOURCE);

  TEXTURE_RESOURCE->device_reference = device;
}

#undef SM_MODULE_NAME
