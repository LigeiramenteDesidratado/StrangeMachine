#include "smpch.h"

#include "core/smCore.h"
#include "math/smMath.h"
#include "renderer/smOrthographicCamera.h"
#include "smInput.h"

void orthographic_camera_on_update(sm_orthographic_camera *camera, float dt) {

  if (input_scan_key(sm_key_a)) {

    camera->position.x -= cos(glm_rad(camera->rotation)) * 10.0f * dt;
    camera->position.y -= sin(glm_rad(camera->rotation)) * 10.0f * dt;

  } else if (input_scan_key(sm_key_d)) {

    camera->position.x += cos(glm_rad(camera->rotation)) * 10.0f * dt;
    camera->position.y += sin(glm_rad(camera->rotation)) * 10.0f * dt;
  }

  if (input_scan_key(sm_key_w)) {

    camera->position.x += -sin(glm_rad(camera->rotation)) * 10.0f * dt;
    camera->position.y += cos(glm_rad(camera->rotation)) * 10.0f * dt;

  } else if (input_scan_key(sm_key_s)) {

    camera->position.x -= -sin(glm_rad(camera->rotation)) * 10.0f * dt;
    camera->position.y -= cos(glm_rad(camera->rotation)) * 10.0f * dt;
  }

  if (camera->rotation) {

    if (input_scan_key(sm_key_q))
      camera->rotation += 3.0f * dt;
    if (input_scan_key(sm_key_e))
      camera->rotation -= 3.0f * dt;

    if (camera->rotation > 180.0f)
      camera->rotation -= 360.0f;
    else if (camera->rotation <= -180.0f)
      camera->rotation += 360.0f;
  }
}
