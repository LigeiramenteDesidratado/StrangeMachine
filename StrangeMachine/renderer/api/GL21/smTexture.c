#include "smpch.h"

#include "vendor/gladGL21/glad.h"

#include "renderer/api/GL21/smGLUtil.h"
#include "resource/smTextureResourcePub.h"

typedef struct {
  GLuint texture;

} texture_s;

texture_s *GL21texture_new(void) {
  texture_s *texture = SM_CALLOC(1, sizeof(texture_s));
  SM_ASSERT(texture);

  return texture;
}

b8 GL21texture_ctor(texture_s *texture, sm_texture_resource_handler_s handler) {

  SM_ASSERT(texture != NULL);
  SM_ASSERT(handler.handle != SM_INVALID_HANDLE);

  if (!sm_texture_resource_load_data(handler))
    return false;

  u32 width = sm_texture_resource_get_width(handler);
  u32 height = sm_texture_resource_get_height(handler);
  u32 channels = sm_texture_resource_get_channels(handler);
  const void *data = sm_texture_resource_get_data(handler);

  glCall(glGenTextures(1, &texture->texture));

  glCall(glBindTexture(GL_TEXTURE_2D, texture->texture));

  if (channels == 4)
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
  else if (channels == 3)
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
  else if (channels == 1)
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data));
  else
    SM_UNREACHABLE();

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

  SM_FREE(texture);
}

void GL21texture_bind(texture_s const *texture, u32 tex_index) {

  SM_ASSERT(texture != NULL);

  glCall(glActiveTexture(GL_TEXTURE0 + tex_index));
  glCall(glBindTexture(GL_TEXTURE_2D, texture->texture));
}

void GL21texture_unbind(texture_s const *texture, u32 tex_index) {

  SM_UNUSED(texture);

  glCall(glActiveTexture(GL_TEXTURE0 + tex_index));
  glCall(glBindTexture(GL_TEXTURE_2D, 0));
  glCall(glActiveTexture(GL_TEXTURE0));
}
