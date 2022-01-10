#ifndef SM_MEM_H
#define SM_MEM_H

#include <stddef.h>

void *__smmem_malloc(size_t size);
void *__smmem_calloc(size_t nmemb, size_t size);
void *__smmem_realloc(void *ptr, size_t size);
void __smmem_free(void *ptr);
void __smmem_print(void);

#define SM_MALLOC(SIZE)        __smmem_malloc(SIZE)
#define SM_CALLOC(NMEMB, SIZE) __smmem_calloc(NMEMB, SIZE)
#define SM_REALLOC(PTR, SIZE)  __smmem_realloc(PTR, SIZE)
#define SM_FREE(PTR)           __smmem_free(PTR)

#endif // SM_MEM_H
