
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/smApplication.h"
#include "core/smKeyCode.h"
#include "core/smLayer.h"
#include "renderer/smRenderer2D.h"
#include "smInput.h"
#include "util/colors.h"

typedef struct {

  layer_s *layer;
  struct renderer2D_s *renderer;

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

  renderer2D_draw_quad(lab->renderer, vec2_new(-0.5, -0.5f), vec2_new(1.0, 1.0f), RED, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(-1.0, -1.0f), vec2_new(1.0, 1.0f), GREEN, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(0.0, 0.0f), vec2_new(1.0, 1.0f), BLACK, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(0.5, -1.5f), vec2_new(1.0, 1.0f), NAVY, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(-1.5, 0.5f), vec2_new(1.0, 1.0f), MAROON, 0.0f);

  static float deg_ang = 0;
  deg_ang += dt * 45;
  renderer2D_draw_quad_rotated(lab->renderer, vec2_new(-1.0, -1.0f), vec2_new(0.5, 0.5f), YELLOW, 0.0f, 45 - deg_ang);
  renderer2D_draw_quad_rotated(lab->renderer, vec2_new(0.0, 0.0f), vec2_new(1.0, 1.0f), BLUE, 0.0f, 90 - deg_ang);
  renderer2D_draw_quad_rotated(lab->renderer, vec2_new(0.5, -1.5f), vec2_new(1.0, 1.0f), FUCHSIA, 0.0f, 180 - deg_ang);
  renderer2D_draw_quad_rotated(lab->renderer, vec2_new(-1.5, 0.5f), vec2_new(1.0, 1.0f), PURPLE, 0.0f, deg_ang);

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

int main(void) {

  struct application_s *app = application_new();
  if (!application_ctor(app, "lab")) {
    exit(EXIT_FAILURE);
  }

  lab_s *lab = calloc(1, sizeof(lab_s));

  layer_s *layer = calloc(1, sizeof(layer_s));
  layer->user_data = lab;
  layer->on_attach = on_attach;
  layer->on_detach = on_detach;
  layer->on_update = on_update;
  layer->on_event = on_event;

  lab->layer = layer;

  application_push_layer(app, layer);

  application_do(app);

  application_dtor(app);
  free(layer);
  free(lab);

  return EXIT_SUCCESS;
}
