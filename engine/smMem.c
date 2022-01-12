#include "smMem.h"
#include "util/common.h"

static struct {
  uint64_t total_allocs;
  uint64_t allocs;
  uint64_t frees;
  size_t total_bytes;
  size_t bytes;

} mem_info = {0};

void *__smmem_malloc(size_t size) {

  /*
   * ISO/IEC 9899:TC3 section 6.5.3.4 The sizeof operator
   * item 3: When applied to an operand that has type char, unsigned char, or signed char,
   * (or a qualified version thereof) the result is 1...
   * */
  char *ptr = NULL;
  ptr = (char *)malloc(size + SM_MEM_HEADER_SIZE);
  if (ptr == NULL)
    return NULL;

  *(size_t *)ptr = size; /* save the size at the begining of the array */

  mem_info.total_bytes += size;
  mem_info.bytes += size;
  mem_info.total_allocs++;
  mem_info.allocs++;

  /* poiter arithmetic to return the array
   * -------------------
   * |size|0|1|2|3|...|
   * -----^------------
   *      |
   *      address returned
   * */
  return ptr + SM_MEM_HEADER_SIZE;
}

void *__smmem_calloc(size_t nmemb, size_t size) {

  size_t _size = nmemb * size;

  char *ptr = NULL;
  ptr = (char *)malloc(_size + SM_MEM_HEADER_SIZE);
  if (ptr == NULL)
    return NULL;
  memset(ptr + SM_MEM_HEADER_SIZE, 0, _size);

  *(size_t *)ptr = _size;

  mem_info.total_bytes += _size;
  mem_info.bytes += _size;
  mem_info.total_allocs++;
  mem_info.allocs++;

  return ptr + SM_MEM_HEADER_SIZE;
}

void *__smmem_realloc(void *ptr, size_t size) {

  if (!ptr)
    return __smmem_malloc(size);

  char *newp = NULL;
  char *_ptr = (((char *)ptr) - SM_MEM_HEADER_SIZE);

  if ((newp = realloc(_ptr, size)) == NULL)
    return NULL;

  _ptr = newp;
  size_t old_size = *(size_t *)_ptr;
  mem_info.bytes -= old_size;
  *(size_t *)_ptr = size;

  mem_info.total_bytes += size - old_size;
  mem_info.bytes += size;
  mem_info.total_allocs++;

  return _ptr + SM_MEM_HEADER_SIZE;
}

void __smmem_free(void *ptr) {

  if (ptr) {

    void *_ptr = ((char *)ptr) - SM_MEM_HEADER_SIZE;
    size_t size = *((size_t *)_ptr);

    mem_info.bytes -= size;
    mem_info.allocs--;
    mem_info.frees++;

    free(_ptr);
  }
}

void __smmem_print(void) {
  printf("allocations: %lu\ntotal alloctions:%lu\nbytes: %lu\ntotal bytes:%lu\nfree calls:%lu\n", mem_info.allocs,
         mem_info.total_allocs, mem_info.bytes, mem_info.total_bytes, mem_info.frees);
}
