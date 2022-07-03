#ifndef SM_CORE_TIME_H
#define SM_CORE_TIME_H

#include "smpch.h"

u32 sm__get_ticks(void);
void sm__delay(u32 ms);

#define SM_GET_TICKS() sm__get_ticks()
#define SM_DELAY(MS)   sm__delay(MS)

#endif /* SM_CORE_TIME_H */
