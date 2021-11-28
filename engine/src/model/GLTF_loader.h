#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include "cgltf/cgltf.h"

#include "model/clip.h"
#include "model/pose.h"
#include "model/skeleton.h"
#include "model/skinned_mesh.h"

cgltf_data *gltf_loader_load_file(const char *path);

struct clip_s **gltf_loader_load_animation_clips(cgltf_data *data);

skinned_mesh_s *gltf_loader_load_meshes(cgltf_data *data);
struct skeleton_s *gltf_loader_load_skeleton(cgltf_data *data);

void gltf_loader_free_data(cgltf_data *data);

#endif // GLTF_LOADER_H
