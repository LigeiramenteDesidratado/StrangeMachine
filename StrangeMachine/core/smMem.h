#ifndef SM_MEM_H
#define SM_MEM_H

#include "smpch.h"

#ifdef SM_DEBUG

void *sm__mem_malloc(size_t size);
void *sm__mem_calloc(size_t nmemb, size_t size);
void *sm__mem_realloc(void *ptr, size_t size);
void sm__mem_free(void *ptr);
void sm__mem_print(void);

void *sm__mem_aligned_alloc(size_t alignment, size_t size);
void sm__mem_aligned_free(void *ptr);

  /* Memory */
  #define SM_MALLOC(SIZE)        sm__mem_malloc(SIZE)
  #define SM_CALLOC(NMEMB, SIZE) sm__mem_calloc(NMEMB, SIZE)
  #define SM_REALLOC(PTR, SIZE)  sm__mem_realloc(PTR, SIZE)
  #define SM_FREE(PTR)           sm__mem_free(PTR)

  /* Aligned memory*/
  #define SM_ALIGNED_ALLOC(ALIGNMENT, SIZE) sm__mem_aligned_alloc(ALIGNMENT, SIZE)
  #define SM_ALIGNED_FREE(PTR)              sm__mem_aligned_free(PTR)

#else

  #define SM_MALLOC(SIZE)                   malloc(SIZE)
  #define SM_CALLOC(NMEMB, SIZE)            calloc(NMEMB, SIZE)
  #define SM_REALLOC(PTR, SIZE)             realloc(PTR, SIZE)
  #define SM_FREE(PTR)                      free(PTR)

  /* TODO: system aligned memory for others plataforms */
  #define SM_ALIGNED_ALLOC(ALIGNMENT, SIZE) aligned_alloc(ALIGNMENT, SIZE)
  #define SM_ALIGNED_FREE(PTR)              free(PTR)

void sm__mem_print(void);

#endif /* SM_DEBUG */

#endif /* SM_MEM_H */
