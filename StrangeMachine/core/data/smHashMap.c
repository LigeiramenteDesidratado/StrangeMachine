#include "smpch.h"

#include "core/smCore.h"

#include "core/data/smHashMap.h"
#include "core/thread/synchronization/smMutex.h"

static i32 str_hash_fn(sm_string string) {

  u32 hash = 5381;

  for (const char *p = string.str; p && *p; p++) {
    hash = ((hash << 5) + hash) + (i32)(*p);
  }
  return (i32)hash;
}
/* copied from libcutils/str_parms.c */
static b8 str_eq(sm_string key_a, sm_string key_b) {
  return sm_string_eq(key_a, key_b);
}

static inline i32 fib_hash_fn(u32 h) {
  u32 bits = 60;
  u64 h64 = (u64)h;
  h64 ^= (h64 >> bits);
  return (i32)((h64 * 11400714819323198485llu) >> bits);
}

static inline b8 num_eq(u32 key_a, u32 key_b) {
  return key_a == key_b;
}

static inline i32 fib_hash64_fn(u64 h) {
  u32 bits = 60;
  u64 h64 = h;
  h64 ^= (h64 >> bits);
  return (i32)((h64 * 11400714819323198485llu) >> bits);
}

static inline b8 num64_eq(u64 key_a, u64 key_b) {
  return key_a == key_b;
}

static inline size_t sm__calculate_index(size_t bucket_count, i32 hash) {
  return ((size_t)hash) & (bucket_count - 1);
}

#define SM_HASHMAP_DEFINE(NAME, KEY_TYPE, DEFAULT_HASH, DEFAULT_EQUALS)                                                \
                                                                                                                       \
  typedef struct sm__entry_##NAME##_s sm_entry_##NAME##_s;                                                             \
  struct sm__entry_##NAME##_s {                                                                                        \
    KEY_TYPE key;                                                                                                      \
    i32 hash;                                                                                                          \
    void *value;                                                                                                       \
    sm_entry_##NAME##_s *next;                                                                                         \
  };                                                                                                                   \
                                                                                                                       \
  typedef struct sm__hashmap_##NAME##_m {                                                                              \
    sm_entry_##NAME##_s **buckets;                                                                                     \
    size_t bucket_count;                                                                                               \
    i32 (*hash)(KEY_TYPE key);                                                                                         \
    b8 (*equals)(KEY_TYPE keyA, KEY_TYPE keyB);                                                                        \
    sm_mutex *lock;                                                                                                    \
    size_t size;                                                                                                       \
                                                                                                                       \
  } sm_hashmap_##NAME##_m;                                                                                             \
                                                                                                                       \
  static void sm__expand_if_necessary_##NAME(sm_hashmap_##NAME##_m *map);                                              \
                                                                                                                       \
  sm_hashmap_##NAME##_m *sm_hashmap_new_##NAME() {                                                                     \
                                                                                                                       \
    sm_hashmap_##NAME##_m *map = SM_MALLOC(sizeof(sm_hashmap_##NAME##_m));                                             \
    SM_CORE_ASSERT(map);                                                                                               \
                                                                                                                       \
    return map;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  b8 sm_hashmap_ctor_##NAME(sm_hashmap_##NAME##_m *map, size_t cap, i32 (*hash)(KEY_TYPE key),                         \
                            b8 (*equals)(KEY_TYPE key_a, KEY_TYPE key_b)) {                                            \
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
    map->buckets = SM_CALLOC(map->bucket_count, sizeof(sm_entry_##NAME##_s *));                                        \
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
  void sm_hashmap_dtor_##NAME(sm_hashmap_##NAME##_m *map) {                                                            \
    size_t i;                                                                                                          \
    for (i = 0; i < map->bucket_count; i++) {                                                                          \
      sm_entry_##NAME##_s *entry = map->buckets[i];                                                                    \
      while (entry != NULL) {                                                                                          \
        sm_entry_##NAME##_s *next = entry->next;                                                                       \
        SM_FREE(entry);                                                                                                \
        entry = next;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
    SM_FREE(map->buckets);                                                                                             \
    sm__mutex_dtor(map->lock);                                                                                         \
    SM_FREE(map);                                                                                                      \
  }                                                                                                                    \
                                                                                                                       \
  static inline i32 sm_hash_key##NAME(sm_hashmap_##NAME##_m *map, KEY_TYPE key) {                                      \
    i32 h = map->hash(key);                                                                                            \
    /* We apply this secondary hashing discovered by Doug Lea to defend against bad hashes. */                         \
    h += ~(h << 9);                                                                                                    \
    h ^= (((u32)h) >> 14);                                                                                             \
    h += (h << 4);                                                                                                     \
    h ^= (((u32)h) >> 10);                                                                                             \
    return h;                                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  static inline b8 sm__equal_keys__##NAME(KEY_TYPE key_a, i32 hash_a, KEY_TYPE key_b, i32 hash_b,                      \
                                          b8 (*equals)(KEY_TYPE, KEY_TYPE)) {                                          \
    if (hash_a != hash_b) {                                                                                            \
      return false;                                                                                                    \
    }                                                                                                                  \
    return equals(key_a, key_b);                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  static sm_entry_##NAME##_s *sm__create_entry_##NAME(KEY_TYPE key, i32 hash, void *value) {                           \
    sm_entry_##NAME##_s *entry = SM_MALLOC(sizeof(sm_entry_##NAME##_s));                                               \
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
  void *sm_hashmap_put_##NAME(sm_hashmap_##NAME##_m *map, KEY_TYPE key, void *value) {                                 \
    i32 hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    sm_entry_##NAME##_s **p = &(map->buckets[index]);                                                                  \
    while (true) {                                                                                                     \
      sm_entry_##NAME##_s *current = *p;                                                                               \
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
  static void sm__expand_if_necessary_##NAME(sm_hashmap_##NAME##_m *map) {                                             \
    /* If the load factor exceeds 0.75... */                                                                           \
    if (map->size > (map->bucket_count * 3 / 4)) {                                                                     \
      /* Start off with a 0.33 load factor. */                                                                         \
      size_t new_bucket_count = map->bucket_count << 1;                                                                \
      sm_entry_##NAME##_s **new_buckets = SM_CALLOC(new_bucket_count, sizeof(sm_entry_##NAME##_s *));                  \
      if (new_buckets == NULL) {                                                                                       \
        /* Abort expansion.  */                                                                                        \
        return;                                                                                                        \
      }                                                                                                                \
      /* Move over existing entries.  */                                                                               \
      size_t i;                                                                                                        \
      for (i = 0; i < map->bucket_count; i++) {                                                                        \
        sm_entry_##NAME##_s *entry = map->buckets[i];                                                                  \
        while (entry != NULL) {                                                                                        \
          sm_entry_##NAME##_s *next = entry->next;                                                                     \
          size_t index = sm__calculate_index(new_bucket_count, entry->hash);                                           \
          entry->next = new_buckets[index];                                                                            \
          new_buckets[index] = entry;                                                                                  \
          entry = next;                                                                                                \
        }                                                                                                              \
      }                                                                                                                \
      /* Copy over i32ernals.  */                                                                                      \
      SM_FREE(map->buckets);                                                                                           \
      map->buckets = new_buckets;                                                                                      \
      map->bucket_count = new_bucket_count;                                                                            \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  void *sm_hashmap_get_##NAME(sm_hashmap_##NAME##_m *map, KEY_TYPE key) {                                              \
    i32 hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    sm_entry_##NAME##_s *entry = map->buckets[index];                                                                  \
    while (entry != NULL) {                                                                                            \
      if (sm__equal_keys__##NAME(entry->key, entry->hash, key, hash, map->equals)) {                                   \
        return entry->value;                                                                                           \
      }                                                                                                                \
      entry = entry->next;                                                                                             \
    }                                                                                                                  \
    return NULL;                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  void *sm_hashmap_remove_##NAME(sm_hashmap_##NAME##_m *map, KEY_TYPE key) {                                           \
    i32 hash = sm_hash_key##NAME(map, key);                                                                            \
    size_t index = sm__calculate_index(map->bucket_count, hash);                                                       \
    /* Pointer to the current entry. */                                                                                \
    sm_entry_##NAME##_s **p = &(map->buckets[index]);                                                                  \
    sm_entry_##NAME##_s *current;                                                                                      \
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
  void sm_hashmap_for_each_##NAME(sm_hashmap_##NAME##_m *map, b8 (*cb)(KEY_TYPE key, void *value, void *user_data),    \
                                  void *user_data) {                                                                   \
    size_t i;                                                                                                          \
    for (i = 0; i < map->bucket_count; i++) {                                                                          \
      sm_entry_##NAME##_s *entry = map->buckets[i];                                                                    \
      while (entry != NULL) {                                                                                          \
        sm_entry_##NAME##_s *next = entry->next;                                                                       \
        if (!cb(entry->key, entry->value, user_data)) {                                                                \
          return;                                                                                                      \
        }                                                                                                              \
        entry = next;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_lock_##NAME(sm_hashmap_##NAME##_m *map) {                                                            \
    SM_MUTEX_LOCK(map->lock);                                                                                          \
  }                                                                                                                    \
                                                                                                                       \
  void sm_hashmap_unlock_##NAME(sm_hashmap_##NAME##_m *map) {                                                          \
    SM_MUTEX_UNLOCK(map->lock);                                                                                        \
  }

SM_HASHMAP_DEFINE(u32, u32, fib_hash_fn, num_eq)
SM_HASHMAP_DEFINE(u64, u64, fib_hash64_fn, num64_eq)
SM_HASHMAP_DEFINE(str, sm_string, str_hash_fn, str_eq)
