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
#include "resource/smTextureResourcePub.h"
#include "smInput.h"
#include "util/colors.h"

typedef struct {

  struct layer_s *layer;
  struct renderer2D_s *renderer;

  int draw_quads;
  vec2 size;
  float space_between;
  bool rotate;
  float angle_rad;
  vec4 color;

  size_t selected;
  texture_handler_s *textures;

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

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;
  glm_vec4_ucopy(SM_MAIN_COLOR_1, lab->color);

  lab->renderer = renderer2D_new();
  if (!renderer2D_ctor(lab->renderer))
    exit(EXIT_FAILURE);

  resource_iter_s iter = resource_iter_new(RESOURCE_TYPE_IMAGE, RESOURCE_STATUS_MASK_ALL);
  const char *name = NULL;
  while ((name = resource_iter_next(&iter))) {
    texture_handler_s texture = resource_load_texture(name);
    SM_ARRAY_PUSH(lab->textures, texture);
  }

  glm_vec2_one(lab->size);
  lab->space_between = 0.5f;
  lab->selected = 0;
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  for (size_t i = 0; i < SM_ARRAY_SIZE(lab->textures); ++i) {
    texture_res_dtor(lab->textures[i]);
  }

  SM_ARRAY_DTOR(lab->textures);

  renderer2D_dtor(lab->renderer);
}

void on_update(void *user_data, float dt) {

  assert(user_data);
  (void)dt;

  lab_s *lab = (lab_s *)user_data;

  renderer2D_set_clear_color(lab->renderer, SM_BACKGROUND_COLOR);
  renderer2D_clear(lab->renderer);

  renderer2D_begin(lab->renderer);

  texture_handler_s th = (SM_ARRAY_SIZE(lab->textures)) ? lab->textures[lab->selected] : (texture_handler_s){0};

  for (int i = 0; i < lab->draw_quads; ++i) {

    if (lab->rotate)
      renderer2D_draw_sprite_rotated(lab->renderer, vec2_new((i * lab->space_between) + -1.0f, 0.0f), lab->size, th,
                                     glm_deg(lab->angle_rad * i));
    else
      renderer2D_draw_sprite(lab->renderer, vec2_new((i * lab->space_between) + -1.0f, 0.0f), lab->size, th);
  }

  if (lab->draw_quads) {
    renderer2D_draw_quad(lab->renderer, vec2_new(0, 0), vec2_new(1.0f, 1.0f), lab->color);
    renderer2D_draw_quad_rotated(lab->renderer, vec2_new(1, 0), vec2_new(1.0f, 1.0f), lab->color,
                                 glm_deg(lab->angle_rad));
    renderer2D_draw_sprite(lab->renderer, vec2_new(0, 1), vec2_new(1.0f, 1.0f), lab->textures[0]);
    renderer2D_draw_sprite_rotated(lab->renderer, vec2_new(1, 1), vec2_new(1.0f, 1.0f), lab->textures[0],
                                   glm_deg(lab->angle_rad));
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
  if (show_demo_window)
    igShowDemoWindow(&show_demo_window);

  igBegin("Settings", &show_demo_window, 0);

  igSliderInt("Draw quads", &lab->draw_quads, 0, 1025, NULL, 0);
  igCheckbox("Rotate", &lab->rotate);
  igSliderAngle("Angle", &lab->angle_rad, 0.0f, 360.0f, NULL, 0);
  igColorEdit4("Color", lab->color, 0);
  igSeparator();

  if (igTreeNode_Str("Assets")) {

    for (size_t i = 0; i < SM_ARRAY_SIZE(lab->textures); ++i) {

      const char *name = texture_res_get_name(lab->textures[i]);

      if (igSelectable_Bool(name, lab->selected == i, 0, (ImVec2){0, 0}))
        lab->selected = i;

      igSameLine(300, -1);
      igText("%d", lab->textures[i].handle);
    }

    /*   resource_iter_s iter = resource_iter_new(RESOURCE_TYPE_IMAGE, RESOURCE_STATUS_MASK_ALL); */
    /*   const char *name = NULL; */
    /*   while ((name = resource_iter_next(&iter))) { */
    /**/
    /*     if (igSelectable_Bool(name, lab->selected + 1 == iter.index, 0, (ImVec2){0, 0})) */
    /*       lab->selected = iter.index - 1; */
    /**/
    /*     igSameLine(300, -1); */
    /*     igText("3,456 bytes"); */
    /*   } */
    /**/
    igTreePop();
  }

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

  lab->draw_quads = 4;

  struct layer_s *layer = layer_new();
  if (!layer_ctor(layer, "lab", lab, on_attach, on_detach, on_update, on_gui, on_event)) {
    exit(EXIT_FAILURE);
  }
  lab->layer = layer;

  application_push_layer(app, layer);

  application_do(app);

  application_dtor(app);
  free(lab);

  return EXIT_SUCCESS;
}
