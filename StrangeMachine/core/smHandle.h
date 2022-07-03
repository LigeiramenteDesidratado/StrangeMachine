#ifndef SM_CORE_HANDLE_H
#define SM_CORE_HANDLE_H

#include "smpch.h"

typedef u32 sm_handle_t;

#define SM_CONFIG_HANDLE_GEN_BITS 14
#define SM_INVALID_HANDLE         0

extern const u32 sm__handle_index_mask;
extern const u32 sm__handle_gen_mask;
extern const u32 sm__handle_gen_shift;

#define sm_handle_index(HANDLE) (u32)((HANDLE)&sm__handle_index_mask)
#define sm_handle_gen(HANDLE)   (u32)(((HANDLE) >> sm__handle_gen_shift) & sm__handle_gen_mask)
#define sm__handle_make(GEN, INDEX)                                                                                    \
  (u32)((((u32)(GEN)&sm__handle_gen_mask) << sm__handle_gen_shift) | ((u32)(INDEX)&sm__handle_index_mask))

#endif /* SM_CORE_HANDLE_H */
