#include "data/array.h"
#include "util/common.h"

#include "smRearrangeBones.h"

bone_map_s *rearrange_skeleton(struct skeleton_s *skeleton) {
  pose_s *rest_pose = skeleton_get_rest_pose(skeleton);
  pose_s *bind_pose = skeleton_get_bind_pose(skeleton);

  size_t size = SM_ARRAY_SIZE(rest_pose->nodes);
  if (size == 0)
    return NULL;

  int32_t **hierarchy = NULL;
  SM_ARRAY_SET_SIZE(hierarchy, size);
  for (size_t i = 0; i < SM_ARRAY_SIZE(hierarchy); ++i) {
    hierarchy[i] = NULL;
  }

  // TODO: investigate
  int32_t *process = (int32_t *)SM_ARRAY_NEW_EMPTY();
  for (size_t i = 0; i < size; ++i) {
    int32_t parent = pose_get_parent(rest_pose, i);
    if (parent >= 0) {
      SM_ARRAY_PUSH(hierarchy[parent], i);
    } else {
      SM_ARRAY_PUSH(process, i);
    }
  }

  bone_map_s *map_forward = NULL;
  bone_map_s *map_backward = NULL;

  int32_t index = 0;
  while (SM_ARRAY_SIZE(process) > 0) {
    int32_t current = process[0];
    /* arrdel(process, 0); */
    SM_ARRAY_DEL(process, 0, 1);
    int32_t *children = hierarchy[current];
    size_t num_children = SM_ARRAY_SIZE(children);
    for (size_t i = 0; i < num_children; ++i) {
      SM_ARRAY_PUSH(process, children[i]);
    }

    hmput(map_forward, index, current);
    hmput(map_backward, current, index);
    index += 1;
  }
  hmput(map_forward, -1, -1);
  hmput(map_backward, -1, -1);

  SM_ARRAY_DTOR(process);

  for (size_t i = 0; i < SM_ARRAY_SIZE(hierarchy); ++i) {
    SM_ARRAY_DTOR(hierarchy[i]);
  }
  SM_ARRAY_DTOR(hierarchy);

  pose_s new_rest_pose = pose_new();
  pose_resize(&new_rest_pose, size);

  pose_s new_bind_pose = pose_new();
  pose_resize(&new_bind_pose, size);

  char **new_names = NULL;
  SM_ARRAY_SET_SIZE(new_names, size);

  for (size_t i = 0; i < size; ++i) {
    int32_t this_bone = hmget(map_forward, i);
    pose_set_local_transform(&new_rest_pose, i, pose_get_local_transform(rest_pose, this_bone));
    pose_set_local_transform(&new_bind_pose, i, pose_get_local_transform(bind_pose, this_bone));

    new_names[i] = skeleton_get_joint_name(skeleton, this_bone);
    int32_t parent = hmget(map_backward, pose_get_parent(bind_pose, this_bone));

    pose_set_parent(&new_rest_pose, i, parent);
    pose_set_parent(&new_bind_pose, i, parent);
  }

  skeleton_set(skeleton, &new_rest_pose, &new_bind_pose, (const char **)new_names);

  hmfree(map_forward);

  pose_dtor(&new_rest_pose);
  pose_dtor(&new_bind_pose);
  SM_ARRAY_DTOR(new_names);

  return map_backward;
}

void rearrange_mesh(skinned_mesh_s *mesh, bone_map_s *bone_map) {

  for (size_t i = 0; i < SM_ARRAY_SIZE(mesh->influences); ++i) {
    mesh->influences[i][0] = hmget(bone_map, mesh->influences[i][0]);
    mesh->influences[i][1] = hmget(bone_map, mesh->influences[i][1]);
    mesh->influences[i][2] = hmget(bone_map, mesh->influences[i][2]);
    mesh->influences[i][3] = hmget(bone_map, mesh->influences[i][3]);
  }

  skinned_mesh_update_gl_buffers(mesh);
}

void rearrange_clip(struct clip_s *clip, bone_map_s *bone_map) {
  size_t size = clip_get_size(clip);
  for (size_t i = 0; i < size; ++i) {
    int32_t joint = (int32_t)clip_get_id_at_index(clip, i);
    uint32_t new_joint = (uint32_t)hmget(bone_map, joint);
    clip_set_id_at_index(clip, i, new_joint);
  }
}
