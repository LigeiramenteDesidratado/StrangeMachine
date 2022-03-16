#include "util/common.h"

#include "smCameraP.h"
#include "smInput.h"

#include <SDL2/SDL_events.h>

#define CAMERA_THIRD_PERSON_DISTANCE_CLAMP 1.2f
#define CAMERA_THIRD_PERSON_MIN_CLAMP      -5.0f
#define CAMERA_THIRD_PERSON_MAX_CLAMP      -70.0f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY    1.4f

typedef struct {
  cam_mode_e mode;
  cam_projection_e projection;

  vec3 position;
  vec3 target, _next_target; // looks-at or front
  vec3 up;
  vec3 right;
  float fovy;
  vec3 angle; // pitch, yaw, roll
  float target_distance;
  float sensitive;
  float move_speed;

} camera_s;

static camera_s CAMERA = {0};

bool is_first = true;
static camera_s last_third_person_camera = {0};
static camera_s last_free_camera = {0};

void camera_update_vectors(float dt);

void camera_init(vec3 position, vec3 target, vec3 up, cam_mode_e mode, cam_projection_e projection) {

  CAMERA.move_speed = 12.0f;
  CAMERA.sensitive = 1.4f;
  CAMERA.target_distance = 10.2f;
  CAMERA.fovy = 75.0f;

  glm_vec3_copy(position, CAMERA.position);
  glm_vec3_copy(target, CAMERA.target);
  glm_vec3_copy(up, CAMERA.up);

  CAMERA.mode = mode;
  CAMERA.projection = projection;
}

// Destructor
void camera_tear_down(void) {
  CAMERA = (camera_s){0};
}

void camera_do(float dt) {

  if (CAMERA.mode == THIRD_PERSON) {

    float offset_x = input_get_x_rel_mouse();
    float offset_y = input_get_y_rel_mouse();

    if (offset_x != 0 || offset_y != 0) {
      CAMERA.angle[0] += (offset_x * -CAMERA.sensitive * dt);
      CAMERA.angle[1] += (offset_y * -CAMERA.sensitive * dt);

      if (CAMERA.angle[1] > glm_rad(CAMERA_THIRD_PERSON_MIN_CLAMP))
        CAMERA.angle[1] = glm_rad(CAMERA_THIRD_PERSON_MIN_CLAMP);
      else if (CAMERA.angle[1] < glm_rad(CAMERA_THIRD_PERSON_MAX_CLAMP))
        CAMERA.angle[1] = glm_rad(CAMERA_THIRD_PERSON_MAX_CLAMP);
    }
  }

  if (CAMERA.mode == FREE) {

    vec3 pos;

    if (input_scan_key(sm_key_w)) {
      glm_vec3_scale(CAMERA.target, CAMERA.move_speed * dt, pos);
      glm_vec3_add(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_add(CAMERA.position, vec3_scale(CAMERA.target, CAMERA.move_speed * dt)); */
    }

    if (input_scan_key(sm_key_s)) {
      glm_vec3_scale(CAMERA.target, CAMERA.move_speed * dt, pos);
      glm_vec3_sub(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_sub(CAMERA.position, vec3_scale(CAMERA.target, CAMERA.move_speed * dt)); */
    }

    if (input_scan_key(sm_key_a)) {
      glm_vec3_scale(CAMERA.right, CAMERA.move_speed * dt, pos);
      glm_vec3_sub(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_sub(CAMERA.position, vec3_scale(CAMERA.right, CAMERA.move_speed * dt)); */
    }

    if (input_scan_key(sm_key_d)) {
      glm_vec3_scale(CAMERA.right, CAMERA.move_speed * dt, pos);
      glm_vec3_add(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_add(CAMERA.position, vec3_scale(CAMERA.right, CAMERA.move_speed * dt)); */
    }

    if (input_scan_key(sm_key_space)) {
      glm_vec3_scale(CAMERA.up, CAMERA.move_speed * dt, pos);
      glm_vec3_add(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_add(CAMERA.position, vec3_scale(CAMERA.up, CAMERA.move_speed * dt)); */
    }
    if (input_scan_key(sm_key_lshift)) {
      glm_vec3_scale(CAMERA.up, CAMERA.move_speed * dt, pos);
      glm_vec3_sub(CAMERA.position, pos, CAMERA.position);
      /* CAMERA.position = vec3_sub(CAMERA.position, vec3_scale(CAMERA.up, CAMERA.move_speed * dt)); */
    }

    float offset_x = input_get_x_rel_mouse();
    float offset_y = input_get_y_rel_mouse();

    if (offset_x != 0 || offset_y != 0) {
      CAMERA.angle[1] += offset_x * 11.3f * dt;
      CAMERA.angle[0] -= offset_y * 11.3f * dt;

      if (CAMERA.angle[0] > 80.0f)
        CAMERA.angle[0] = 80.0f;
      if (CAMERA.angle[0] < -80.0f)
        CAMERA.angle[0] = -80.0f;

      if (CAMERA.angle[1] > 360.0f || CAMERA.angle[1] < -360.0f)
        CAMERA.angle[1] = 0.0f;
    }
  }

  camera_update_vectors(dt);
}

void camera_set_target(vec3 target) {
  glm_vec3_copy(target, CAMERA._next_target);
  /* CAMERA._next_target = target; */
}

void camera_update_vectors(float dt) {

  if (CAMERA.mode == THIRD_PERSON) {
    glm_vec3_lerp(CAMERA.target, CAMERA._next_target, 10 * dt, CAMERA.target);

    CAMERA.target_distance -= (input_get_mouse_scroll() * CAMERA_MOUSE_SCROLL_SENSITIVITY);

    if (CAMERA.target_distance < CAMERA_THIRD_PERSON_DISTANCE_CLAMP)
      CAMERA.target_distance = CAMERA_THIRD_PERSON_DISTANCE_CLAMP;

    // TODO: It seems camera.position is not correctly updated or some rounding
    // issue makes the camera move straight to camera.target...
    vec3 nex_position = {0};
    nex_position[0] = sinf(CAMERA.angle[0]) * CAMERA.target_distance * cosf(CAMERA.angle[1]) + CAMERA.target[0];

    if (CAMERA.angle[1] <= 0.0f)
      nex_position[1] = sinf(CAMERA.angle[1]) * CAMERA.target_distance * sinf(CAMERA.angle[1]) + CAMERA.target[1];
    else
      nex_position[1] = -sinf(CAMERA.angle[1]) * CAMERA.target_distance * sinf(CAMERA.angle[1]) + CAMERA.target[1];

    nex_position[2] = cosf(CAMERA.angle[0]) * CAMERA.target_distance * cosf(CAMERA.angle[1]) + CAMERA.target[2];

    glm_vec3_lerp(CAMERA.position, nex_position, 10 * dt, CAMERA.position);
  }

  if (CAMERA.mode == FREE) {

    vec3 front;
    front[0] = cosf(glm_rad(CAMERA.angle[1])) * glm_rad(cosf(CAMERA.angle[0]));
    front[1] = sinf(glm_rad(CAMERA.angle[0]));
    front[2] = sinf(glm_rad(CAMERA.angle[1])) * cosf(glm_rad(CAMERA.angle[0]));

    glm_vec3_normalize_to(front, CAMERA.target);
    vec3 right;
    glm_vec3_cross(CAMERA.target, (vec3){0.0f, 1.0f, 0.0f}, right);
    glm_vec3_normalize_to(right, CAMERA.right);
    vec3 up;
    glm_vec3_cross(CAMERA.right, CAMERA.target, up);
    glm_vec3_normalize_to(up, CAMERA.up);
  }
}

void camera_get_view(mat4 out_view) {

  if (CAMERA.mode == THIRD_PERSON)
    glm_lookat(CAMERA.position, CAMERA.target, CAMERA.up, out_view);
  /* view = glm_mat4_look_at(CAMERA.position, CAMERA.target, CAMERA.up); */
  else {
    vec3 dist;
    glm_vec3_sub(CAMERA.position, CAMERA.target, dist);
    glm_lookat(CAMERA.position, dist, CAMERA.up, out_view);
    /* view = mat4_look_at(CAMERA.position, vec3_add(CAMERA.position, CAMERA.target), CAMERA.up); */
  }
}

void camera_set_projection(cam_projection_e projection) {
  CAMERA.projection = projection;
}

void camera_set_mode(cam_mode_e mode) {

  if (CAMERA.mode != mode) {
    if (mode == THIRD_PERSON) {
      last_free_camera = CAMERA;
    } else
      last_third_person_camera = CAMERA;
  } else
    return;

  if (mode == THIRD_PERSON) {
    if (last_third_person_camera.move_speed != 0.0f) {
      CAMERA = last_third_person_camera;
    }
    CAMERA.mode = mode;
  }
  if (mode == FREE) {
    if (last_free_camera.move_speed != 0.0f)
      CAMERA = last_free_camera;

    CAMERA.mode = mode;
  }
}

void camera_get_projection_matrix(float aspect_ratio, mat4 out_projection) {

  if (CAMERA.projection == PERSPECTIVE) {
    glm_perspective(glm_rad(CAMERA.fovy), aspect_ratio, 0.01f, 100.0f, out_projection);

  } else {

    // https://stackoverflow.com/a/55009832
    float ratio_size_per_depth = atanf(glm_rad(CAMERA.fovy / 2.0f)) * 2.0f;
    /* float ratio_size_per_depth = atanf(DEG2RAD * (CAMERA.fovy / 2.0f)) * 2.0f; */
    vec3 dist;
    glm_vec3_sub(CAMERA.target, CAMERA.position, dist);
    float distance = glm_vec3_norm(dist);

    /* float distance = vec3_len(vec3_sub(CAMERA.target, CAMERA.position)); */

    float size_y = ratio_size_per_depth * distance;
    float size_x = ratio_size_per_depth * distance * aspect_ratio;

    glm_ortho(-size_x, size_x, -size_y, size_y, 0.01f, 100.0f, out_projection);
    /* projection = mat4_ortho(-size_x, size_x, -size_y, size_y, -50.0f, 100.00f); */
  }
}

float camera_get_fovy() {
  return CAMERA.fovy;
}

cam_projection_e camera_get_projection() {
  return CAMERA.projection;
}

cam_mode_e camera_get_mode() {
  return CAMERA.mode;
}
