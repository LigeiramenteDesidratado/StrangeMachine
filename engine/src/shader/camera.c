
#include "input/input.h"
#include "math/mat4.h"
#include "shader/camera_p.h"
#include "util/common.h"
#include <SDL2/SDL_events.h>

#define CAMERA_THIRD_PERSON_DISTANCE_CLAMP 1.2f
#define CAMERA_THIRD_PERSON_MIN_CLAMP -5.0f
#define CAMERA_THIRD_PERSON_MAX_CLAMP -70.0f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 1.4f

typedef struct {
  MODE_EX5 mode;
  PROJECTION_EX4 projection;

  vec3 position;
  vec3 target, _next_target; // looks-at or front
  vec3 up;
  vec3 right;
  float fovy;
  // pitch, yaw, roll
  vec3 angle;
  float target_distance;
  float sensitive;
  float move_speed;

} camera_s;

camera_s CAMERA;

bool is_first = true;
camera_s last_third_person_camera = {0};
camera_s last_free_camera = {0};

void camera_update_vectors(float dt);

void camera_init(vec3 position, vec3 target, vec3 up, MODE_EX5 mode, PROJECTION_EX4 projection) {

  CAMERA.move_speed = 12.0f;
  CAMERA.sensitive = 1.4f;
  CAMERA.target_distance = 10.2f;
  CAMERA.fovy = 75.0f;

  CAMERA.up = up;
  CAMERA.position = position;
  CAMERA.target = target;
  CAMERA.mode = mode;
  CAMERA.projection = projection;

}

// Destructor
void camera_tear_down(void) {

  CAMERA = (camera_s){0};
}

void camera_do(float dt) {

  if (CAMERA.mode == THIRD_PERSON_EX5) {

    float offset_x = input_get_x_rel_mouse();
    float offset_y = input_get_y_rel_mouse();

    if (offset_x != 0 || offset_y != 0) {
      CAMERA.angle.x += (offset_x * -CAMERA.sensitive * dt);
      CAMERA.angle.y += (offset_y * -CAMERA.sensitive * dt);

      if (CAMERA.angle.y > CAMERA_THIRD_PERSON_MIN_CLAMP * DEG2RAD)
        CAMERA.angle.y = CAMERA_THIRD_PERSON_MIN_CLAMP * DEG2RAD;
      else if (CAMERA.angle.y < CAMERA_THIRD_PERSON_MAX_CLAMP * DEG2RAD)
        CAMERA.angle.y = CAMERA_THIRD_PERSON_MAX_CLAMP * DEG2RAD;
    }
  }

  if (CAMERA.mode == FREE_EX5) {
    if (input_scan_key(SDL_SCANCODE_W))
      CAMERA.position =
          vec3_add(CAMERA.position,
                   vec3_scale(CAMERA.target, CAMERA.move_speed * dt));

    if (input_scan_key(SDL_SCANCODE_S))
      CAMERA.position =
          vec3_sub(CAMERA.position,
                   vec3_scale(CAMERA.target, CAMERA.move_speed * dt));

    if (input_scan_key(SDL_SCANCODE_A))
      CAMERA.position = vec3_sub(
          CAMERA.position, vec3_scale(CAMERA.right, CAMERA.move_speed * dt));

    if (input_scan_key(SDL_SCANCODE_D))
      CAMERA.position = vec3_add(
          CAMERA.position, vec3_scale(CAMERA.right, CAMERA.move_speed * dt));

    if (input_scan_key(SDL_SCANCODE_SPACE))
      CAMERA.position = vec3_add(
          CAMERA.position, vec3_scale(CAMERA.up, CAMERA.move_speed * dt));

    if (input_scan_key(SDL_SCANCODE_LSHIFT))
      CAMERA.position = vec3_sub(
          CAMERA.position, vec3_scale(CAMERA.up, CAMERA.move_speed * dt));

    float offset_x = input_get_x_rel_mouse();
    float offset_y = input_get_y_rel_mouse();

    if (offset_x != 0 || offset_y != 0) {
      CAMERA.angle.yaw += offset_x * 11.3 * dt;
      CAMERA.angle.pitch -= offset_y * 11.3 * dt;

      if (CAMERA.angle.pitch > 80.0f)
        CAMERA.angle.pitch = 80.0f;
      if (CAMERA.angle.pitch < -80.0f)
        CAMERA.angle.pitch = -80.0f;

      if (CAMERA.angle.yaw > 360.0f || CAMERA.angle.yaw < -360.0f)
        CAMERA.angle.yaw = 0;
    }
  }

  camera_update_vectors(dt);
}

void camera_set_target(vec3 target) {
  CAMERA._next_target = target;
}

void camera_update_vectors(float dt) {

  if (CAMERA.mode == THIRD_PERSON_EX5) {
    CAMERA.target = vec3_lerp(CAMERA.target, CAMERA._next_target, 10 * dt);

    CAMERA.target_distance -=
        (input_get_mouse_scroll() * CAMERA_MOUSE_SCROLL_SENSITIVITY);

    if (CAMERA.target_distance < CAMERA_THIRD_PERSON_DISTANCE_CLAMP)
      CAMERA.target_distance = CAMERA_THIRD_PERSON_DISTANCE_CLAMP;

    // TODO: It seems camera.position is not correctly updated or some rounding
    // issue makes the camera move straight to camera.target...
    vec3 nex_position = {0};
    nex_position.x = sinf(CAMERA.angle.x) * CAMERA.target_distance *
                         cosf(CAMERA.angle.y) +
                     CAMERA.target.x;

    if (CAMERA.angle.y <= 0.0f)
      nex_position.y = sinf(CAMERA.angle.y) * CAMERA.target_distance *
                           sinf(CAMERA.angle.y) +
                       CAMERA.target.y;
    else
      nex_position.y = -sinf(CAMERA.angle.y) * CAMERA.target_distance *
                           sinf(CAMERA.angle.y) +
                       CAMERA.target.y;

    nex_position.z = cosf(CAMERA.angle.x) * CAMERA.target_distance *
                         cosf(CAMERA.angle.y) +
                     CAMERA.target.z;

    CAMERA.position = vec3_lerp(CAMERA.position, nex_position, 10 * dt);
  }

  if (CAMERA.mode == FREE_EX5) {

    vec3 front;
    front.x = (cosf(CAMERA.angle.yaw * DEG2RAD) *
               cosf(CAMERA.angle.pitch * DEG2RAD));
    front.y = sinf(CAMERA.angle.pitch * DEG2RAD);
    front.z =
        sinf(CAMERA.angle.yaw * DEG2RAD) * cosf(CAMERA.angle.pitch * DEG2RAD);

    CAMERA.target = vec3_norm(front);
    CAMERA.right =
        vec3_norm(vec3_cross(CAMERA.target, vec3_new(0.0f, 1.0f, 0.0f)));
    CAMERA.up = vec3_norm(vec3_cross(CAMERA.right, CAMERA.target));
  }
}

mat4 camera_get_view(void) {

  mat4 view;
  if (CAMERA.mode == THIRD_PERSON_EX5)
    view = mat4_look_at(CAMERA.position, CAMERA.target, CAMERA.up);
  else
    view = mat4_look_at(CAMERA.position,
                        vec3_add(CAMERA.position, CAMERA.target), CAMERA.up);

  return view;
}

void camera_set_projection(PROJECTION_EX4 projection) {
  CAMERA.projection = projection;
}

void camera_set_mode(MODE_EX5 mode) {

  if (CAMERA.mode != mode) {
    if (mode == THIRD_PERSON_EX5) {
      last_free_camera = CAMERA;
    } else
      last_third_person_camera = CAMERA;
  } else
    return;

  if (mode == THIRD_PERSON_EX5) {
    if (last_third_person_camera.move_speed != 0.0f) {
      CAMERA = last_third_person_camera;
    }
    CAMERA.mode = mode;
  }
  if (mode == FREE_EX5) {
    if (last_free_camera.move_speed != 0.0f)
      CAMERA = last_free_camera;

    CAMERA.mode = mode;
  }
}

mat4 camera_get_projection_matrix(float aspect_ratio) {

  mat4 projection;
  if (CAMERA.projection == PERSPECTIVE_EX4) {
    projection = mat4_perspective(CAMERA.fovy, aspect_ratio, 0.01f, 100.0f);

  } else  {

    // https://stackoverflow.com/a/55009832
    float ratio_size_per_depth = atan(DEG2RAD*(CAMERA.fovy / 2.0f)) * 2.0f;
    float distance = vec3_len(vec3_sub(CAMERA.target, CAMERA.position));

    float size_y = ratio_size_per_depth * distance;
    float size_x = ratio_size_per_depth * distance * aspect_ratio;

    projection = mat4_ortho(-size_x, size_x, -size_y, size_y, -50.0f, 100.00f);
  }

  return projection;
}

float camera_get_fovy() {
  return CAMERA.fovy;
}

PROJECTION_EX4 camera_get_projection() {

  return CAMERA.projection;
}

MODE_EX5 camera_get_mode() {

  return CAMERA.mode;
}
