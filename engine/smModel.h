#ifndef SM_MODEL_H
#define SM_MODEL_H

#include <stdbool.h>

#include "smMesh.h"

struct model_s;

// Allocate memory
struct model_s *model_new(void);

// Constructor
bool model_ctor(struct model_s *model, const char* obj_path, const char* texture_path);

// Destructor
void model_dtor(struct model_s *model);

void model_do(struct model_s *model);
mesh_s** model_get_meshes(struct model_s *model);
void model_draw(const struct model_s *const model);

#endif // SM_MODEL_H
