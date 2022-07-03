#include "smpch.h"

#include "core/smHandle.h"

const u32 sm__handle_index_mask = (1 << (32 - SM_CONFIG_HANDLE_GEN_BITS)) - 1;
const u32 sm__handle_gen_mask = ((1 << SM_CONFIG_HANDLE_GEN_BITS) - 1);
const u32 sm__handle_gen_shift = (32 - SM_CONFIG_HANDLE_GEN_BITS);
