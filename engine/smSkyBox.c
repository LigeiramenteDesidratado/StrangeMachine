#include "util/common.h"

#include "smShader.h"
#include "smShaderProgram.h"
#include "smSkyBox.h"

static vec3 positions[36] = {
    // positions          
  {-1.0f*32,  1.0f*32, -1.0f*32},
  {-1.0f*32, -1.0f*32, -1.0f*32},
  {1.0f*32, -1.0f*32, -1.0f*32},
  {1.0f*32, -1.0f*32, -1.0f*32},
  {1.0f*32,  1.0f*32, -1.0f*32},
  {-1.0f*32,  1.0f*32, -1.0f*32},

  {-1.0f*32, -1.0f*32,  1.0f*32},
   {-1.0f*32, -1.0f*32, -1.0f*32},
   {-1.0f*32,  1.0f*32, -1.0f*32},
   {-1.0f*32,  1.0f*32, -1.0f*32},
   {-1.0f*32,  1.0f*32,  1.0f*32},
   {-1.0f*32, -1.0f*32,  1.0f*32},

   {1.0f*32, -1.0f*32, -1.0f*32},
   {1.0f*32, -1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32, -1.0f*32},
   {1.0f*32, -1.0f*32, -1.0f*32},

   {-1.0f*32, -1.0f*32,  1.0f*32},
   {-1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32, -1.0f*32,  1.0f*32},
   {-1.0f*32, -1.0f*32,  1.0f*32},

   {-1.0f*32,  1.0f*32, -1.0f*32},
   {1.0f*32,  1.0f*32, -1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {1.0f*32,  1.0f*32,  1.0f*32},
   {-1.0f*32,  1.0f*32,  1.0f*32},
   {-1.0f*32,  1.0f*32, -1.0f*32},

   {-1.0f*32, -1.0f*32, -1.0f*32},
   {-1.0f*32, -1.0f*32,  1.0f*32},
   {1.0f*32, -1.0f*32, -1.0f*32},
   {1.0f*32, -1.0f*32, -1.0f*32},
   {-1.0f*32, -1.0f*32,  1.0f*32},
   {1.0f*32, -1.0f*32,  1.0f*32}
};

bool __skybox_load(skybox_s *texture, const char *path[6]);

// Constructor
bool skybox_ctor(skybox_s *skybox, const char *path[6]) {

  SM_ASSERT(skybox != NULL);

  skybox->position_attr = attribute_new();
  if (!attribute_ctor(&skybox->position_attr, VEC3_KIND))
    return false;

  attribute_set(&skybox->position_attr, positions, 36, GL_STATIC_DRAW);

  glGenTextures(1, &skybox->texture);
  if (!__skybox_load(skybox, path))
    return false;

  return true;
}

void skybox_dtor(skybox_s *skybox) {
  SM_ASSERT(skybox != NULL);

  glDeleteTextures(1, &skybox->texture);
  attribute_dtor(&skybox->position_attr);
}

void skybox_draw(skybox_s const *skybox) {

  glDepthMask(GL_FALSE);
  shader_bind(SHADERS[SKYBOX_SHADER]);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
  attribute_bind_to(&skybox->position_attr, 1);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  attribute_unbind_from(&skybox->position_attr, 1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glDepthMask(GL_TRUE);
  shader_unbind();
}

bool __skybox_load(skybox_s *skybox, const char *path[6]) {
  SM_ASSERT(skybox != NULL);

  int32_t width, height, channels;
  stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = NULL;

  for (int i = 0; i < 6; ++i) {

    data = stbi_load(path[i], &width, &height, &channels, 4);
    if (data == NULL) {
      SM_LOG_ERROR("[%s] failed to load image", path[i]);
      return false;
    }
    SM_LOG_INFO("[%s] image successfully loaded", path[i]);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    /* glGenerateMipmap(GL_TEXTURE_2D); */
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    skybox->width[i] = width;
    skybox->height[i] = height;
    skybox->channels[i] = channels;
  }

  return true;
}
