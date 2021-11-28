#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "util/common.h"
#include "model/mesh.h"

status_v obj_loader_load(mesh_s **meshes, const string path);

#endif // OBJ_LOADER_H 
