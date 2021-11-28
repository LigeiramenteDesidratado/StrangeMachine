#ifndef REARRANGE_BONES_H
#define REARRANGE_BONES_H

#include "model/clip.h"
#include "model/skinned_mesh.h"
#include "model/skeleton.h"

typedef struct {
  int32_t key;
  int32_t value;

} bone_map_s;

bone_map_s *rearrange_skeleton(struct skeleton_s *skeleton);
void rearrange_mesh(skinned_mesh_s *mesh, bone_map_s *bone_map);
void rearrange_clip(struct clip_s *clip, bone_map_s *bone_map);

#endif // REARRANGE_BONES_H

