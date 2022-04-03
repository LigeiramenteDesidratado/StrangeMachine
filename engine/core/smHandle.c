#include "smpch.h"

#include "core/smHandle.h"

#include "core/data/smArray.h"

const uint32_t sm__handle_index_mask = (1 << (32 - SM_CONFIG_HANDLE_GEN_BITS)) - 1;
const uint32_t sm__handle_gen_mask = ((1 << SM_CONFIG_HANDLE_GEN_BITS) - 1);
const uint32_t sm__handle_gen_shift = (32 - SM_CONFIG_HANDLE_GEN_BITS);
