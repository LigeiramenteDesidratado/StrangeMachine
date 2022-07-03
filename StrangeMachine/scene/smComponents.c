#include "smpch.h"

#include "core/smCore.h"

#include "scene/smComponents.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "COMPONENTS"

const sm_component_t SM_TRANSFORM_COMP = 1ULL << 0;
const sm_component_t SM_VELOCITY_COMP = 1ULL << 1;
const sm_component_t SM_FORCE_COMP = 1ULL << 2;
const sm_component_t SM_SPEED_COMP = 1ULL << 3;
const sm_component_t SM_MESH_COMP = 1ULL << 4;
const sm_component_t SM_CAMERA_COMP = 1ULL << 5;
const sm_component_t SM_ALL_COMP =
    SM_TRANSFORM_COMP | SM_VELOCITY_COMP | SM_FORCE_COMP | SM_SPEED_COMP | SM_MESH_COMP | SM_CAMERA_COMP;

static int initialized = 0;
SM_ARRAY(sm_component_view_s) COMPONENTS = NULL;

void component_init(void) {

  if (initialized++ > 0) {
    return;
  }

  SM_ASSERT(COMPONENTS == NULL && "Component system already initialized");

  sm_component_view_s transform_view = {.name = sm_string_from("transform"),
                                        .size = sizeof(sm_transform_s),
                                        .alligned = false,
                                        .id = SM_TRANSFORM_COMP,
                                        .write = sm_transform_write,
                                        .read = sm_transform_read,
                                        .dtor = NULL};

  sm_component_view_s vel_view = {.name = sm_string_from("velocity"),
                                  .size = sizeof(sm_velocity_s),
                                  .alligned = false,
                                  .id = SM_VELOCITY_COMP,
                                  .write = sm_vec3_write,
                                  .read = sm_vec3_read,
                                  .dtor = NULL};

  sm_component_view_s force_view = {.name = sm_string_from("force"),
                                    .size = sizeof(sm_force_s),
                                    .alligned = false,
                                    .id = SM_FORCE_COMP,
                                    .write = sm_vec3_write,
                                    .read = sm_vec3_read,
                                    .dtor = NULL};

  sm_component_view_s speed_view = {.name = sm_string_from("speed"),
                                    .size = sizeof(sm_speed_s),
                                    .alligned = false,
                                    .id = SM_SPEED_COMP,
                                    .write = sm_speed_write,
                                    .read = sm_speed_read,
                                    .dtor = NULL};

  sm_component_view_s mesh_view = {.name = sm_string_from("mesh"),
                                   .size = sizeof(sm_mesh_s),
                                   .alligned = false,
                                   .id = SM_MESH_COMP,
                                   .write = sm_mesh_write,
                                   .read = sm_mesh_read,
                                   .dtor = sm_mesh_dtor};

  sm_component_view_s camera_view = {.name = sm_string_from("camera"),
                                     .size = sizeof(sm_camera_s),
                                     .alligned = false,
                                     .id = SM_CAMERA_COMP,
                                     .write = sm_camera_write,
                                     .read = sm_camera_read,
                                     .dtor = NULL};

  SM_ARRAY_PUSH(COMPONENTS, transform_view);
  SM_ARRAY_PUSH(COMPONENTS, vel_view);
  SM_ARRAY_PUSH(COMPONENTS, force_view);
  SM_ARRAY_PUSH(COMPONENTS, speed_view);
  SM_ARRAY_PUSH(COMPONENTS, mesh_view);
  SM_ARRAY_PUSH(COMPONENTS, camera_view);
}

void component_register_component(sm_component_view_s *desc) {

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");
  SM_ASSERT(desc != NULL);
  SM_ASSERT(desc->id != 0);

#ifdef SM_DEBUG
  for (size_t i = 0; i < SM_ARRAY_LEN(COMPONENTS); i++) {
    SM_ASSERT(COMPONENTS[i].id != desc->id && "Component already registered");
  }
#endif

  SM_ARRAY_PUSH(COMPONENTS, *desc);
}

const sm_component_view_s *component_get_desc(sm_component_t id) {

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");

  for (size_t i = 0; i < SM_ARRAY_LEN(COMPONENTS); i++) {
    if (COMPONENTS[i].id == id) {
      return &COMPONENTS[i];
    }
  }

  return NULL;
}

void component_teardown(void) {

  if (--initialized > 0) {
    return;
  }

  SM_LOG_TRACE("Component system teardown");

  SM_ASSERT(COMPONENTS != NULL && "Component system not initialized");

  for (size_t i = 0; i < SM_ARRAY_LEN(COMPONENTS); i++) {
    if (COMPONENTS[i].name.str) {
      sm_string_dtor(COMPONENTS[i].name);
    }
  }

  SM_ARRAY_DTOR(COMPONENTS);

  COMPONENTS = NULL;
}

b8 sm_mesh_write(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_mesh_s *mesh = (sm_mesh_s *)ptr;
  sm_file_handle_s *fh = (sm_file_handle_s *)handle;
  SM_LOG_TRACE("Writing mesh...");

  size_t len = 0, size = 0;

  /* Write positions */
  len = SM_ARRAY_LEN(mesh->positions);
  size = sizeof(*mesh->positions);
  SM_LOG_TRACE("positions: len: %lu, size: %lu", len, size);
  if (!sm_filesystem_write_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the length of positions");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the size of positions");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, mesh->positions, len * size)) {
    SM_LOG_ERROR("[s] failed to write positions");
    return false;
  }

  /* Write UVs */
  len = SM_ARRAY_LEN(mesh->uvs);
  size = sizeof(*mesh->uvs);
  SM_LOG_TRACE("uvs: len: %lu, size: %lu", len, size);
  if (!sm_filesystem_write_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the length of uvs");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the size of uvs");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, mesh->uvs, len * size)) {
    SM_LOG_ERROR("[s] failed to write uvs");
    return false;
  }

  /* Write colors */
  len = SM_ARRAY_LEN(mesh->colors);
  size = sizeof(*mesh->colors);
  SM_LOG_TRACE("colors: len: %lu, size: %lu", len, size);
  if (!sm_filesystem_write_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the length of colors");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the size of colors");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, mesh->colors, len * size)) {
    SM_LOG_ERROR("[s] failed to write colors");
    return false;
  }

  /* Write normals */
  len = SM_ARRAY_LEN(mesh->normals);
  size = sizeof(*mesh->normals);
  SM_LOG_TRACE("normals: len: %lu, size: %lu", len, size);
  if (!sm_filesystem_write_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the length of normals");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the size of normals");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, mesh->normals, len * size)) {
    SM_LOG_ERROR("[s] failed to write normals");
    return false;
  }

  /* Write indices */
  len = SM_ARRAY_LEN(mesh->indices);
  size = sizeof(*mesh->indices);
  SM_LOG_TRACE("indices: len: %lu, size: %lu", len, size);
  if (!sm_filesystem_write_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the length of indices");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to write the size of indices");
    return false;
  }
  if (!sm_filesystem_write_bytes(fh, mesh->indices, len * size)) {
    SM_LOG_ERROR("[s] failed to write indices");
    return false;
  }

  if (!sm_filesystem_write_bytes(fh, &mesh->flags, sizeof(mesh->flags))) {
    SM_LOG_ERROR("[s] failed to write flags");
    return false;
  }

  return true;
}

b8 sm_mesh_read(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_mesh_s *mesh = (sm_mesh_s *)ptr;
  sm_file_handle_s *fh = (sm_file_handle_s *)handle;
  SM_LOG_TRACE("Reading mesh...");

  size_t len = 0, size = 0;

  /* Read positions */
  if (!sm_filesystem_read_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the length of positions");
    return false;
  }
  if (!sm_filesystem_read_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the size of positions");
    return false;
  }
  SM_LOG_TRACE("positions: len: %lu, size: %lu", len, size);
  SM_ARRAY_SET_LEN(mesh->positions, len);
  if (!sm_filesystem_read_bytes(fh, mesh->positions, len * size)) {
    SM_LOG_ERROR("[s] failed to read vertices");
    return false;
  }

  /* Read UVs */
  if (!sm_filesystem_read_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the length of uvs");
    return false;
  }
  if (!sm_filesystem_read_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the size of uvs");
    return false;
  }
  SM_LOG_TRACE("uvs: len: %lu, size: %lu", len, size);
  SM_ARRAY_SET_LEN(mesh->uvs, len);
  if (!sm_filesystem_read_bytes(fh, mesh->uvs, len * size)) {
    SM_LOG_ERROR("[s] failed to read uvs");
    return false;
  }

  /* Read colors */
  if (!sm_filesystem_read_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the length of colors");
    return false;
  }
  if (!sm_filesystem_read_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the size of colors");
    return false;
  }
  SM_LOG_TRACE("colors: len: %lu, size: %lu", len, size);
  SM_ALIGNED_ARRAY_NEW(mesh->colors, 16, len);
  if (!sm_filesystem_read_bytes(fh, mesh->colors, len * size)) {
    SM_LOG_ERROR("[s] failed to read colors");
    return false;
  }

  /* Read normals */
  if (!sm_filesystem_read_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the length of normals");
    return false;
  }
  if (!sm_filesystem_read_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the size of normals");
    return false;
  }
  SM_LOG_TRACE("normals: len: %lu, size: %lu", len, size);
  SM_ARRAY_SET_LEN(mesh->normals, len);
  if (!sm_filesystem_read_bytes(fh, mesh->normals, len * size)) {
    SM_LOG_ERROR("[s] failed to read normals");
    return false;
  }

  /* Read indices */
  if (!sm_filesystem_read_bytes(fh, &len, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the length of indices");
    return false;
  }
  if (!sm_filesystem_read_bytes(fh, &size, sizeof(size_t))) {
    SM_LOG_ERROR("[s] failed to read the size of indices");
    return false;
  }
  SM_LOG_TRACE("indices: len: %lu, size: %lu", len, size);
  SM_ARRAY_SET_LEN(mesh->indices, len);
  if (!sm_filesystem_read_bytes(fh, mesh->indices, len * size)) {
    SM_LOG_ERROR("[s] failed to read indices");
    return false;
  }

  if (!sm_filesystem_read_bytes(fh, &mesh->flags, sizeof(mesh->flags))) {
    SM_LOG_ERROR("[s] failed to read flags");
    return false;
  }

  return true;
}

void sm_mesh_dtor(void *ptr) {

  SM_ASSERT(ptr);

  sm_mesh_s *mesh = (sm_mesh_s *)ptr;

  SM_ARRAY_DTOR(mesh->positions);
  SM_ARRAY_DTOR(mesh->uvs);
  SM_ALIGNED_ARRAY_DTOR(mesh->colors);
  SM_ARRAY_DTOR(mesh->normals);
  SM_ARRAY_DTOR(mesh->indices);
}

b8 sm_transform_write(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_transform_s *transform = (sm_transform_s *)ptr;
  SM_LOG_TRACE("Writing transform...");
  sm_transform_print((*(sm_transform_s *)transform));

  if (!sm_filesystem_write_bytes(handle, transform->data, sizeof(vec4[3]))) {
    SM_LOG_ERROR("failed to write transform");
    return false;
  }

  return true;
}

b8 sm_transform_read(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_transform_s *transform = (sm_transform_s *)ptr;
  SM_LOG_TRACE("Reading transform...");

  if (!sm_filesystem_read_bytes(handle, transform->data, sizeof(vec4[3]))) {
    SM_LOG_ERROR("failed to read transform");
    return false;
  }

  return true;
}

b8 sm_speed_write(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_speed_s *speed = (sm_speed_s *)ptr;
  SM_LOG_TRACE("Writing speed...");

  if (!sm_filesystem_write_bytes(handle, speed, sizeof(sm_speed_s))) {
    SM_LOG_ERROR("failed to write speed");
    return false;
  }

  return true;
}

b8 sm_speed_read(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_speed_s *speed = (sm_speed_s *)ptr;
  SM_LOG_TRACE("Reading speed...");

  if (!sm_filesystem_read_bytes(handle, speed, sizeof(sm_speed_s))) {
    SM_LOG_ERROR("failed to read speed");
    return false;
  }

  return true;
}

b8 sm_vec3_write(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_vec3 *vec3 = (sm_vec3 *)ptr;
  SM_LOG_TRACE("Writing vec3...");

  if (!sm_filesystem_write_bytes(handle, vec3, sizeof(sm_vec3))) {
    SM_LOG_ERROR("failed to write vec3");
    return false;
  }

  return true;
}

b8 sm_vec3_read(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_vec3 *vec3 = (sm_vec3 *)ptr;
  SM_LOG_TRACE("Reading vec3...");

  if (!sm_filesystem_read_bytes(handle, vec3, sizeof(sm_vec3))) {
    SM_LOG_ERROR("failed to read vec3");
    return false;
  }

  return true;
}

b8 sm_camera_write(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_camera_s *camera = (sm_camera_s *)ptr;
  SM_LOG_TRACE("Writing camera...");

  if (!sm_filesystem_write_bytes(handle, camera, sizeof(sm_camera_s))) {
    SM_LOG_ERROR("failed to write camera");
    return false;
  }

  return true;
}

b8 sm_camera_read(const sm_file_handle_s *handle, const void *ptr) {

  SM_ASSERT(ptr);

  sm_camera_s *camera = (sm_camera_s *)ptr;
  SM_LOG_TRACE("Reading camera...");

  if (!sm_filesystem_read_bytes(handle, camera, sizeof(sm_camera_s))) {
    SM_LOG_ERROR("failed to read camera");
    return false;
  }

  return true;
}

#undef SM_MODULE_NAME
