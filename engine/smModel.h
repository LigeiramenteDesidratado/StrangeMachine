#ifndef SM_MODEL_H
#define SM_MODEL_H

#include "util/common.h"
#include "smMesh.h"

struct model_s;

// Allocate memory
struct model_s *model_new(void);

// Constructor
status_v model_ctor(struct model_s *model, const string obj_path,
                    const string texture_path);

// Destructor
void model_dtor(struct model_s *model);

void model_do(struct model_s *model);
mesh_s** model_get_meshes(struct model_s *model);
void model_draw(const struct model_s *const model);

#endif // SM_MODEL_H
