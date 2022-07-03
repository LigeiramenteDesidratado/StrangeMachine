#include "smpch.h"

#include "resource/smResource.h"

#include "core/smCore.h"

#include "resource/smShaderResource.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "SHADER RES."

typedef struct sm__shader_s {
  sm_string name;

  sm_string vertex_data;
  sm_string fragment_data;

} sm_shader_s;

typedef struct sm__shader_resource_s {

  struct handle_pool_s *handle_pool;
  SM_ARRAY(sm_shader_s) shaders;

} sm_shader_resource_s;

SM_ARRAY(sm_shader_resource_s) SHADER_RESOURCE = NULL;

b8 sm_shader_resource_init(size_t capacity) {

  SM_ASSERT(SHADER_RESOURCE == NULL && "SHADER_RESOURCE already initialized");

  SHADER_RESOURCE = SM_CALLOC(1, sizeof(sm_shader_resource_s));
  struct handle_pool_s *handle_pool = sm_handle_pool_new();
  if (!sm_handle_pool_ctor(handle_pool, capacity)) {
    SM_LOG_ERROR("failed to create shader handle pool");
    return false;
  }

  SHADER_RESOURCE->handle_pool = handle_pool;
  SM_ARRAY_SET_LEN(SHADER_RESOURCE->shaders, capacity);
  memset(SHADER_RESOURCE->shaders, 0x0, sizeof(sm_shader_s) * capacity);

  return true;
}

void sm_shader_resource_teardown() {

  SM_ASSERT(SHADER_RESOURCE != NULL && "SHADER_RESOURCE not initialized");

  for (size_t i = 0; i < SM_ARRAY_LEN(SHADER_RESOURCE->shaders); ++i) {
    sm_shader_s *shader = &SHADER_RESOURCE->shaders[i];
    if (shader->name.str)
      sm_string_dtor(shader->name);
    if (shader->vertex_data.str)
      sm_string_dtor(shader->vertex_data);
    if (shader->fragment_data.str)
      sm_string_dtor(shader->fragment_data);
  }

  SM_ARRAY_DTOR(SHADER_RESOURCE->shaders);
  sm_handle_pool_dtor(SHADER_RESOURCE->handle_pool);
  SM_FREE(SHADER_RESOURCE);
}

sm_shader_resource_handler_s sm_shader_resource_new(sm_string shader_name) {

  SM_ASSERT(SHADER_RESOURCE != NULL && "SHADER_RESOURCE not initialized");

  if (sm_handle_full(SHADER_RESOURCE->handle_pool)) {
    SM_LOG_ERROR("shader handle pool is full");
    return (sm_shader_resource_handler_s){SM_INVALID_HANDLE};
  }

  sm_resource_s *res = sm_resource_manager_get(shader_name);
  if (!res) {
    SM_LOG_WARN("[%s] resource not found", shader_name.str);
    return (sm_shader_resource_handler_s){SM_INVALID_HANDLE};
  }

  SM_ASSERT(SM_MASK_CHK(res->type, RESOURCE_TYPE_SHADER) && "resource is not a shader");

  if (res->handle != SM_INVALID_HANDLE) {
    return (sm_shader_resource_handler_s){res->handle};
  }

  u32 handle = sm_handle_new(SHADER_RESOURCE->handle_pool);
  if (handle == SM_INVALID_HANDLE) {
    SM_LOG_ERROR("failed to create shader handle");
    return (sm_shader_resource_handler_s){SM_INVALID_HANDLE};
  }

  sm_shader_s *shader = &SHADER_RESOURCE->shaders[sm_handle_index(handle)];

  shader->name = sm_string_reference(shader_name);

  sm_file_handle_s out_handle;
  if (!sm_filesystem_open(shader->name, SM_FILE_MODE_READ, false, &out_handle)) {
    SM_LOG_ERROR("[%s] failed to open shader file", shader->name.str);
    return (sm_shader_resource_handler_s){SM_INVALID_HANDLE};
  }

  sm_string line = sm_string_ctor(256);
  size_t file_size = sm_filesystem_size(&out_handle);
  char *buffer = SM_MALLOC(sizeof(char) * file_size);
  memset(buffer, 0x0, sizeof(char) * file_size);
  while (sm_filesystem_read_line(&out_handle, line) && !sm_string_eq_c_str(line, "// SM FRAGMENT\n"))
    buffer = strcat(buffer, line.str);

  shader->vertex_data = sm_string_from(buffer);
  memset(buffer, 0x0, sizeof(char) * file_size);

  while (sm_filesystem_read_line(&out_handle, line))
    buffer = strcat(buffer, line.str);

  shader->fragment_data = sm_string_from(buffer);

  /* printf("vertex:\n%s\n", shader->vertex_source.str); */
  /* printf("fragment:\n%s\n", shader->fragment_source.str); */

  sm_string_dtor(line);
  SM_FREE(buffer);

  res->handle = handle;
  return (sm_shader_resource_handler_s){handle};
}

void sm_shader_resource_unload_data(sm_shader_resource_handler_s handler) {

  SM_ASSERT(SHADER_RESOURCE);
  SM_ASSERT(sm_handle_valid(SHADER_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(SHADER_RESOURCE->shaders));

  sm_shader_s *shader = &SHADER_RESOURCE->shaders[index];

  if (shader->fragment_data.str)
    shader->fragment_data.str = sm_string_dtor(shader->fragment_data);
  if (shader->vertex_data.str)
    shader->vertex_data.str = sm_string_dtor(shader->vertex_data);
}

b8 sm_shader_resource_has_data(sm_shader_resource_handler_s handler) {

  SM_ASSERT(SHADER_RESOURCE);
  SM_ASSERT(sm_handle_valid(SHADER_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(SHADER_RESOURCE->shaders));

  sm_shader_s *shader = &SHADER_RESOURCE->shaders[index];

  return shader->fragment_data.str && shader->vertex_data.str;
}

sm_string sm_shader_resource_get_vertex_data(sm_shader_resource_handler_s handler) {

  SM_ASSERT(SHADER_RESOURCE);
  SM_ASSERT(sm_handle_valid(SHADER_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(SHADER_RESOURCE->shaders));

  sm_shader_s *shader = &SHADER_RESOURCE->shaders[index];

  return sm_string_reference(shader->vertex_data);
}

sm_string sm_shader_resource_get_fragment_data(sm_shader_resource_handler_s handler) {

  SM_ASSERT(SHADER_RESOURCE);
  SM_ASSERT(sm_handle_valid(SHADER_RESOURCE->handle_pool, handler.handle));

  u32 index = sm_handle_index(handler.handle);
  SM_ASSERT(index < SM_ARRAY_LEN(SHADER_RESOURCE->shaders));

  sm_shader_s *shader = &SHADER_RESOURCE->shaders[index];

  return sm_string_reference(shader->fragment_data);
}

#undef SM_MODULE_NAME
