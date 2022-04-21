#ifndef SM_STACK_LAYER_H
#define SM_STACK_LAYER_H

#include "smpch.h"

#include "core/smLayer.h"

struct stack_layer_s;

/* Allocate memory for a new stack layer */
struct stack_layer_s *stack_layer_new(void);

/* Constructor */
bool stack_layer_ctor(struct stack_layer_s *stack_layer);

/* Destructor */
void stack_layer_dtor(struct stack_layer_s *stack_layer);

void stack_layer_push(struct stack_layer_s *stack_layer, struct layer_s *layer);
void stack_layer_push_overlay(struct stack_layer_s *stack_layer, struct layer_s *layer);
void stack_layer_pop(struct stack_layer_s *stack_layer);
void stack_layer_pop_overlay(struct stack_layer_s *stack_layer);
size_t stack_layer_get_size(struct stack_layer_s *stack_layer);
struct layer_s *stack_layer_get_layer(struct stack_layer_s *stack_layer, size_t index);

#endif /* SM_STACK_LAYER_H */
