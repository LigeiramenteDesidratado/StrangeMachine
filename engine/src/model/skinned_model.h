#ifndef SKINNED_MODEL_H
#define SKINNED_MODEL_H

#include "util/common.h"

struct skinned_model_s;

struct skinned_model_s *skinned_model_new(void);

// Constructor
status_v skinned_model_ctor(struct skinned_model_s *skinned_model,
                            const string gltf_path, const string texture_path);

// Destructor
void skinned_model_dtor(struct skinned_model_s *skinned_model);

void skinned_model_do(struct skinned_model_s *skinned_model, float dt);
void skinned_model_draw(struct skinned_model_s *skinned_model);
status_v skinned_model_set_animation(struct skinned_model_s *skinned_model, const string animation);

#endif // SKINNED_MODEL_H
