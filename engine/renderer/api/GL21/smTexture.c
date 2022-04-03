#include "smpch.h"

#include "vendor/gladGL21/glad.h"

#include "renderer/api/GL21/smGLUtil.h"

typedef struct {
  GLuint texture;

} texture_s;

texture_s *GL21texture_new(void) {
  texture_s *texture = SM_CALLOC(1, sizeof(texture_s));
  SM_ASSERT(texture);

  return texture;
}

// Constructor
bool GL21texture_ctor(texture_s *texture, uint32_t width, uint32_t height, void *data) {

  SM_ASSERT(texture != NULL);

  glCall(glGenTextures(1, &texture->texture));

  glCall(glBindTexture(GL_TEXTURE_2D, texture->texture));
  glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
  /* glGenerateMipmap(GL_TEXTURE_2D); */

  glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

  glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  glCall(glBindTexture(GL_TEXTURE_2D, 0));

  return true;
}

// Destructor
void GL21texture_dtor(texture_s *texture) {
  SM_ASSERT(texture != NULL);

  glCall(glDeleteTextures(1, &texture->texture));
}

void GL21texture_bind(texture_s const *texture, uint32_t tex_index) {

  SM_ASSERT(texture != NULL);

  glCall(glActiveTexture(GL_TEXTURE0 + tex_index));
  glCall(glBindTexture(GL_TEXTURE_2D, texture->texture));
}

void GL21texture_unbind(texture_s const *texture, uint32_t tex_index) {

  SM_UNUSED(texture);

  glCall(glActiveTexture(GL_TEXTURE0 + tex_index));
  glCall(glBindTexture(GL_TEXTURE_2D, 0));
  glCall(glActiveTexture(GL_TEXTURE0));
}
