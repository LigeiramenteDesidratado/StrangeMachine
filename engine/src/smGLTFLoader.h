#ifndef SM_GLTF_LOADER_H
#define SM_GLTF_LOADER_H

#include "cgltf/cgltf.h"

#include "smClip.h"
#include "smPose.h"
#include "smSkeleton.h"
#include "smSkinnedMesh.h"

cgltf_data *gltf_loader_load_file(const char *path);

struct clip_s **gltf_loader_load_animation_clips(cgltf_data *data);

skinned_mesh_s *gltf_loader_load_meshes(cgltf_data *data);
struct skeleton_s *gltf_loader_load_skeleton(cgltf_data *data);

void gltf_loader_free_data(cgltf_data *data);

#endif // SM_GLTF_LOADER_H
