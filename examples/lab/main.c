
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cimgui/smCimguiImpl.h"
#include "core/smApplication.h"
#include "core/smKeyCode.h"
#include "core/smLayer.h"
#include "renderer/smRenderer2D.h"
#include "resources/smResources.h"
#include "smInput.h"
#include "util/colors.h"

typedef struct {

  layer_s *layer;
  struct renderer2D_s *renderer;

  int draw_quads;
  bool rotate;
  float angle_rad;
  vec4 color;

} lab_s;

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  lab->renderer = renderer2D_new();
  if (!renderer2D_ctor(lab->renderer, OPENGL21))
    exit(EXIT_FAILURE);
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;
  renderer2D_dtor(lab->renderer);
}

void on_update(void *user_data, float dt) {

  /* printf("on_update\n"); */
  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  renderer2D_set_clear_color(lab->renderer, GRAY);
  renderer2D_clear(lab->renderer);

  renderer2D_begin(lab->renderer);

  for (size_t i = 0; i < lab->draw_quads; ++i) {

    if (lab->rotate)
      renderer2D_draw_quad_rotated(lab->renderer, vec2_new((i * 0.6) + -1.0f, 0.0f), vec2_new(0.5, 0.5f), lab->color,
                                   0.0f, glm_deg(lab->angle_rad));
    else
      renderer2D_draw_quad(lab->renderer, vec2_new((i * 0.6) + -1.0f, 0.0f), vec2_new(0.5, 0.5f), lab->color, 0.0f);
  }

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

  static bool show_demo_window = true;
  if (show_demo_window)
    igShowDemoWindow(&show_demo_window);

  igText("Drawing quads: %d", lab->draw_quads);

  igSliderInt("Draw quads", &lab->draw_quads, 0, 10, NULL, 0);
  igCheckbox("Rotate", &lab->rotate);
  igSliderAngle("Angle", &lab->angle_rad, 0.0f, 360.0f, NULL, 0);
  igColorEdit4("Color", lab->color, 0);

  if (igTreeNode_Str("Assets")) {
    resource_iter_s iter = resource_iter_new(RESOURCE_TYPE_IMAGE, RESOURCE_STATUS_MASK_ALL);
    const char *name = NULL;
    while ((name = resource_iter_next(&iter))) {

      static int selected = -1;
      if (igSelectable_Bool(name, selected == iter.index, 0, (ImVec2){0, 0}))
        selected = iter.index;
    }

    igTreePop();
  }
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
