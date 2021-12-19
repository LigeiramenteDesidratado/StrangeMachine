#include "smTexture.h"
#include "util/common.h"

bool __texture_load(texture_s *texture, const char *path);

// Constructor
bool texture_ctor(texture_s *texture, const char *path) {

  assert(texture != NULL);

  glGenTextures(1, &texture->texture);
  if (!__texture_load(texture, path))
    return false;

  return true;
}

// Destructor
void texture_dtor(texture_s *texture) {
  assert(texture != NULL);

  glDeleteTextures(1, &texture->texture);
}

void texture_set(texture_s const *texture, int32_t uniform, int32_t tex_index) {
  assert(texture != NULL);

  glActiveTexture(GL_TEXTURE0 + tex_index);
  glBindTexture(GL_TEXTURE_2D, texture->texture);
  glUniform1i(uniform, tex_index);
}

void texture_unset(uint32_t tex_index) {

  glActiveTexture(GL_TEXTURE0 + tex_index);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
}

bool __texture_load(texture_s *texture, const char *path) {
  assert(texture != NULL);

  int32_t width, height, channels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
  if (data == NULL) {
    log_error("[%s] failed to load image", path);
    return false;
  }
  log_info("[%s] image successfully loaded", path);

  glBindTexture(GL_TEXTURE_2D, texture->texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  /* glGenerateMipmap(GL_TEXTURE_2D); */
  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, 0);

  texture->width = width;
  texture->height = height;
  texture->channels = channels;

  return true;
}
