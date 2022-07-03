#ifndef SM_SCENE_COMPONENTS_H
#define SM_SCENE_COMPONENTS_H

#include "smpch.h"

#include "core/smCore.h"
#include "math/smMath.h"

typedef u64 sm_component_t;

extern const sm_component_t SM_TRANSFORM_COMP;
/* typedef struct sm__transform_s sm_transform_s; */
b8 sm_transform_write(const sm_file_handle_s *handle, const void *ptr);
b8 sm_transform_read(const sm_file_handle_s *handle, const void *ptr);

extern const sm_component_t SM_VELOCITY_COMP;
typedef sm_vec3 sm_velocity_s;

extern const sm_component_t SM_FORCE_COMP;
typedef sm_vec3 sm_force_s;

b8 sm_vec3_write(const sm_file_handle_s *handle, const void *ptr);
b8 sm_vec3_read(const sm_file_handle_s *handle, const void *ptr);

extern const sm_component_t SM_SPEED_COMP;
typedef struct {
  f32 speed;
} sm_speed_s;

b8 sm_speed_write(const sm_file_handle_s *handle, const void *ptr);
b8 sm_speed_read(const sm_file_handle_s *handle, const void *ptr);

extern const sm_component_t SM_MESH_COMP;
/* typedef struct sm__vertex_s { */
/**/
/*   sm_vec3 positions; */
/*   sm_vec2 uvs; */
/*   sm_vec4 colors; */
/*   sm_vec3 normals; */
/**/
/* } sm_vertex_s; */

typedef struct sm__mesh_s {

  /* SM_ARRAY(sm_vertex_s) vertices; */
  SM_ARRAY(sm_vec3) positions;
  SM_ARRAY(sm_vec2) uvs;
  SM_ARRAY(sm_vec4) colors;
  SM_ARRAY(sm_vec3) normals;
  SM_ARRAY(u32) indices;

  enum sm_mesh_component_flags_e { MESH_FLAG_NONE, MESH_FLAG_RENDERABLE } flags;
} sm_mesh_s;

b8 sm_mesh_write(const sm_file_handle_s *handle, const void *ptr);
b8 sm_mesh_read(const sm_file_handle_s *handle, const void *ptr);
void sm_mesh_dtor(void *ptr);

static inline void sm_mesh_component_set_renderable(sm_mesh_s *mesh, bool renderable) {
  if (renderable)
    mesh->flags |= MESH_FLAG_RENDERABLE;
  else
    mesh->flags &= ~MESH_FLAG_RENDERABLE;
}

static inline b8 sm_mesh_component_is_renderable(const sm_mesh_s *mesh) {
  return mesh->flags & MESH_FLAG_RENDERABLE;
}

extern const sm_component_t SM_CAMERA_COMP;

typedef struct sm__camera_s {

  b8 main;

  sm_vec3 position;
  sm_vec3 target, _next_target;
  float target_distance;
  sm_vec3 right;
  sm_vec3 up;

  sm_vec3 angle; // pitch, yaw, roll

  f32 fov;
  f32 aspect;

  f32 move_speed;
  f32 sensitive;

} sm_camera_s;

b8 sm_camera_write(const sm_file_handle_s *handle, const void *ptr);
b8 sm_camera_read(const sm_file_handle_s *handle, const void *ptr);

extern const sm_component_t SM_ALL_COMP;

typedef void (*sm_component_dtor_f)(void *ptr);
typedef b8 (*sm_component_write_f)(const sm_file_handle_s *handle, const void *ptr);
typedef b8 (*sm_component_read_f)(const sm_file_handle_s *handle, const void *ptr);

typedef struct sm__component_view_s {

  sm_component_t id;
  sm_string name;

  /* size in bytes of the component */
  size_t size;
  size_t offset;

  b8 alligned;
  sm_component_write_f write;
  sm_component_read_f read;
  sm_component_dtor_f dtor;

} sm_component_view_s;

void component_init(void);
void component_teardown(void);

void component_register_component(sm_component_view_s *desc);
const sm_component_view_s *component_get_desc(sm_component_t id);

#endif /* SM_SCENE_COMPONENTS_H */
