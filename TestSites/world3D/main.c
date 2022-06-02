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
#include "resource/smResource.h"
#include "resource/smTextureResourcePub.h"
#include "scene/smComponents.h"
#include "scene/smScene.h"
#include "smInput.h"
#include "util/colors.h"

#define REF(X) X

typedef struct {

  struct sm_scene_s *scene;
  struct layer_s *layer;
  sm_vec4 colors[16];

  struct sm_renderer3D_s *renderer3d;
  int32_t selected;
  sm_entity_s *entity;

} lab_s;

void sm__default_system_renderable(sm_system_iterator_s *iter, float dt) {

  (void)dt;

  while (system_iter_next(iter)) {

    const sm_transform_s *trans = (sm_transform_s *)iter->iter_data[0].data;

    renderer3D_begin();
    renderer3D_draw_cube_transform(*trans, MAROON);
    renderer3D_end();
  }
}

void on_attach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  lab->scene = scene_new();
  if (!scene_ctor(lab->scene, SM_TRANSFORM_COMP)) {
    fprintf(stderr, "Failed to create scene\n");
    exit(EXIT_FAILURE);
  }

  scene_register_system(lab->scene, SM_TRANSFORM_COMP, sm__default_system_renderable, SM_SYSTEM_INCLUSIVE_FLAG);

  lab->entity = calloc(16, sizeof(sm_entity_s));

  uint32_t i = 0;
  for (float y = -1.0; y < 1.0f; y += 0.5f) {
    for (float x = -1.0; x < 1.0f; x += 0.5f) {
      sm_entity_s entity = scene_new_entity(lab->scene, SM_TRANSFORM_COMP);
      lab->entity[i] = entity;
      sm_transform_s t; // = sm_transform_zero();
      glm_vec4_copy(sm_vec4_new(x * 5, y * 5, 0.0f, 0.0f).data, t.position.data);
      glm_vec4_copy(((sm_vec4){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}).data, t.rotation.data);
      glm_vec4_copy(sm_vec4_one().data, t.scale.data);
      scene_set_component(lab->scene, lab->entity[i], &t);
      i += 1;
    }
  }

  /* for (size_t i = 0; i < 16; ++i) {} */

  lab->selected = 0;
  lab->colors[0] = BLACK;
  lab->colors[1] = SILVER;
  lab->colors[2] = GRAY;
  lab->colors[3] = WHITE;
  lab->colors[4] = MAROON;
  lab->colors[5] = RED;
  lab->colors[6] = PURPLE;
  lab->colors[7] = FUCHSIA;
  lab->colors[8] = GREEN;
  lab->colors[9] = LIME;
  lab->colors[10] = OLIVE;
  lab->colors[11] = YELLOW;
  lab->colors[12] = NAVY;
  lab->colors[13] = BLUE;
  lab->colors[14] = TEAL;
  lab->colors[15] = AQUA;
}

void on_detach(void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

  scene_dtor(lab->scene);

  free(lab->entity);
}

void on_update(void *user_data, float dt) {

  assert(user_data);
  (void)dt;

  lab_s *lab = (lab_s *)user_data;

  renderer3D_set_clear_color(SM_BACKGROUND_COLOR);
  renderer3D_clear();
  scene_do(lab->scene, dt);

  /* for (size_t i = 0; i < 16; ++i) { */
  /*   const sm_transform_s *trans = scene_get_component(lab->scene, lab->entity[i]); */
  /*   renderer3D_begin(); */
  /*   renderer3D_draw_cube_transform(*trans, lab->colors[i]); */
  /*   renderer3D_end(); */
  /* } */
}

bool on_event(event_s *event, void *user_data) {

  assert(user_data);

  lab_s *lab = (lab_s *)user_data;

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

  static bool show_demo_window = false;

  if (input_scan_key_lock(sm_key_escape))
    show_demo_window = !show_demo_window;

  if (show_demo_window)
    igShowDemoWindow(&show_demo_window);

  igBegin("Settings", &show_demo_window, 0);

  /* igSliderInt("Quad Count", (int32_t *)&lab->entities_count, 0, ENTITY_COUNT, NULL, 0); */
  /* (label,v,v_speed,v_min,v_max,format,flags); */
  /* igDragInt("cube index", &lab->selected, 1.0f, 0, 15, NULL, 0); */

  igCombo_Str("Selected cube", &lab->selected, " 0\0 1\0 2\0 3\0 4\0 5\0 6\0 7\0 8\0 9\0 10\0 11\0 12\0 13\0 14\0 15\0",
              6);

  sm_transform_s *t = (sm_transform_s *)scene_get_component(lab->scene, lab->entity[lab->selected]);

  igDragFloat3("Position", t->position.data, 0.01f, -100.f, 100.0f, "%.4f", 0);
  igDragFloat3("Scale", t->scale.data, 0.01f, 0.0f, 100.0f, "%.4f", 0);

  sm_vec3 v_deg = {.x = glm_deg(t->rotation.x), .y = glm_deg(t->rotation.y), .z = glm_deg(t->rotation.z)};

  float min = 0.0f, max = 360.0f;
  igDragScalarN("Rotation", ImGuiDataType_Float, v_deg.data, 3, .333f, &min, &max, "%.3f", 0);

  t->rotation.x = glm_rad(v_deg.x);
  t->rotation.y = glm_rad(v_deg.y);
  t->rotation.z = glm_rad(v_deg.z);

  igColorEdit4("Color", lab->colors[lab->selected].data, 0);

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
