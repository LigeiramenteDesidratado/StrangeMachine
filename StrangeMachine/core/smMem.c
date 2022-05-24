#include "smpch.h"

#include "smAssert.h"
#include "smMem.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "MEMORY"

static struct {
  uint64_t total_allocs;
  uint64_t allocs;
  uint64_t frees;
  size_t total_bytes;
  size_t bytes;

} mem_info = {0};

#define SM_MEM_HEADER_SIZE sizeof(size_t)

static char *sm__mem_human_readable_size(void);
void *sm__mem_malloc(size_t size) {

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

  /* pointer arithmetic to return the array
   * -------------------
   * |size|0|1|2|3|...|
   * -----^------------
   *      |
   *      address returned
   * */
  return ptr + SM_MEM_HEADER_SIZE;
}

void *sm__mem_calloc(size_t nmemb, size_t size) {

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

void *sm__mem_realloc(void *ptr, size_t size) {

  if (!ptr)
    return sm__mem_malloc(size);

  char *newp = NULL;
  char *_ptr = (((char *)ptr) - SM_MEM_HEADER_SIZE);

  if ((newp = realloc(_ptr, size + SM_MEM_HEADER_SIZE)) == NULL)
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

void sm__mem_free(void *ptr) {

  if (ptr) {

    void *_ptr = ((char *)ptr) - SM_MEM_HEADER_SIZE;
    size_t size = *((size_t *)_ptr);

    mem_info.bytes -= size;
    mem_info.allocs--;
    mem_info.frees++;

    free(_ptr);
  }
}

/* Alinged memory allocation */

void *sm__mem_aligned_alloc(size_t alignment, size_t size) {

  void *ptr = NULL;

  /* ensure align and size are non-zero values before we try to allocate any memory. We also need to check that our
   * alignment request is a power of two */

  SM_ASSERT((alignment & (alignment - 1)) == 0);

  if (alignment && size) {

    size_t _size = size + alignment - 1;

    void *raw = (void *)malloc(_size + SM_MEM_HEADER_SIZE);
    if (!raw)
      return NULL;

    /* store the size at the begining of the array */
    *(size_t *)raw = size;

    void *header_address = (void *)((((char *)raw) + SM_MEM_HEADER_SIZE)); /* not the actual address of the header */

    void *aligned_address = (void *)((((uintptr_t)header_address) + alignment) & ~(alignment - 1));
    /* void *aligned_address = (void *)(((size_t)header_address + alignment) & ~(alignment - 1)); */

    uint8_t offset = (char *)aligned_address - (char *)header_address;

    /* store the offset 1 byte before the aligned address */
    *((char *)aligned_address - 1) = offset;

    ptr = aligned_address;

    mem_info.total_bytes += size;
    mem_info.bytes += size;
    mem_info.total_allocs++;
    mem_info.allocs++;
  }

  return ptr;
}

void sm__mem_aligned_free(void *ptr) {

  if (ptr) {

    /* get the offset by reading the byte before the aligned address */
    uint8_t offset = *((char *)ptr - 1);

    /* get the original address by subtracting the offset plus the header size from the aligned address */
    void *_ptr = ((char *)ptr) - (offset + SM_MEM_HEADER_SIZE);

    size_t size = *((size_t *)_ptr);

    mem_info.bytes -= size;
    mem_info.allocs--;
    mem_info.frees++;

    free(_ptr);
  }
}

#define BYTES2KB ((float)(1 << 10))
#define BYTES2MB ((float)(1 << 20))
#define BYTES2GB ((float)(1 << 30))

void sm__mem_print(void) {
  printf("dangling: %lu\ndangling bytes: %lu %s\ntotal (re)alloctions: %lu\ntotal bytes: %s\nfree calls: %lu\n",
         mem_info.allocs, mem_info.bytes, (mem_info.bytes == 0) ? "" : "(Waring: memory leaks!)", mem_info.total_allocs,
         sm__mem_human_readable_size(), mem_info.frees);
}

// return a string with human readable memory size
static char *sm__mem_human_readable_size(void) {
  static char buf[32];
  float size = (float)mem_info.total_bytes;
  if (size < BYTES2KB)
    sprintf(buf, "%.2f B", size);
  else if (size < BYTES2MB)
    sprintf(buf, "%.2f KB", size / BYTES2KB);
  else if (size < BYTES2GB)
    sprintf(buf, "%.2f MB", size / BYTES2MB);
  else
    sprintf(buf, "%.2f GB", size / BYTES2GB);
  return buf;
}

#undef SM_MODULE_NAME
