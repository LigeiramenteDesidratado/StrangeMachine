#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/data/smArray.h"

#include "cimgui/smCimguiImpl.h"
#include "core/smApplication.h"
#include "core/smKeyCode.h"
#include "core/smLayer.h"
#include "core/smPool.h"
#include "math/smMath.h"
#include "renderer/smRenderer2D.h"
#include "resource/smResource.h"
#include "smInput.h"
#include "util/colors.h"

typedef struct {

  layer_s *layer;
  struct renderer2D_s *renderer;

  int draw_quads;
  bool rotate;
  float angle_rad;
  vec4 color;

  int selected;
  texture_handler_s *textures;

} lab_s;

static void SimpleOverlay(bool *p_open) {
  static int corner = 0;
  ImGuiIO *io = igGetIO();
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
    /* if (igIsMousePosValid()) */
    /* igText("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y); */
    /* else */
    /* igText("Mouse Position: <invalid>"); */
  }
  igEnd();
}

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;
  glm_vec4_copy(SM_MAIN_COLOR_1, lab->color);

  lab->renderer = renderer2D_new();
  if (!renderer2D_ctor(lab->renderer, OPENGL21))
    exit(EXIT_FAILURE);

  resource_iter_s iter = resource_iter_new(RESOURCE_TYPE_IMAGE, RESOURCE_STATUS_MASK_ALL);
  const char *name = NULL;
  while ((name = resource_iter_next(&iter))) {
    texture_handler_s texture = resource_load_texture(name);
    SM_ARRAY_PUSH(lab->textures, texture);
  }

  lab->selected = 0;
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  SM_ARRAY_DTOR(lab->textures);

  /* texture_dtor(lab->texture); */

  renderer2D_dtor(lab->renderer);
}

void on_update(void *user_data, float dt) {

  /* printf("on_update\n"); */
  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  renderer2D_set_clear_color(lab->renderer, SM_BACKGROUND_COLOR);
  renderer2D_clear(lab->renderer);

  renderer2D_begin(lab->renderer);

  /* printf("selected: %d\n", lab->selected); */

  for (size_t i = 0; i < lab->draw_quads; ++i) {

    if (lab->rotate)
      renderer2D_draw_sprite_rotated(lab->renderer, vec2_new((i * 0.6) + -1.0f, 0.0f), vec2_new(0.5, 0.5f),
                                     lab->textures[lab->selected], glm_deg(lab->angle_rad * i));
    else
      /* renderer2D_draw_quad(lab->renderer, vec2_new((i * 0.6) + -1.0f, 0.0f), vec2_new(0.5, 0.5f), lab->color, 0.0f);
       */
      renderer2D_draw_sprite(lab->renderer, vec2_new((i * 0.6) + -1.0f, 0.0f), vec2_new(0.5, 0.5f),
                             lab->textures[lab->selected]);
  }

  renderer2D_end(lab->renderer);

  // renderer2D_draw_quad(lab->renderer, vec2_new(-1.0, -1.0f), vec2_new(1.0, 1.0f), GREEN, 0.0f);
  // renderer2D_draw_quad(lab->renderer, vec2_new(0.0, 0.0f), vec2_new(1.0, 1.0f), BLACK, 0.0f);
  // renderer2D_draw_quad(lab->renderer, vec2_new(0.5, -1.5f), vec2_new(1.0, 1.0f), NAVY, 0.0f);
  // renderer2D_draw_quad(lab->renderer, vec2_new(-1.5, 0.5f), vec2_new(1.0, 1.0f), MAROON, 0.0f);

  // static float deg_ang = 0;
  // deg_ang += dt * 45;
  // renderer2D_draw_quad_rotated(lab->renderer, vec2_new(-1.0, -1.0f), vec2_new(0.5, 0.5f), YELLOW, 5.0f, 45 -
  // deg_ang); renderer2D_draw_quad_rotated(lab->renderer, vec2_new(0.0, 0.0f), vec2_new(1.0, 1.0f), BLUE, 0.0f, 90 -
  // deg_ang); renderer2D_draw_quad_rotated(lab->renderer, vec2_new(0.5, -1.5f), vec2_new(1.0, 1.0f), FUCHSIA, 0.0f, 180
  // - deg_ang); renderer2D_draw_quad_rotated(lab->renderer, vec2_new(-1.5, 0.5f), vec2_new(1.0, 1.0f), PURPLE, 0.0f,
  // deg_ang);
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

  igSliderInt("Draw quads", &lab->draw_quads, 0, 1025, NULL, 0);
  igCheckbox("Rotate", &lab->rotate);
  igSliderAngle("Angle", &lab->angle_rad, 0.0f, 360.0f, NULL, 0);
  igColorEdit4("Color", lab->color, 0);
  igSeparator();

  if (igTreeNode_Str("Assets")) {
    resource_iter_s iter = resource_iter_new(RESOURCE_TYPE_IMAGE, RESOURCE_STATUS_MASK_ALL);
    const char *name = NULL;
    while ((name = resource_iter_next(&iter))) {

      if (igSelectable_Bool(name, lab->selected + 1 == iter.index, 0, (ImVec2){0, 0}))
        lab->selected = iter.index - 1;
    }

    igTreePop();
  }

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

  lab->draw_quads = 4;

  layer_s *layer = calloc(1, sizeof(layer_s));
  layer->name = "lab";
  layer->user_data = lab;
  layer->on_attach = on_attach;
  layer->on_detach = on_detach;
  layer->on_update = on_update;
  layer->on_event = on_event;
  layer->on_gui = on_gui;

  lab->layer = layer;

  application_push_layer(app, layer);

  application_do(app);

  application_dtor(app);
  free(layer);
  free(lab);

  return EXIT_SUCCESS;
}
