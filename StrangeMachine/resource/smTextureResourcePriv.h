#ifndef SM_TEXTURE_RESOURCE_PRIVATE_H
#define SM_TEXTURE_RESOURCE_PRIVATE_H

#include "renderer/smDevicePub.h"
#include "smpch.h"

bool sm_texture_resource_init(size_t capacity);
void sm_texture_resource_teardown(void);

#endif /* SM_TEXTURE_RESOURCE_PRIVATE_H */
