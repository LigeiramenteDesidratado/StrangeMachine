#include "smpch.h"

#include "core/smCore.h"

#include "resource/smResource.h"
#include "resource/smTextureResourcePub.h"

#include "vendor/stbi/stb_image.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "TEXTURE RES."

typedef struct sm__texture_s {
  sm_string name;
  u32 width, height;
  u32 channels;
  void *data;

} sm_texture_s;

typedef struct sm__texture_resource_s {

  struct handle_pool_s *handle_pool;
  SM_ARRAY(sm_texture_s) textures;

} sm_texture_resource_s;

/* Globals */
sm_texture_resource_s *TEXTURE_RESOURCE = NULL;

b8 sm_texture_resource_init(size_t capacity) {

  SM_ASSERT(TEXTURE_RESOURCE == NULL && "TEXTURE_RESOURCE already initialized");

  TEXTURE_RESOURCE = SM_CALLOC(1, sizeof(sm_texture_resource_s));
  SM_ASSERT(TEXTURE_RESOURCE);

  struct handle_pool_s *handle_pool = sm_handle_pool_new();
  if (!sm_handle_pool_ctor(handle_pool, capacity)) {
    SM_LOG_ERROR("failed to create texture handle pool");
    return false;
  }
  TEXTURE_RESOURCE->handle_pool = handle_pool;

  SM_ARRAY_CTOR(TEXTURE_RESOURCE->textures, capacity);
  SM_ARRAY_SET_LEN(TEXTURE_RESOURCE->textures, capacity);
  memset(TEXTURE_RESOURCE->textures, 0x0, sizeof(sm_texture_s) * capacity);

  stbi_set_flip_vertically_on_load(true); /* tell stb_image.h to flip loaded texture's on the y-axis. */

  return true;
}

void sm_texture_resource_teardown(void) {

  SM_ASSERT(TEXTURE_RESOURCE);

  for (size_t i = 0; i < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures); i++) {
    sm_texture_s *texture = &TEXTURE_RESOURCE->textures[i];
    if (texture->name.str)
      sm_string_dtor(texture->name);
    if (texture->data) {
      stbi_image_free(texture->data);
      texture->data = NULL;
    }
  }

  SM_ARRAY_DTOR(TEXTURE_RESOURCE->textures);
  sm_handle_pool_dtor(TEXTURE_RESOURCE->handle_pool);
  SM_FREE(TEXTURE_RESOURCE);
}

sm_texture_resource_handler_s sm_texture_resource_new(sm_string file) {

  SM_ASSERT(TEXTURE_RESOURCE);

  if (sm_handle_full(TEXTURE_RESOURCE->handle_pool)) {
    SM_LOG_ERROR("texture handle pool is full");
    return (sm_texture_resource_handler_s){SM_INVALID_HANDLE};
  }

  sm_resource_s *res = sm_resource_manager_get(file);
  if (!res) {
    SM_LOG_WARN("[%s] resource not found", file.str);
    return (sm_texture_resource_handler_s){SM_INVALID_HANDLE};
  }
  SM_ASSERT(SM_MASK_CHK(res->type, RESOURCE_TYPE_TEXTURE) && "resource is not a texture");

  if (res->handle != SM_INVALID_HANDLE) {
    return (sm_texture_resource_handler_s){res->handle};
  }

  u32 handle = sm_handle_new(TEXTURE_RESOURCE->handle_pool);
  if (handle == SM_INVALID_HANDLE) {
    SM_LOG_ERROR("failed to create texture handle");
    return (sm_texture_resource_handler_s){SM_INVALID_HANDLE};
  }

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[sm_handle_index(handle)];

  i32 width, height, channels, ok = 0;
  ok = stbi_info(file.str, &width, &height, &channels);
  if (!ok) {
    SM_LOG_ERROR("[%s] failed to load image from disk", file.str);
    sm_handle_del(TEXTURE_RESOURCE->handle_pool, handle);
    return (sm_texture_resource_handler_s){SM_INVALID_HANDLE};
  }
  SM_LOG_TRACE("[%s] (%dx%d:%d) image successfully loaded", file.str, width, height, channels);

  texture->name = sm_string_reference(file);
  texture->width = width;
  texture->height = height;
  texture->channels = channels;

  res->handle = handle;

  return (sm_texture_resource_handler_s){handle};
}

b8 sm_texture_resource_load_data(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  if (texture->data) {
    SM_LOG_WARN("[%s] texture already has cpu data", texture->name.str);
    return false;
  }

  i32 width, height, channels;
  u8 *data = stbi_load(texture->name.str, &width, &height, &channels, 4);
  if (data == NULL) {
    SM_LOG_ERROR("[%s] failed to load texture", texture->name.str);
    return false;
  }

  texture->width = width;
  texture->height = height;
  texture->channels = channels;
  texture->data = data;

  SM_LOG_TRACE("[%s] texture successfully loaded from disk to main memory", texture->name.str);
  return true;
}

/*
 * Unload texture from GPU and CPU memory
 * but keep the handle in the handle pool
 */
void sm_texture_resource_unload_data(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  stbi_image_free(texture->data);
  texture->data = NULL;
}

/*
 * Unload texture from GPU and CPU memory
 * and remove the handle from the handle pool
 */
void sm_texture_resource_dtor(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];

  texture->name.str = sm_string_dtor(texture->name);
  texture->channels = 0;
  texture->height = 0;
  texture->width = 0;

  sm_texture_resource_unload_data(handler);
  sm_handle_del(TEXTURE_RESOURCE->handle_pool, handler.handle);
}

u32 sm_texture_resource_get_width(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->width;
}

u32 sm_texture_resource_get_height(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->height;
}

u32 sm_texture_resource_get_channels(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->channels;
}

sm_string sm_texture_resource_get_name(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->name;
}

const void *sm_texture_resource_get_data(sm_texture_resource_handler_s handler) {

  SM_ASSERT(TEXTURE_RESOURCE);
  SM_ASSERT(sm_handle_valid(TEXTURE_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(TEXTURE_RESOURCE->textures));

  sm_texture_s *texture = &TEXTURE_RESOURCE->textures[index];
  return texture->data;
}

b8 sm_texture_resource_has_data(sm_texture_resource_handler_s handler) {

  return sm_texture_resource_get_data(handler) != NULL;
}

#undef SM_MODULE_NAME
