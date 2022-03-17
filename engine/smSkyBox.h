#ifndef SM_SKYBOX_H
#define SM_SKYBOX_H

#include <stdbool.h>
#include <stdint.h>

#include "glad/glad.h"
#include "smAttribute.h"

typedef struct {
  uint32_t width[6], height[6];
  uint32_t channels[6];
  GLuint texture;
  attribute_s position_attr; // VEC3_KIND

} skybox_s;

#define skybox_new()                                                                                                   \
  (skybox_s) {                                                                                                         \
    .width = {0}, .height = {0}, .channels = {0}, .texture = 0                                                         \
  }

bool skybox_ctor(skybox_s *skybox, const char *path[6]);
void skybox_dtor(skybox_s *skybox);
void skybox_draw(skybox_s const *skybox);

#endif /* SM_SKYBOX_H */
