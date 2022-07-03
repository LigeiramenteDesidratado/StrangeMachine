#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cimgui/smCimguiImpl.h"
#include "core/smApplication.h"
#include "core/smKeyCode.h"
#include "core/smLayer.h"
#include "core/smPool.h"
#include "math/smMath.h"
#include "renderer/smRenderer2D.h"
#include "renderer/smRenderer3D.h"
#include "resource/file_format/smSMR.h"
#include "resource/smResource.h"
#include "resource/smTextureResourcePub.h"
#include "scene/smComponents.h"
#include "scene/smScene.h"
#include "smInput.h"
#include "util/colors.h"

#define REF(X) X

typedef struct {

  sm_scene_s *scene;
  struct layer_s *layer;

} lab_s;

sm_mat4 view_matrix;
sm_mat4 projection_matrix;
sm_mat4 view_projection_matrix;
sm_camera_s *main_camera = NULL;

void sm__mesh_draw(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  while (system_iter_next(iter)) {

    const sm_transform_s *trans = (sm_transform_s *)iter->iter_data[0].data;
    const sm_mesh_s *mesh = (sm_mesh_s *)iter->iter_data[1].data;

    sm_transform_s t = *trans;
    sm_renderer3D_begin(view_projection_matrix);
    sm_renderer3D_draw_mesh(mesh, t);
    sm_renderer3D_end();
  }
}

void sm__camera_free_do(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  while (system_iter_next(iter)) {

    sm_camera_s *camera = (sm_camera_s *)iter->iter_data[0].data;

    if (!camera->main)
      continue;

    if (input_scan_key(sm_key_period)) {
      /* camera reset */
      sm_camera_s cam = {
          .main = true,
          .position = sm_vec3_new(0.0f, 0.0f, 1.0f),
          .target = sm_vec3_new(0.0f, 0.0f, 0.0f),
          ._next_target = sm_vec3_new(0.0f, 0.0f, 0.0f),
          .up = sm_vec3_new(0.0f, 1.0f, 0.0f),
          .move_speed = 12.0f,
          .sensitive = 11.4f,
          .target_distance = 10.2f,
          .fov = 75.0f,
          .angle = sm_vec3_new(0.0f, 0.0f, 0.0f),
      };

      *camera = cam;
      main_camera = camera;
      return;
    }

    sm_vec3 pos;

    if (input_scan_key(sm_key_s)) {
      glm_vec3_scale(camera->target.data, camera->move_speed * dt, pos.data);
      glm_vec3_add(camera->position.data, pos.data, camera->position.data);
    }

    if (input_scan_key(sm_key_w)) {
      glm_vec3_scale(camera->target.data, camera->move_speed * dt, pos.data);
      glm_vec3_sub(camera->position.data, pos.data, camera->position.data);
    }

    if (input_scan_key(sm_key_d)) {
      glm_vec3_scale(camera->right.data, camera->move_speed * dt, pos.data);
      glm_vec3_sub(camera->position.data, pos.data, camera->position.data);
    }

    if (input_scan_key(sm_key_a)) {
      glm_vec3_scale(camera->right.data, camera->move_speed * dt, pos.data);
      glm_vec3_add(camera->position.data, pos.data, camera->position.data);
    }

    if (input_scan_key(sm_key_space)) {
      glm_vec3_scale(camera->up.data, camera->move_speed * dt, pos.data);
      glm_vec3_add(camera->position.data, pos.data, camera->position.data);
    }
    if (input_scan_key(sm_key_lshift)) {
      glm_vec3_scale(camera->up.data, camera->move_speed * dt, pos.data);
      glm_vec3_sub(camera->position.data, pos.data, camera->position.data);
    }

    float offset_x = input_get_x_rel_mouse();
    float offset_y = input_get_y_rel_mouse();

    if (offset_x != 0 || offset_y != 0) {
      camera->angle.y += offset_x * camera->sensitive * dt;
      camera->angle.x -= offset_y * camera->sensitive * dt;

      if (camera->angle.x > 80.0f)
        camera->angle.x = 80.0f;
      if (camera->angle.x < -80.0f)
        camera->angle.x = -80.0f;

      if (camera->angle.y > 360.0f || camera->angle.y < -360.0f)
        camera->angle.y = 0.0f;
    }

    sm_vec3 front;
    front.x = cosf(glm_rad(camera->angle.y)) * cosf(glm_rad(camera->angle.x));
    front.y = sinf(glm_rad(camera->angle.x));
    front.z = sinf(glm_rad(camera->angle.y)) * cosf(glm_rad(camera->angle.x));

    glm_vec3_normalize_to(front.data, camera->target.data);

    sm_vec3 right;
    glm_vec3_cross(camera->target.data, (vec3){0.0f, 1.0f, 0.0f}, right.data);
    glm_vec3_normalize_to(right.data, camera->right.data);

    sm_vec3 up;
    glm_vec3_cross(camera->right.data, camera->target.data, up.data);
    glm_vec3_normalize_to(up.data, camera->up.data);

    sm_vec3 dist;
    glm_vec3_sub(camera->position.data, camera->target.data, dist.data);
    glm_lookat(camera->position.data, dist.data, camera->up.data, view_matrix.data);

    glm_perspective(glm_rad(camera->fov), 800 / 600.0f, 0.01f, 100.0f, projection_matrix.data);

    glm_mat4_mul(projection_matrix.data, view_matrix.data, view_projection_matrix.data);
    main_camera = camera;
  }
}

void sm__mesh_move(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  while (system_iter_next(iter)) {

    sm_transform_s *trans = (sm_transform_s *)iter->iter_data[0].data;
    sm_speed_s *speed = (sm_speed_s *)iter->iter_data[1].data;
    sm_mesh_s *mesh = (sm_mesh_s *)iter->iter_data[2].data;

    trans->position.x += 10.0f * speed->speed * dt;
  }
}

void sm__default_system_renderable(sm_graph_s *graph, u32 index, void *user_data) {

  if (index == ROOT)
    return;

  sm_entity_s ett = sm_scene_graph_get_entity(graph, index);
  if (!SM_MASK_CHK(ett.archetype_index, SM_MESH_COMP))
    return;

  const void *data = sm_scene_get_component(user_data, ett);
  if (index == 3) {

    sm_vec3 dir = {
        {{(float)(input_scan_key(sm_key_a) * .03f + -input_scan_key(sm_key_d) * .03f), 0.f,
          (float)(input_scan_key(sm_key_w) * .03f + -input_scan_key(sm_key_s) * .03f)}},
    };

    sm_transform_s *t = (sm_transform_s *)data;
    glm_vec3_add(dir.data, t->position.data, t->position.data);
  }

  sm_transform_s global = sm_scene_get_global_transform(user_data, index);
  sm_mesh_s *mesh_data = (sm_mesh_s *)((u8 *)data + sizeof(sm_transform_s) + sizeof(sm_speed_s));

  sm_renderer3D_begin(view_projection_matrix);
  sm_renderer3D_draw_mesh(mesh_data, global);
  sm_renderer3D_end();
}

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  sm_string path = sm_string_from("assets/scene/scene.smscene");
  lab->scene = sm_scene_open(path);
  if (!lab->scene) {
    printf("Failed to open scene\n");
    exit(EXIT_FAILURE);
  }
  sm_string_dtor(path);

  sm_scene_register_system(lab->scene, SM_CAMERA_COMP, sm__camera_free_do, SM_SYSTEM_INCLUSIVE_FLAG);

  /* sm_entity_s ett = sm_scene_new_entity(lab->scene, SM_CAMERA_COMP); */
  /* u32 idx = sm_scene_graph_new_node(lab->scene->scene_graph); */
  /* sm_scene_graph_add_child(lab->scene->scene_graph, ROOT, idx); */
  /**/
  /* sm_camera_s camera = { */
  /*     .main = true, */
  /*     .position = sm_vec3_new(0.0f, 0.0f, 1.0f), */
  /*     .target = sm_vec3_new(0.0f, 0.0f, 0.0f), */
  /*     .up = sm_vec3_new(0.0f, 1.0f, 0.0f), */
  /*     .move_speed = 12.0f, */
  /*     .sensitive = 11.4f, */
  /*     .target_distance = 10.2f, */
  /*     .fov = 75.0f, */
  /*     .angle = sm_vec3_new(0.0f, 0.0f, 0.0f), */
  /* }; */
  /* sm_scene_set_component_data(lab->scene, ett, SM_CAMERA_COMP, &camera); */
  /* sm_scene_graph_set_entity(lab->scene->scene_graph, idx, ett); */
  /**/
  /* sm_string name = sm_string_from("camera"); */
  /* sm_scene_graph_set_name(lab->scene->scene_graph, idx, name); */
  /* sm_string_dtor(name); */

  /* sm_scene_register_system(lab->scene, SM_TRANSFORM_COMP | SM_SPEED_COMP | SM_MESH_COMP, sm__mesh_move, */
  /*                          SM_SYSTEM_INCLUSIVE_FLAG); */
  /* sm_scene_register_system(lab->scene, SM_TRANSFORM_COMP | SM_MESH_COMP, sm__mesh_draw, SM_SYSTEM_INCLUSIVE_FLAG); */
  /* sm_mesh_component_set_renderable(&mesh, true); */
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  sm_string path = sm_string_from("assets/scene/scene.smscene");
  sm_scene_save(lab->scene, path);
  sm_string_dtor(path);

  sm_scene_dtor(lab->scene);
}

void on_update(void *user_data, float dt) {

  assert(user_data);
  (void)dt;

  lab_s *lab = (lab_s *)user_data;

  sm_renderer3D_set_clear_color(SM_BACKGROUND_COLOR);
  sm_renderer3D_clear();
  sm_scene_do(lab->scene, dt);

  sm_scene_graph_for_each(lab->scene->scene_graph, 0, sm__default_system_renderable, lab->scene);
}

bool on_event(event_s *event, void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;
  (void)lab;

  switch (event->category) {
  case SM_CATEGORY_WINDOW:
    if (event->window.type == SM_EVENT_WINDOW_RESIZE)
      /* renderer2D_set_viewport(lab->renderer, 0, 0, event->window.width, event->window.height); */
      return true;
  default:
    break;
  }
  return false;
}

void on_gui(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;
  (void)lab;

  static bool show_window = true;

  if (main_camera == NULL)
    return;

  igBegin("Settings", &show_window, 0);

  /* igSliderInt("Quad Count", (int32_t *)&lab->entities_count, 0, ENTITY_COUNT, NULL, 0); */
  /* (label,v,v_speed,v_min,v_max,format,flags); */
  /* igDragInt("cube index", &lab->selected, 1.0f, 0, 15, NULL, 0); */

  /* igCombo_Str("Selected cube", &lab->selected, " 0\0 1\0 2\0 3\0 4\0 5\0 6\0 7\0 8\0 9\0 10\0 11\0 12\0 13\0 14\0
   * 15\0", */
  /*             6); */

  /* sm_vec3 position; */
  /* sm_vec3 target, _next_target; */
  /* float target_distance; */
  /* sm_vec3 right; */
  /* sm_vec3 up; */
  /**/
  /* sm_vec3 angle; // pitch, yaw, roll */
  /**/
  /* f32 fov; */
  /* f32 aspect; */
  /**/
  /* f32 move_speed; */
  /* f32 sensitive; */

  igDragFloat3("Position", main_camera->position.data, 0.01f, -100.f, 100.0f, "%.4f", 0);
  igDragFloat3("Target", main_camera->target.data, 0.01f, -100.f, 100.0f, "%.4f", 0);
  igDragFloat("Target Distance", &main_camera->target_distance, 0.01f, 0.0f, 100.0f, "%.4f", 0);
  igDragFloat3("Up", main_camera->up.data, 0.01f, -100.f, 100.0f, "%.4f", 0);
  igDragFloat("Sensitive", &main_camera->sensitive, 0.01f, 0.0f, 100.0f, "%.4f", 0);
  igDragFloat("Move Speed", &main_camera->move_speed, 0.01f, 0.0f, 100.0f, "%.4f", 0);
  igDragFloat("FOV", &main_camera->fov, 0.01f, 0.0f, 100.0f, "%.4f", 0);
  igDragFloat("Aspect", &main_camera->aspect, 0.01f, 0.0f, 100.0f, "%.4f", 0);
  igDragFloat3("Angle", main_camera->angle.data, 0.01f, -100.f, 100.0f, "%.4f", 0);

  /**/
  /* sm_vec3 v_deg = {.x = glm_deg(t->rotation.x), .y = glm_deg(t->rotation.y), .z = glm_deg(t->rotation.z)}; */
  /**/
  /* float min = 0.0f, max = 360.0f; */
  /* igDragScalarN("Rotation", ImGuiDataType_Float, v_deg.data, 3, .333f, &min, &max, "%.3f", 0); */
  /**/
  /* t->rotation.x = glm_rad(v_deg.x); */
  /* t->rotation.y = glm_rad(v_deg.y); */
  /* t->rotation.z = glm_rad(v_deg.z); */
  /**/
  /* igColorEdit4("Color", lab->colors[lab->selected].data, 0); */

  /* igDragFloat3("monkey pos", lab->model->transform.position.data, 0.01f, -100.f, 100.0f, "%.4f", 0); */
  /* igDragFloat3("monkey sca", lab->model->transform.scale.data, 0.01f, 0.0f, 100.0f, "%.4f", 0); */
  /**/
  /* sm_vec3 m_deg = {.x = glm_deg(lab->model->transform.rotation.x), */
  /*                  .y = glm_deg(lab->model->transform.rotation.y), */
  /*                  .z = glm_deg(lab->model->transform.rotation.z)}; */
  /**/
  /* igDragScalarN("Rotation", ImGuiDataType_Float, m_deg.data, 3, .333f, &min, &max, "%.3f", 0); */
  /**/
  /* lab->model->transform.rotation.x = glm_rad(m_deg.x); */
  /* lab->model->transform.rotation.y = glm_rad(m_deg.y); */
  /* lab->model->transform.rotation.z = glm_rad(m_deg.z); */

  /* igColorEdit4("Color", lab->colors[lab->selected].data, 0); */

  igEnd();
}

int main(void) {

  struct application_s *app = application_new();
  if (!application_ctor(app, "lab")) {
    exit(EXIT_FAILURE);
  }

  lab_s *lab = calloc(1, sizeof(lab_s));

  struct layer_s *layer = layer_new();
  if (!layer_ctor(layer, "entity", lab, on_attach, on_detach, on_update, on_gui, on_event)) {
    exit(EXIT_FAILURE);
  }
  lab->layer = layer;

  application_push_layer(app, layer);

  application_do(app);

  application_dtor(app);
  layer_dtor(layer);
  free(lab);

  return EXIT_SUCCESS;
}
