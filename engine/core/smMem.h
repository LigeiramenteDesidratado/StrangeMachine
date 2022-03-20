#ifndef SM_MEM_H
#define SM_MEM_H

#include "smpch.h"

#ifdef SM_DEBUG

void *__smmem_malloc(size_t size);
void *__smmem_calloc(size_t nmemb, size_t size);
void *__smmem_realloc(void *ptr, size_t size);
void __smmem_free(void *ptr);
void __smmem_print(void);

void *__smmem_aligned_alloc(size_t alignment, size_t size);
void __smmem_aligned_free(void *ptr);

  /* Memory */
  #define SM_MALLOC(SIZE)        __smmem_malloc(SIZE)
  #define SM_CALLOC(NMEMB, SIZE) __smmem_calloc(NMEMB, SIZE)
  #define SM_REALLOC(PTR, SIZE)  __smmem_realloc(PTR, SIZE)
  #define SM_FREE(PTR)           __smmem_free(PTR)

  /* Aligned memory*/
  #define SM_ALIGNED_ALLOC(ALIGNMENT, SIZE) __smmem_aligned_alloc(ALIGNMENT, SIZE)
  #define SM_ALIGNED_FREE(PTR)              __smmem_aligned_free(PTR)

#else

  #define SM_MALLOC(SIZE)                   malloc(SIZE)
  #define SM_CALLOC(NMEMB, SIZE)            calloc(NMEMB, SIZE)
  #define SM_REALLOC(PTR, SIZE)             realloc(PTR, SIZE)
  #define SM_FREE(PTR)                      free(PTR)

  /* TODO: system aligned memory for others plataforms */
  #define SM_ALIGNED_ALLOC(ALIGNMENT, SIZE) aligned_alloc(ALIGNMENT, SIZE)
  #define SM_ALIGNED_FREE(PTR)              free(PTR)

void __smmem_print(void);

#endif /* SM_DEBUG */

#endif /* SM_MEM_H */
