#ifndef SM_CORE_HANDLE_H
#define SM_CORE_HANDLE_H

#include "smpch.h"

typedef uint32_t sm_handle;

#define SM_CONFIG_HANDLE_GEN_BITS 14
#define SM_INVALID_HANDLE         0

extern const uint32_t sm__handle_index_mask;
extern const uint32_t sm__handle_gen_mask;
extern const uint32_t sm__handle_gen_shift;

#define sm_handle_index(HANDLE) (uint32_t)((HANDLE)&sm__handle_index_mask)
#define sm_handle_gen(HANDLE)   (uint32_t)(((HANDLE) >> sm__handle_gen_shift) & sm__handle_gen_mask)
#define sm__handle_make(GEN, INDEX)                                                                                    \
  (uint32_t)((((uint32_t)(GEN)&sm__handle_gen_mask) << sm__handle_gen_shift) |                                         \
             ((uint32_t)(INDEX)&sm__handle_index_mask))

#endif /* SM_CORE_HANDLE_H */
