#include "smpch.h"

#include "core/smAssert.h"
#include "core/smMem.h"

#define STBI_MALLOC(sz)        SM_MALLOC(sz)
#define STBI_REALLOC(p, newsz) SM_REALLOC(p, newsz)
#define STBI_FREE(p)           SM_FREE(p)
#define STBI_ASSERT(x)         SM_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stbi/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
