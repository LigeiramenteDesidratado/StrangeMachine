#ifndef SM_SKINNED_MODEL_H
#define SM_SKINNED_MODEL_H

#include <stdbool.h>

struct skinned_model_s;

struct skinned_model_s *skinned_model_new(void);

// Constructor
bool skinned_model_ctor(struct skinned_model_s *skinned_model, const char *gltf_path, const char *texture_path);
bool skinned_model_ctor2(struct skinned_model_s *skinned_model, const char *gltf_path, const char *texture_path);

// Destructor
void skinned_model_dtor(struct skinned_model_s *skinned_model);

void skinned_model_do(struct skinned_model_s *skinned_model, float dt);
void skinned_model_draw(struct skinned_model_s *skinned_model);
bool skinned_model_set_animation(struct skinned_model_s *skinned_model, const char *animation);
char **skinned_model_get_animations(struct skinned_model_s *skinned_model);

#endif // SM_SKINNED_MODEL_H
