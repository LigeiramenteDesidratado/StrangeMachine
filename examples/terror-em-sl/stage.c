#include "util/common.h"

#include "scene/scene.h"
#include "scene/world/center.h"
#include "smCamera.h"
#include "smCameraP.h"
#include "smInput.h"
#include "smShader.h"
#include "smShaderProgram.h"
#include "smUniform.h"

typedef struct {

  SCENE_EX6 next_scene;
  struct scene_s *scene;

} stage_s;

static void __stage_tear_down_scene(stage_s *stage);
static void __stage_build_scene(stage_s *stage);

stage_s *stage_new(void) {
  stage_s *stage = calloc(1, sizeof(stage_s));
  assert(stage != NULL);

  return stage;
}

status_v stage_ctor(stage_s *stage) {

  assert(stage);

  stage->next_scene = CENTER_EX6;
  stage->scene = (struct scene_s *)center_new();
  if (!center_ctor((struct center_s *)stage->scene, CENTER_EX6)) {
    return fail;
  }

  camera_init(vec3_new(0.0f, 3.0f, 8.0f), vec3_new(0.0f, 2.0f, 0.0f),
              vec3_new(0.0f, 1.0f, 0.0f), THIRD_PERSON_EX5, PERSPECTIVE_EX4);

  return ok;
}

void stage_dtor(stage_s *stage) {
  assert(stage);

  stage->next_scene = MAX_SCENES_EX6;
  __stage_tear_down_scene(stage);
  camera_tear_down();
  scene_dtor(stage->scene);

  free(stage);
  stage = NULL;
}

void stage_do(stage_s *stage, float dt) {
  assert(stage != NULL);

  if (scene_get_id(stage->scene) != stage->next_scene) {
    __stage_tear_down_scene(stage);
    __stage_build_scene(stage);
  }

  if (input_scan_key_lock(SDL_SCANCODE_F))
    camera_set_mode(FREE_EX5);

  if (input_scan_key_lock(SDL_SCANCODE_T))
    camera_set_mode(THIRD_PERSON_EX5);

  vec3 loc = scene_get_look_at(stage->scene);
  loc.y += 4.5;
  camera_set_target(loc);

  MODE_EX5 cam_mode = camera_get_mode();
  if (cam_mode == THIRD_PERSON_EX5)
    scene_do(stage->scene, dt);

  camera_do(dt);
}

void stage_draw(stage_s *stage, float aspect_ratio) {
  assert(stage != NULL);

  if (input_scan_key_lock(SDL_SCANCODE_J))
    camera_set_projection(PERSPECTIVE_EX4);

  if (input_scan_key_lock(SDL_SCANCODE_K))
    camera_set_projection(ORTHOGONAL_EX4);

  mat4 projection = camera_get_projection_matrix(aspect_ratio);
  mat4 view = camera_get_view();

  shader_bind(SHADERS[STATIC_SHADER_EX7]);
  uniform_set_value(glGetUniformLocation(SHADERS[STATIC_SHADER_EX7], "view"),
                    view);
  uniform_set_value(
      glGetUniformLocation(SHADERS[STATIC_SHADER_EX7], "projection"),
      projection);
  shader_unbind();

  shader_bind(SHADERS[SKINNED_SHADER_EX7]);
  uniform_set_value(glGetUniformLocation(SHADERS[SKINNED_SHADER_EX7], "view"),
                    view);
  uniform_set_value(
      glGetUniformLocation(SHADERS[SKINNED_SHADER_EX7], "projection"),
      projection);
  shader_unbind();

  shader_bind(SHADERS[TEXT_SHADER_EX7]);
  mat4 ui = mat4_ortho(0.0f, 800.0f, 600.0f, 0.0f, -0.1f, 100.0f);
  /* uniform_set_value(glGetUniformLocation(SHADERS[TEXT_SHADER_EX7], "view"),
   * view); */
  uniform_set_value(
      glGetUniformLocation(SHADERS[TEXT_SHADER_EX7], "projection"), ui);
  shader_unbind();

  shader_bind(SHADERS[DEBUG_SHADER_EX7]);
  uniform_set_value(glGetUniformLocation(SHADERS[DEBUG_SHADER_EX7], "view"),
                    view);
  uniform_set_value(
      glGetUniformLocation(SHADERS[DEBUG_SHADER_EX7], "projection"),
      projection);
  shader_unbind();

  scene_draw(stage->scene);
}

static void __stage_build_scene(stage_s *stage) {

  if (stage->scene != NULL) {
    return;
  }

  switch (stage->next_scene) {
  case MENU_EX6:
    // TODO: buiçd menu

    break;

  case CENTER_EX6:
    stage->scene = (struct scene_s *)center_new();
    center_ctor((struct center_s *)stage->scene, CENTER_EX6);
    break;

  case MAX_SCENES_EX6:
  default:
    /* stage->scene = (struct scene_s *)menu_new(); */
    /* menu_ctor((struct menu_s *)stage->scene, MENU_EX6); */
    break;
  }
}

static void __stage_tear_down_scene(stage_s *stage) {

  switch (scene_get_id(stage->scene)) {
  case MENU_EX6:
    if (stage->next_scene != MENU_EX6) {
      /* camera_dtor(stage->camera); */
      /* free(stage->camera); */

      /* menu_dtor((struct menu_s *)stage->scene); */
      /* free((struct menu_t *)stage->scene); */
      /* stage->scene = NULL; */
    }
    break;

  case CENTER_EX6:
    if (stage->next_scene != CENTER_EX6) {
      center_dtor((struct center_s *)stage->scene);
      stage->scene = NULL;
    }
    break;
  case MAX_SCENES_EX6:
  default:
    break;
  }
}
