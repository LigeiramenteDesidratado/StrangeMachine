#ifndef SM_REARRANGE_BONES_H
#define SM_REARRANGE_BONES_H

#include "smClip.h"
#include "smSkinnedMesh.h"
#include "smSkeleton.h"

typedef struct {
  int32_t key;
  int32_t value;

} bone_map_s;

bone_map_s *rearrange_skeleton(struct skeleton_s *skeleton);
void rearrange_mesh(skinned_mesh_s *mesh, bone_map_s *bone_map);
void rearrange_clip(struct clip_s *clip, bone_map_s *bone_map);

#endif // SM_REARRANGE_BONES_H

