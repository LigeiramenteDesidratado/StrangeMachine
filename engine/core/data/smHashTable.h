#ifndef SM_CORE_DATA_HASH_TABLE_H
#define SM_CORE_DATA_HASH_TABLE_H

#include "smpch.h"

#include "core/smMem.h"

#define STBDS_REALLOC(c, PTR, SIZE) SM_REALLOC(PTR, SIZE)
#define STBDS_FREE(c, PTR)          SM_FREE(PTR)
#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds/stb_ds.h"

/* This is temporary until I implement my own hash table */
#define SM_HT_INSERT  hmput
#define SM_HT_GET     hmget
#define SM_HT_DELETE  hmdel
#define SM_HT_LENGTH  hmlenu
#define SM_HT_DTOR    hmfree
#define SM_HT_DEFAULT hmdefault

#define SM_SH_INSERT  shput
#define SM_SH_GET     shget
#define SM_SH_DELETE  shdel
#define SM_SH_LENGTH  shlenu
#define SM_SH_DTOR    shfree
#define SM_SH_DEFAULT shdefault
#define SM_SH_STRDUP  sh_new_strdup

#endif /* SM_CORE_DATA_HASH_TABLE_H */
