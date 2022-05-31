#include "smpch.h"

#include "core/smCore.h"

#include "core/data/smHashMap.h"
#include "core/thread/synchronization/smMutex.h"

static int str_hash_fn(sm_string *str) {
  uint32_t hash = 5381;
  const char *p;
  for (p = sm_string_c_str(str); p && *p; p++) {
    hash = ((hash << 5) + hash) + (uint32_t)(*p);
  }
  return (int)hash;
}
/* copied from libcutils/str_parms.c */
static bool str_eq(sm_string *key_a, sm_string *key_b) {
  return sm_string_eq(key_a, key_b);
}

static inline int fib_hash_fn(uint32_t h) {
  uint32_t bits = 60;
  uint64_t h64 = (uint64_t)h;
  h64 ^= (h64 >> bits);
  return (int)((h64 * 11400714819323198485llu) >> bits);
}

static inline bool num_eq(uint32_t key_a, uint32_t key_b) {
  return key_a == key_b;
}

static inline int fib_hash64_fn(uint64_t h) {
  uint32_t bits = 60;
  uint64_t h64 = h;
  h64 ^= (h64 >> bits);
  return (int)((h64 * 11400714819323198485llu) >> bits);
}

static inline bool num64_eq(uint64_t key_a, uint64_t key_b) {
  return key_a == key_b;
}

static inline size_t sm__calculate_index(size_t bucket_count, int hash) {
  return ((size_t)hash) & (bucket_count - 1);
}

#define SM_HASHMAP_DEFINE(NAME, KEY_TYPE, DEFAULT_HASH, DEFAULT_EQUALS)                                                \
                                                                                                                       \
  typedef struct sm__entry_##NAME sm_entry_##NAME;                                                                     \
  struct sm__entry_##NAME {                                                                                            \
    KEY_TYPE key;                                                                                                      \
    int hash;                                                                                                          \
    void *value;                                                                                                       \
    sm_entry_##NAME *next;                                                                                             \
  };                                                                                                                   \
                                                                                                                       \
  typedef struct sm__hashmap_##NAME##_s {                                                                              \
    sm_entry_##NAME **buckets;                                                                                         \
    size_t bucket_count;                                                                                               \
    int (*hash)(KEY_TYPE key);                                                                                         \
    bool (*equals)(KEY_TYPE keyA, KEY_TYPE keyB);                                                                      \
    sm_mutex *lock;                                                                                                    \
    size_t size;                                                                                                       \
                                                                                                                       \
  } sm_hashmap_##NAME##_s;                                                                                             \
                                                                                                                       \
  static void sm__expand_if_necessary_##NAME(sm_hashmap_##NAME##_s *map);                                              \
                                                                                                                       \
  sm_hashmap_##NAME##_s *sm_hashmap_new_##NAME() {                                                                     \
                                                                                                                       \
    sm_hashmap_##NAME##_s *map = SM_MALLOC(sizeof(sm_hashmap_##NAME##_s));                                             \
    SM_CORE_ASSERT(map);                                                                                               \
                                                                                                                       \
    return map;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  bool sm_hashmap_ctor_##NAME(sm_hashmap_##NAME##_s *map, size_t cap, int (*hash)(KEY_TYPE key),                       \
                              bool (*equals)(KEY_TYPE key_a, KEY_TYPE key_b)) {                                        \
    SM_CORE_ASSERT(map);                                                                                               \
    if (hash == NULL) {                                                                                                \
      hash = DEFAULT_HASH;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    if (equals == NULL) {                                                                                              \
      equals = DEFAULT_EQUALS;                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    size_t min_bucket_count = cap * 4 / 3;                                                                             \
    map->bucket_count = 1;                                                                                             \
    while (map->bucket_count <= min_bucket_count) {                                                                    \
      /* Bucket count must be power of 2. */                                                                           \
      map->bucket_count <<= 1;                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    map->buckets = SM_CALLOC(map->bucket_count, sizeof(sm_entry_##NAME *));                                            \
    if (map->buckets == NULL) {                                                                                        \
      SM_FREE(map);                                                                                                    \
      return false;                                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    map->size = 0;                                                                                                     \
    map->hash = hash;                                                                                                  \
    map->lock = sm__mutex_ctor();                                                                                      \
    map->equals = equals;                                                                                              \
                                                                                                                       \
    return true;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_dtor_##NAME(sm_hashmap_##NAME##_s *map) {                                                            \
    size_t i;                                                                                                          \
    for (i = 0; i < map->bucket_count; i++) {                                                                          \
      sm_entry_##NAME *entry = map->buckets[i];                                                                        \
      while (entry != NULL) {                                                                                          \
        sm_entry_##NAME *next = entry->next;                                                                           \
        SM_FREE(entry);                                                                                                \
        entry = next;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
    SM_FREE(map->buckets);                                                                                             \
    sm__mutex_dtor(map->lock);                                                                                         \
    SM_FREE(map);                                                                                                      \
  }                                                                                                                    \
                                                                                                                       \
  static inline int sm_hash_key##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key) {                                      \
    int h = map->hash(key);                                                                                            \
    /* We apply this secondary hashing discovered by Doug Lea to defend against bad hashes. */                         \
    h += ~(h << 9);                                                                                                    \
    h ^= (((uint32_t)h) >> 14);                                                                                        \
    h += (h << 4);                                                                                                     \
    h ^= (((uint32_t)h) >> 10);                                                                                        \
    return h;                                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  static inline bool sm__equal_keys__##NAME(KEY_TYPE key_a, int hash_a, KEY_TYPE key_b, int hash_b,                    \
                                            bool (*equals)(KEY_TYPE, KEY_TYPE)) {                                      \
    if (key_a == key_b) {                                                                                              \
      return true;                                                                                                     \
    }                                                                                                                  \
    if (hash_a != hash_b) {                                                                                            \
      return false;                                                                                                    \
    }                                                                                                                  \
    return equals(key_a, key_b);                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  static sm_entry_##NAME *sm__create_entry_##NAME(KEY_TYPE key, int hash, void *value) {                               \
    sm_entry_##NAME *entry = SM_MALLOC(sizeof(sm_entry_##NAME));                                                       \
    if (entry == NULL) {                                                                                               \
      return NULL;                                                                                                     \
    }                                                                                                                  \
    entry->key = key;                                                                                                  \
    entry->hash = hash;                                                                                                \
    entry->value = value;                                                                                              \
    entry->next = NULL;                                                                                                \
    return entry;                                                                                                      \
  }                                                                                                                    \
                                                                                                                       \
  void *sm_hashmap_put_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key, void *value) {                                 \
    int hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    sm_entry_##NAME **p = &(map->buckets[index]);                                                                      \
    while (true) {                                                                                                     \
      sm_entry_##NAME *current = *p;                                                                                   \
      /* Add a new entry.  */                                                                                          \
      if (current == NULL) {                                                                                           \
        *p = sm__create_entry_##NAME(key, hash, value);                                                                \
        if (*p == NULL) {                                                                                              \
          errno = ENOMEM;                                                                                              \
          return NULL;                                                                                                 \
        }                                                                                                              \
        map->size++;                                                                                                   \
        sm__expand_if_necessary_##NAME(map);                                                                           \
        return NULL;                                                                                                   \
      }                                                                                                                \
      /* Replace existing entry. */                                                                                    \
      if (sm__equal_keys__##NAME(current->key, current->hash, key, hash, map->equals)) {                               \
        void *oldValue = current->value;                                                                               \
        current->value = value;                                                                                        \
        return oldValue;                                                                                               \
      }                                                                                                                \
      /* Move to next entry. */                                                                                        \
      p = &current->next;                                                                                              \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  static void sm__expand_if_necessary_##NAME(sm_hashmap_##NAME##_s *map) {                                             \
    /* If the load factor exceeds 0.75... */                                                                           \
    if (map->size > (map->bucket_count * 3 / 4)) {                                                                     \
      /* Start off with a 0.33 load factor. */                                                                         \
      size_t new_bucket_count = map->bucket_count << 1;                                                                \
      sm_entry_##NAME **new_buckets = SM_CALLOC(new_bucket_count, sizeof(sm_entry_##NAME *));                          \
      if (new_buckets == NULL) {                                                                                       \
        /* Abort expansion.  */                                                                                        \
        return;                                                                                                        \
      }                                                                                                                \
      /* Move over existing entries.  */                                                                               \
      size_t i;                                                                                                        \
      for (i = 0; i < map->bucket_count; i++) {                                                                        \
        sm_entry_##NAME *entry = map->buckets[i];                                                                      \
        while (entry != NULL) {                                                                                        \
          sm_entry_##NAME *next = entry->next;                                                                         \
          size_t index = sm__calculate_index(new_bucket_count, entry->hash);                                           \
          entry->next = new_buckets[index];                                                                            \
          new_buckets[index] = entry;                                                                                  \
          entry = next;                                                                                                \
        }                                                                                                              \
      }                                                                                                                \
      /* Copy over internals.  */                                                                                      \
      free(map->buckets);                                                                                              \
      map->buckets = new_buckets;                                                                                      \
      map->bucket_count = new_bucket_count;                                                                            \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  void *sm_hashmap_get_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key) {                                              \
    int hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    sm_entry_##NAME *entry = map->buckets[index];                                                                      \
    while (entry != NULL) {                                                                                            \
      if (sm__equal_keys__##NAME(entry->key, entry->hash, key, hash, map->equals)) {                                   \
        return entry->value;                                                                                           \
      }                                                                                                                \
      entry = entry->next;                                                                                             \
    }                                                                                                                  \
    return NULL;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void *sm_hashmap_remove_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key) {                                           \
    int hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    /* Pointer to the current entry. */                                                                                \
    sm_entry_##NAME **p = &(map->buckets[index]);                                                                      \
    sm_entry_##NAME *current;                                                                                          \
    while ((current = *p) != NULL) {                                                                                   \
      if (sm__equal_keys__##NAME(current->key, current->hash, key, hash, map->equals)) {                               \
        void *value = current->value;                                                                                  \
        *p = current->next;                                                                                            \
        SM_FREE(current);                                                                                              \
        map->size--;                                                                                                   \
        return value;                                                                                                  \
      }                                                                                                                \
      p = &current->next;                                                                                              \
    }                                                                                                                  \
    return NULL;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_for_each_##NAME(sm_hashmap_##NAME##_s *map, bool (*cb)(KEY_TYPE key, void *value, void *user_data),  \
                                  void *user_data) {                                                                   \
    size_t i;                                                                                                          \
    for (i = 0; i < map->bucket_count; i++) {                                                                          \
      sm_entry_##NAME *entry = map->buckets[i];                                                                        \
      while (entry != NULL) {                                                                                          \
        sm_entry_##NAME *next = entry->next;                                                                           \
        if (!cb(entry->key, entry->value, user_data)) {                                                                \
          return;                                                                                                      \
        }                                                                                                              \
        entry = next;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_lock_##NAME(sm_hashmap_##NAME##_s *map) {                                                            \
    SM_MUTEX_LOCK(map->lock);                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_unlock_##NAME(sm_hashmap_##NAME##_s *map) {                                                          \
    SM_MUTEX_UNLOCK(map->lock);                                                                                        \
  }

SM_HASHMAP_DEFINE(u32, uint32_t, fib_hash_fn, num_eq)
SM_HASHMAP_DEFINE(u64, uint64_t, fib_hash64_fn, num64_eq)
SM_HASHMAP_DEFINE(str, sm_string *, str_hash_fn, str_eq)
