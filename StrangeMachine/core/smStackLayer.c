#include "smpch.h"

#include "core/data/smArray.h"
#include "core/smAssert.h"
#include "core/smMem.h"

#include "core/smLayer.h"

#undef SM_MODULE_NAME
#define SM_MODULE_NAME "STACKLAYER"

typedef struct {

  SM_ARRAY(struct layer_s *) layers;
  SM_ARRAY(struct layer_s *) overlayers;

} stack_layer_s;

stack_layer_s *stack_layer_new(void) {

  stack_layer_s *stack_layer = SM_CALLOC(1, sizeof(stack_layer_s));
  SM_CORE_ASSERT(stack_layer);

  return stack_layer;
}

b8 stack_layer_ctor(stack_layer_s *stack_layer) {

  SM_CORE_ASSERT(stack_layer);

  /* stack_layer->layers = (SM_ARRAY(struct layer_s *))SM_ARRAY_NEW_EMPTY(); */
  /* stack_layer->overlayers = (SM_ARRAY(struct layer_s *))SM_ARRAY_NEW_EMPTY(); */
  SM_ARRAY_CTOR(stack_layer->layers, 8);
  SM_ARRAY_CTOR(stack_layer->overlayers, 8);
  SM_CORE_ASSERT(stack_layer->layers && stack_layer->overlayers);

  return true;
}

void stack_layer_dtor(stack_layer_s *stack_layer) {

  SM_CORE_ASSERT(stack_layer);

  size_t layer_size = SM_ARRAY_LEN(stack_layer->layers);
  for (size_t i = 0; i < layer_size; ++i) {
    struct layer_s *layer = stack_layer->layers[i];
    layer_detach(layer);
  }

  size_t overlayer_size = SM_ARRAY_LEN(stack_layer->overlayers);
  for (size_t i = 0; i < overlayer_size; ++i) {
    struct layer_s *layer = stack_layer->overlayers[i];
    layer_detach(layer);
  }

  SM_ARRAY_DTOR(stack_layer->layers);
  SM_ARRAY_DTOR(stack_layer->overlayers);

  SM_FREE(stack_layer);
}

void stack_layer_push(stack_layer_s *stack_layer, struct layer_s *layer) {

  SM_CORE_ASSERT(stack_layer);
  SM_CORE_ASSERT(layer != NULL);

  SM_ARRAY_PUSH(stack_layer->layers, layer);
}

void stack_layer_push_overlay(stack_layer_s *stack_layer, struct layer_s *layer) {

  SM_CORE_ASSERT(stack_layer);
  SM_CORE_ASSERT(layer != NULL);

  SM_ARRAY_PUSH(stack_layer->overlayers, layer);
}

void stack_layer_pop(stack_layer_s *stack_layer) {

  SM_CORE_ASSERT(stack_layer != NULL);

  SM_ARRAY_POP(stack_layer->layers);
}

void stack_layer_pop_overlay(stack_layer_s *stack_layer) {

  SM_CORE_ASSERT(stack_layer);

  SM_ARRAY_POP(stack_layer->overlayers);
}

size_t stack_layer_get_size(stack_layer_s *stack_layer) {

  SM_CORE_ASSERT(stack_layer);

  return SM_ARRAY_LEN(stack_layer->layers) + SM_ARRAY_LEN(stack_layer->overlayers);
}

struct layer_s *stack_layer_get_layer(stack_layer_s *stack_layer, size_t index) {

  SM_CORE_ASSERT(stack_layer);

  if (index < SM_ARRAY_LEN(stack_layer->layers)) {
    return stack_layer->layers[index];
  }
  return stack_layer->overlayers[index - SM_ARRAY_LEN(stack_layer->layers)];
}
#undef SM_MODULE_NAME
