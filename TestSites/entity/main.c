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

typedef struct {

  struct layer_s *layer;
  struct renderer2D_s *renderer;
  struct scene_s *scene;
  sm_entity_s entity;
  size_t player_count;
  sm_entity_s *player;

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

void apply_force(system_iterator_s *iter, float dt) {

  printf("apply force\n");

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    sm_vec3 *force = (sm_vec3 *)iter->iter_data[1].data;
  }
}

void apply_vel(system_iterator_s *iter, float dt) {

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    sm_vec3 *velocity = (sm_vec3 *)iter->iter_data[1].data;

    /* window 800x600 */
    /* bounce off the walls */
    if (position->x < 0.0f) {
      position->x = 0.0f;
      velocity->x = -velocity->x;
    }
    if (position->x > 60.0f) {
      position->x = 60.0f;
      velocity->x = -velocity->x;
    }
    if (position->y < 0.0f) {
      position->y = 0.0f;
      velocity->y = -velocity->y;
    }
    if (position->y > 60.0f) {
      position->y = 60.0f;
      velocity->y = -velocity->y;
    }

    position->data[0] += velocity->x * dt * 13.0f;
    position->data[1] += velocity->y * dt * 13.0f;
    position->data[2] += velocity->z * dt * 13.0f;

    /* printf("%f %f %f\n", position->x, position->y, position->z); */
  }
}

void output_position(system_iterator_s *iter, float dt) {

  printf("output position\n");

  while (system_iter_next(iter)) {

    sm_vec3 *position = (sm_vec3 *)iter->iter_data[0].data;
    /* printf("%f %f %f\n", position->x, position->y, position->z); */
  }
}

void output_speed(system_iterator_s *iter, float dt) {

  printf("output speed\n");

  while (system_iter_next(iter)) {

    speed_s *speed = (speed_s *)iter->iter_data[0].data;
    speed->speed += 1.0f;
    printf("%f\n", speed->speed);
  }
}

#define ENTITY_COUNT 100

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  lab->renderer = renderer2D_new();
  if (!renderer2D_ctor(lab->renderer))
    exit(EXIT_FAILURE);

  lab->scene = scene_new();
  if (!scene_ctor(lab->scene, SM_FORCE_COMP | SM_VELOCITY_COMP | SM_POSITION_COMP | SM_SPEED_COMP))
    exit(EXIT_FAILURE);

  /* SM_POSITION_COMP | SM_VELOCITY_COMP | SM_FORCE_COMP | SM_COLOR_COMP | SM_DRAWBLE_COMP | SM_TRANSFORM_COMP */

  scene_set_system(lab->scene, SM_POSITION_COMP | SM_FORCE_COMP, apply_force, SM_SYSTEM_EXCLUSIVE_FLAG);
  scene_set_system(lab->scene, SM_POSITION_COMP | SM_VELOCITY_COMP, apply_vel, SM_SYSTEM_EXCLUSIVE_FLAG);
  scene_set_system(lab->scene, SM_POSITION_COMP, output_position, SM_SYSTEM_INCLUSIVE_FLAG);
  scene_set_system(lab->scene, SM_SPEED_COMP, output_speed, SM_SYSTEM_INCLUSIVE_FLAG);

  lab->player = calloc(ENTITY_COUNT, sizeof(sm_entity_s));
  /* lab->player[lab->player_count++] = scene_new_entity(lab->scene, SM_SPEED_COMP | SM_POSITION_COMP |
   * SM_VELOCITY_COMP); */

  for (size_t i = 0; i < ENTITY_COUNT; ++i) {
    lab->player[lab->player_count] = scene_new_entity(lab->scene, SM_SPEED_COMP | SM_POSITION_COMP | SM_VELOCITY_COMP);
    int32_t x = rand() % 600 + 1;
    int32_t y = rand() % 600 + 1;
    int32_t z = 0.0f;
    int32_t negx = ((rand() % 6) - 3);
    int32_t negy = (rand() % 6) - 3;
    int32_t negz = 0.0f;

    float val[6] = {x, y, z, negx, negy, negz};
    scene_set_component(lab->scene, lab->player[lab->player_count], val);
    lab->player_count++;
  }
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

  scene_do(lab->scene, dt);

  renderer2D_set_clear_color(lab->renderer, SM_BACKGROUND_COLOR);
  renderer2D_clear(lab->renderer);

  renderer2D_begin(lab->renderer);

  /* float data[6] = {0}; */
  /* for (size_t i = 0; i < lab->player_count; ++i) { */
  /*   scene_get_component(lab->scene, lab->player[i], data); */
  /*   sm_vec2 pos = sm_vec2_new(data[0], data[1]); */
  /*   sm_vec2 size = sm_vec2_new(1.5f, 1.5f); */
  /*   float colorx = sinf(data[2] * 0.1f) * 0.5f + 0.5f; */
  /*   float colory = cosf(data[2] * 0.1f) * 0.5f + 0.5f; */
  /*   float colorz = atanf(data[2] * 0.1f) * 0.5f + 0.5f; */
  /*   sm_vec4 color = sm_vec4_new(colorx, colory, colorz, 1.0f); */
  /**/
  /*   renderer2D_draw_quad(lab->renderer, pos.data, size.data, color); */
  /* } */

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
  if (show_demo_window)
    igShowDemoWindow(&show_demo_window);

  igBegin("Settings", &show_demo_window, 0);

  igSliderInt("Quad Count", &lab->player_count, 0, ENTITY_COUNT, NULL, 0);

  igEnd();

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