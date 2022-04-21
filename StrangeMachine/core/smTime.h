#ifndef SM_CORE_TIME_H
#define SM_CORE_TIME_H

#include "smpch.h"

uint32_t sm__get_ticks(void);
void sm__delay(uint32_t ms);

#define SM_GET_TICKS() sm__get_ticks()
#define SM_DELAY(MS)   sm__delay(MS)

#endif /* SM_CORE_TIME_H */
