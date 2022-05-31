#ifndef SM_CORE_REFERENCES_H
#define SM_CORE_REFERENCES_H

#include "smpch.h"

typedef struct sm__rc_s {

  _Atomic(uint16_t) count;

} sm_rc_s;

static inline void sm_rc_inc(const sm_rc_s *ref) {
  atomic_fetch_add((_Atomic(uint16_t) *)&ref->count, 1);
}

static inline void sm_rc_dec(const sm_rc_s *ref) {
  atomic_fetch_sub((_Atomic(uint16_t) *)&ref->count, 1);
}

#endif /* SM_CORE_REFERENCES_H */
