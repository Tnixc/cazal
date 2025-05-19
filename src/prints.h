#ifndef UTIL_H
#define UTIL_H

int print_node_array(struct NodeArray *tokens);

int print_stack(struct Stack *stack);

void print_stack_item(struct StackItem *item);

// New functions for WASM interface
#include <stddef.h>
void print_stack_item_to_buffer(struct StackItem *item);
int print_stack_to_buffer(struct Stack *stack, char *buffer, size_t buffer_size);
int print_node_array_to_buffer(struct NodeArray *tokens, char *buffer, size_t buffer_size);

#endif /* UTIL_H */