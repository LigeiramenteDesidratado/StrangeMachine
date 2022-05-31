#ifndef SM_CORE_DATA_SMHASHTABLE_C
#define SM_CORE_DATA_SMHASHTABLE_C

#include "smpch.h"

#include "core/util/smString.h"

#define SM_HASHMAP_DECLARE(NAME, KEY_TYPE)                                                                             \
  typedef struct sm__hashmap_##NAME##_s sm_hashmap_##NAME##_s;                                                         \
  sm_hashmap_##NAME##_s *sm_hashmap_new_##NAME();                                                                      \
                                                                                                                       \
  /**                                                                                                                  \
   * Creates a new hash table                                                                                          \
   *                                                                                                                   \
   * @param cap Number of expected entries                                                                             \
   * @param hash Function which hashes keys or NULL to use default                                                     \
   * @param equals Function which compares keys for equality or NULL to use default                                    \
   * @return True if successful, false otherwise                                                                       \
   */                                                                                                                  \
  bool sm_hashmap_ctor_##NAME(sm_hashmap_##NAME##_s *map, size_t cap, int (*hash)(KEY_TYPE key),                       \
                              bool (*equals)(KEY_TYPE key_a, KEY_TYPE key_b));                                         \
                                                                                                                       \
  /**                                                                                                                  \
   * Frees the hash table. Does not free the keys or values themselves.                                                \
   */                                                                                                                  \
  void sm_hashmap_dtor_##NAME(sm_hashmap_##NAME##_s *map);                                                             \
                                                                                                                       \
  /**                                                                                                                  \
   * Puts value for the given key in the map. Returns pre-existing value if                                            \
   * any.                                                                                                              \
   *                                                                                                                   \
   * If memory allocation fails, this function returns NULL, the map's size                                            \
   * does not increase, and errno is set to ENOMEM.                                                                    \
   */                                                                                                                  \
  void *sm_hashmap_put_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key, void *value);                                  \
                                                                                                                       \
  /**                                                                                                                  \
   * Gets a value from the map. Returns NULL if no entry for the given key is                                          \
   * found or if the value itself is NULL.                                                                             \
   */                                                                                                                  \
  void *sm_hashmap_get_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key);                                               \
                                                                                                                       \
  /**                                                                                                                  \
   * Removes an entry from the map. Returns the removed value or NULL if no                                            \
   * entry was present.                                                                                                \
   */                                                                                                                  \
  void *sm_hashmap_remove_##NAME(sm_hashmap_##NAME##_s *map, KEY_TYPE key);                                            \
                                                                                                                       \
  /**                                                                                                                  \
   * Invokes the given callback on each entry in the map. Stops iterating if                                           \
   * the callback returns false.                                                                                       \
   */                                                                                                                  \
  void sm_hashmap_for_each_##NAME(sm_hashmap_##NAME##_s *map, bool (*cb)(KEY_TYPE key, void *value, void *user_data),  \
                                  void *user_data);                                                                    \
  /**                                                                                                                  \
   * Concurrency support.                                                                                              \
   */                                                                                                                  \
  /**                                                                                                                  \
   * Locks the hash table so only the current thread can access it.                                                    \
   */                                                                                                                  \
  void sm_hashmap_lock_##NAME(sm_hashmap_##NAME##_s *map);                                                             \
                                                                                                                       \
  /**                                                                                                                  \
   * Unlocks the hash table so other threads can access it.                                                            \
   */                                                                                                                  \
  void sm_hashmap_unlock_##NAME(sm_hashmap_##NAME##_s *map);

SM_HASHMAP_DECLARE(u32, uint32_t)
SM_HASHMAP_DECLARE(u64, uint64_t)
SM_HASHMAP_DECLARE(str, sm_string *)

#endif /* SM_CORE_DATA_SMHASHTABLE_C */
