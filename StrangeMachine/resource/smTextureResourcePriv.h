#ifndef SM_TEXTURE_RESOURCE_PRIVATE_H
#define SM_TEXTURE_RESOURCE_PRIVATE_H

#include "renderer/smDevicePub.h"
#include "smpch.h"

bool texture_res_init(size_t capacity);
void texture_res_teardown(void);

#endif /* SM_TEXTURE_RESOURCE_PRIVATE_H */
