#include <stdbool.h>

typedef struct hash_table_s {
  char *key;
  void *value;

  struct hash_table *next;

} hash_table_s;

hash_table_s *hash_table_s_new(void);
bool hash_table_s_ctor(hash_table_s *table);
void hash_table_s_dtor(hash_table_s *table);

void *hash_table_s_get(hash_table_s *table, char *key);
void hash_table_s_set(hash_table_s *table, char *key, void *value);
void hash_table_s_remove(hash_table_s *table, char *key);
