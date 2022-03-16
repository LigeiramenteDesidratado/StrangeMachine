#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "smMem.h"
#define STBDS_REALLOC(c, PTR, SIZE) SM_REALLOC(PTR, SIZE)
#define STBDS_FREE(c, PTR)          SM_FREE(PTR)
#define STB_DS_IMPLEMENTATION
#include "stb_ds/stb_ds.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
