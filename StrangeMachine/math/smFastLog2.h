#ifndef SM_MATH_FAST_LOG2_H
#define SM_MATH_FAST_LOG2_H

#include "smpch.h"

extern const uint32_t sm__tab32[32];
extern const uint32_t sm__tab64[64];

uint32_t sm__log2_32(uint32_t value);
uint32_t sm__log2_64(uint64_t value);

#endif /* SM_MATH_FAST_LOG2_H */
