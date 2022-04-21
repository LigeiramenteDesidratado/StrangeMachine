

#include "data/hash_table.h"
#include "smMem.h"
#include <assert.h>

hash_table_s* hash_table_new(void) {
  hash_table_s* ht = (hash_table_s*)SM_CALLOC(1 ,sizeof(hash_table_s));
  assert(ht != NULL);

  return ht;

}

