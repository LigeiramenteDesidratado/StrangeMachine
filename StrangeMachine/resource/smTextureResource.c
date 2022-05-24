#include "smpch.h"

#include "core/smCore.h"
#include "core/smHandle.h"
#include "core/smPool.h"

#include "core/data/smArray.h"

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
  SM_ARRAY(texture_s) textures;

} texture_resource_s;

/* Globals */
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

  SM_ARRAY_CTOR(TEXTURE_RESOURCE->textures, capacity);
  SM_ARRAY_SET_LEN(TEXTURE_RESOURCE->textures, capacity);
  memset(TEXTURE_RESOURCE->textures, 0x0, sizeof(texture_s) * capacity);

  stbi_set_flip_vertically_on_load(true); /* tell stb_image.h to flip loaded texture's on the y-axis. */

  return true;
}

void texture_res_teardown(void) {

  SM_ASSERT(TEXTURE_RESOURCE);

  for (size_t i = 0; i < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures); i++) {
    texture_s *texture = &TEXTURE_RESOURCE->textures[i];
    if (texture->name)
      free(texture->name);
    if (texture->cpu_data) {
      stbi_image_free(texture->cpu_data);
      texture->cpu_data = NULL;
    }
    if (texture->gpu_data) {
      DEVICE->texture_dtor(texture->gpu_data);
      texture->gpu_data = NULL;
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
    SM_LOG_ERROR("failed to create texture handle");
    return (texture_handler_s){SM_INVALID_HANDLE};
  }

  texture_s *texture = &TEXTURE_RESOURCE->textures[sm_handle_index(handle)];

  int32_t width, height, channels, ok = 0;
  ok = stbi_info(file, &width, &height, &channels);
  if (!ok) {
    SM_LOG_ERROR("[%s] failed to load image from disk", file);
    handle_del(TEXTURE_RESOURCE->handle_pool, handle);
    return (texture_handler_s){SM_INVALID_HANDLE};
  }
  SM_LOG_TRACE("[%s] (%dx%d:%d) image successfully loaded", file, width, height, channels);

  texture->name = strdup(file);
  texture->width = width;
  texture->height = height;
  texture->channels = channels;

  return (texture_handler_s){handle};
}

/*
 * Unload texture from GPU and CPU memory
 * but keep the handle in the handle pool
 */
void texture_res_unload_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  texture_res_unload_gpu_data(handler);
  texture_res_unload_cpu_data(handler);
}

/*
 * Unload texture from GPU and CPU memory
 * and remove the handle from the handle pool
 */
void texture_res_dtor(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  texture_res_unload_gpu_data(handler);
  texture_res_unload_cpu_data(handler);
  handle_del(TEXTURE_RESOURCE->handle_pool, handler.handle);
}

uint32_t texture_res_get_width(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->width;
}

uint32_t texture_res_get_height(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->height;
}

uint32_t texture_res_get_channels(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->channels;
}

const char *texture_res_get_name(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->name;
}

/*
 * Unload texture from CPU memory only
 * but keep the handle in the handle pool
 */
void texture_res_unload_cpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  stbi_image_free(texture->cpu_data);
  texture->cpu_data = NULL;
}

/*
 * Unload texture from GPU memory only
 * but keep the handle in the handle pool
 */

void texture_res_unload_gpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->gpu_data == NULL) {
    return;
  }

  DEVICE->texture_dtor(texture->gpu_data);
  texture->gpu_data = NULL;
}

bool texture_res_has_cpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  return texture->cpu_data != NULL;
}

bool texture_res_has_gpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  return texture->gpu_data != NULL;
}

/*
 * Load texture into GPU memory
 */
bool texture_res_load_gpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->gpu_data)
    return true;

  if (!texture->cpu_data) {
    texture_res_load_cpu_data(handler);
    if (!texture->cpu_data)
      return false;
  }

  struct texture_s *gpu_texture = DEVICE->texture_new();
  if (!DEVICE->texture_ctor(gpu_texture, texture->width, texture->height, texture->cpu_data)) {
    SM_LOG_ERROR("[%s] failed to upload texture to GPU", texture->name);
    return false;
  }
  texture->gpu_data = gpu_texture;

  SM_LOG_TRACE("[%s] texture successfully uploaded to GPU", texture->name);

  return true;
}

/*
 * Load texture into CPU memory
 */
void texture_res_load_cpu_data(texture_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->cpu_data) {
    SM_LOG_WARN("[%s] texture already has cpu data", texture->name);
    return;
  }

  int32_t width, height, channels;
  uint8_t *data = stbi_load(texture->name, &width, &height, &channels, 4);
  if (data == NULL) {
    SM_LOG_ERROR("[%s] failed to load texture", texture->name);
    return;
  }

  texture->width = width;
  texture->height = height;
  texture->channels = channels;
  texture->cpu_data = data;

  SM_LOG_TRACE("[%s] texture successfully loaded from disk to main memory", texture->name);
}

void texture_res_bind(texture_handler_s handler, uint32_t tex_index) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (!texture->gpu_data) {
    if (!texture_res_load_gpu_data(handler)) {
      SM_LOG_ERROR("[%s] failed to load texture to GPU", texture->name);
      return;
    }
  }

  DEVICE->texture_bind(texture->gpu_data, tex_index);
}

void texture_res_unbind(texture_handler_s handler, uint32_t tex_index) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  uint32_t index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  DEVICE->texture_unbind(texture->gpu_data, tex_index);
}

#undef SM_MODULE_NAME
