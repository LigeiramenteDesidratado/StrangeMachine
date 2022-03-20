#include "smpch.h"

#include "data/array.h"

#include "core/smLayer.h"

typedef struct {

  layer_s **layers;
  layer_s **overlayers;

} stack_layer_s;

stack_layer_s *stack_layer_new(void) {

  stack_layer_s *stack_layer = SM_CALLOC(1, sizeof(stack_layer_s));
  SM_ASSERT(stack_layer != NULL);

  return stack_layer;
}

bool stack_layer_ctor(stack_layer_s *stack_layer) {

  SM_ASSERT(stack_layer != NULL);

  stack_layer->layers = (layer_s **)SM_ARRAY_NEW_EMPTY();
  stack_layer->overlayers = (layer_s **)SM_ARRAY_NEW_EMPTY();
  SM_ASSERT(stack_layer->layers != NULL);

  return true;
}

void stack_layer_dtor(stack_layer_s *stack_layer) {

  SM_ASSERT(stack_layer != NULL);

  size_t layer_size = SM_ARRAY_SIZE(stack_layer->layers);
  for (size_t i = 0; i < layer_size; i++) {
    layer_s *layer = stack_layer->layers[i];
    layer->on_detach(layer->user_data);
  }

  size_t overlayer_size = SM_ARRAY_SIZE(stack_layer->overlayers);
  for (size_t i = 0; i < overlayer_size; i++) {
    layer_s *layer = stack_layer->overlayers[i];
    layer->on_detach(layer->user_data);
  }

  SM_ARRAY_DTOR(stack_layer->layers);
  SM_ARRAY_DTOR(stack_layer->overlayers);

  SM_FREE(stack_layer);
}

void stack_layer_push(stack_layer_s *stack_layer, layer_s *layer) {

  SM_ASSERT(stack_layer != NULL);
  SM_ASSERT(layer != NULL);

  SM_ARRAY_PUSH(stack_layer->layers, layer);
}

void stack_layer_push_overlay(stack_layer_s *stack_layer, layer_s *layer) {

  SM_ASSERT(stack_layer != NULL);
  SM_ASSERT(layer != NULL);

  SM_ARRAY_PUSH(stack_layer->overlayers, layer);
}

void stack_layer_pop(stack_layer_s *stack_layer) {

  SM_ASSERT(stack_layer != NULL);

  SM_ARRAY_POP(stack_layer->layers);
}

void stack_layer_pop_overlay(stack_layer_s *stack_layer) {

  SM_ASSERT(stack_layer != NULL);

  SM_ARRAY_POP(stack_layer->overlayers);
}

size_t stack_layer_get_size(stack_layer_s *stack_layer) {

  SM_ASSERT(stack_layer != NULL);

  return SM_ARRAY_SIZE(stack_layer->layers) + SM_ARRAY_SIZE(stack_layer->overlayers);
}

layer_s *stack_layer_get_layer(stack_layer_s *stack_layer, size_t index) {

  SM_ASSERT(stack_layer != NULL);

  if (index < SM_ARRAY_SIZE(stack_layer->layers)) {
    return stack_layer->layers[index];
  } else {
    return stack_layer->overlayers[index - SM_ARRAY_SIZE(stack_layer->layers)];
  }
}
