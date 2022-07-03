#include "smpch.h"

#include "core/smCore.h"

#include "scene/smSystem.h"

const sm_system_flags_t SM_SYSTEM_INCLUSIVE_FLAG = (1ULL << 1);
const sm_system_flags_t SM_SYSTEM_EXCLUSIVE_FLAG = (1ULL << 2);

b8 system_iter_next(sm_system_iterator_s *iter) {

  if (iter->index >= iter->length) {
    return false;
  }

  iter->iter_data_count = 0;

  for (size_t i = 0; i < SM_ARRAY_LEN(iter->desc); i++) {
    const sm_component_view_s *desc = &iter->desc[i];
    iter->iter_data[i].data = ((u8 *)iter->data) + (iter->index * iter->size) + desc->offset;
    iter->iter_data[i].size = desc->size;
    iter->iter_data_count++;
  }

  iter->index++;

  return true;
}
