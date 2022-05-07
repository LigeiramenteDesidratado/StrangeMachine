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
#include "resource/smResource.h"
#include "resource/smTextureResourcePub.h"
#include "scene/smComponents.h"
#include "scene/smScene.h"
#include "smInput.h"
#include "util/colors.h"

#define REF(X) X

typedef struct {

  struct layer_s *layer;

  /* reference to the current scene */
  REF(struct sm_scene_s *) scene;

  struct renderer2D_s *renderer;
  size_t entities_count;
  sm_entity_s *entities;
  sm_entity_s transform_entity;
  sm_entity_s camera_entity;

  texture_handler_s texture;

} lab_s;

static void SimpleOverlay(bool *p_open) {
  static int corner = 0;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav;
  if (corner != -1) {
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  }

  igSetNextWindowBgAlpha(0.35f); // Transparent background
  if (igBegin("Example: Simple overlay", p_open, window_flags)) {
    igText("Quad Count: %d", renderer2D_stats_get_quad_count());
    igText("Draw Calls: %d", renderer2D_stats_get_draw_call_count());
  }
  igEnd();
}

void apply_force(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  printf("apply force\n");

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    sm_vec3 *force = (sm_vec3 *)iter->iter_data[1].data;

    (void)position;
    (void)force;
  }
}

void apply_vel(sm_system_iterator_s *iter, float dt) {

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    sm_vec3 *velocity = (sm_vec3 *)iter->iter_data[1].data;

    /* window 800x600 */
    /* bounce off the walls */

    int32_t width = 800.0f / 2.0f;
    int32_t height = 600.0f / 2.0f;

    if (position->x < -width) {
      position->x = -width;
      velocity->x = -velocity->x;
    }
    if (position->x > width) {
      position->x = width;
      velocity->x = -velocity->x;
    }
    if (position->y < -height) {
      position->y = -height;
      velocity->y = -velocity->y;
    }
    if (position->y > height) {
      position->y = height;
      velocity->y = -velocity->y;
    }

    position->data[0] += velocity->x * dt * 13.0f;
    position->data[1] += velocity->y * dt * 13.0f;
    position->data[2] += velocity->z * dt * 13.0f;

    /* printf("%f %f %f\n", position->x, position->y, position->z); */
  }
}

void output_position(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  printf("output position\n");

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    (void)position;
  }
}

void output_speed(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  printf("output speed\n");

  while (system_iter_next(iter)) {

    speed_s *speed = (speed_s *)iter->iter_data[0].data;

    speed->speed += 1.0f;
    printf("%f\n", speed->speed);
  }
}

void transform_system(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  printf("transform system\n");

  while (system_iter_next(iter)) {

    sm_mat4 *transform = (sm_mat4 *)iter->iter_data[0].data;

    sm_mat4_print((*transform));

  }
}

#define ENTITY_COUNT 1684

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  lab->renderer = renderer2D_new();
  if (!renderer2D_ctor(lab->renderer))
    exit(EXIT_FAILURE);

  lab->scene = scene_new();
  if (!scene_ctor(lab->scene, SM_TRANSFORM_COMP | SM_FORCE_COMP | SM_VELOCITY_COMP | SM_POSITION_COMP | SM_SPEED_COMP))
    exit(EXIT_FAILURE);

  scene_register_system(lab->scene, SM_POSITION_COMP | SM_FORCE_COMP, apply_force, SM_SYSTEM_EXCLUSIVE_FLAG);
  scene_register_system(lab->scene, SM_POSITION_COMP | SM_VELOCITY_COMP, apply_vel, SM_SYSTEM_INCLUSIVE_FLAG);
  scene_register_system(lab->scene, SM_TRANSFORM_COMP, transform_system, SM_SYSTEM_INCLUSIVE_FLAG);
  

  lab->transform_entity = scene_new_entity(lab->scene, SM_TRANSFORM_COMP);
  scene_set_component(lab->scene, lab->transform_entity, &sm_mat4_identity());

  lab->entities = calloc(ENTITY_COUNT, sizeof(sm_entity_s));

  for (size_t i = 0; i < ENTITY_COUNT; ++i) {
    lab->entities[lab->entities_count] =
        scene_new_entity(lab->scene, SM_POSITION_COMP | SM_VELOCITY_COMP | SM_SPEED_COMP);
    int32_t posx = (rand() % 800) - 400.0f;
    int32_t posy = (rand() % 600) - 300.0f;
    int32_t posz = 0.0f;
    int32_t velx = (rand() % 6) - 3;
    int32_t vely = (rand() % 6) - 3;
    int32_t velz = 0.0f;

    scene_set_component(lab->scene, lab->entities[lab->entities_count], &(struct {
                          float position[3];
                          float velocity[3];
                          float speed;
                        }){{posx, posy, posz}, {velx, vely, velz}, 1.0f});

    lab->entities_count++;
  }

  lab->texture = resource_load_texture("assets/z.png");
  lab->camera_entity = scene_new_entity(lab->scene, SM_TRANSFORM_COMP);
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  scene_dtor(lab->scene);
  renderer2D_dtor(lab->renderer);
}

void on_update(void *user_data, float dt) {

  assert(user_data);
  (void)dt;

  lab_s *lab = (lab_s *)user_data;

  if (input_scan_key(sm_key_space)) {
    printf("sm_key_space pressed\n");
  }

  scene_do(lab->scene, dt);

  renderer2D_set_clear_color(lab->renderer, SM_BACKGROUND_COLOR);
  renderer2D_clear(lab->renderer);

  renderer2D_begin(lab->renderer);

  for (size_t i = 0; i < lab->entities_count; ++i) {

    const float *data = scene_get_component(lab->scene, lab->entities[i]);
    sm_vec2 pos = sm_vec2_new(data[0], data[1]);
    sm_vec2 size = sm_vec2_new(1.5f, 1.5f);

    renderer2D_draw_sprite(lab->renderer, pos.data, size.data, lab->texture);
  }

  renderer2D_end(lab->renderer);
}

bool on_event(event_s *event, void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  switch (event->category) {
  case SM_CATEGORY_WINDOW:
    if (event->window.type == SM_EVENT_WINDOW_RESIZE)
      renderer2D_set_viewport(lab->renderer, 0, 0, event->window.width, event->window.height);
    return true;
  default:
    break;
  }
  return false;
}

void on_gui(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  static bool show_demo_window = false;

  if (input_scan_key_lock(sm_key_escape))
    show_demo_window = !show_demo_window;

  if (show_demo_window)
    igShowDemoWindow(&show_demo_window);

  /* igBegin("Settings", &show_demo_window, 0); */
  /**/
  /* igSliderInt("Quad Count", (int32_t *)&lab->entities_count, 0, ENTITY_COUNT, NULL, 0); */
  /**/
  /* igEnd(); */

  static bool show_overlay = true;
  if (show_overlay)
    SimpleOverlay(&show_overlay);
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
