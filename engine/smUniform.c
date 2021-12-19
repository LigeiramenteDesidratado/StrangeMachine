#include "smUniform.h"
#include "util/common.h"

void __uniform_default(int32_t slot, void *data, int32_t length) {
  (void)slot;
  (void)data;
  (void)length;
  log_warn("invalid uniform array type");
}
