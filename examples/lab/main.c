
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

  renderer2D_begin(lab->renderer);

  renderer2D_draw_quad(lab->renderer, vec2_new(-0.5, -0.5f), vec2_new(1.0, 1.0f), AQUA, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(-1.0, -1.0f), vec2_new(1.0, 1.0f), TEAL, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(0.0, 0.0f), vec2_new(1.0, 1.0f), BLUE, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(0.5, -1.5f), vec2_new(1.0, 1.0f), OLIVE, 0.0f);
  renderer2D_draw_quad(lab->renderer, vec2_new(-1.5, 0.5f), vec2_new(1.0, 1.0f), GREEN, 0.0f);

  renderer2D_end(lab->renderer);
}

void on_event(event_s *event, void *user_data) {

  assert(user_data);

  printf("on_event\n");
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
