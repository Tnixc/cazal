#ifndef PRINTS_H
#define PRINTS_H

#include <stddef.h>
#include "lex.h"
#include "exec.h"

int print_node_array(struct NodeArray *tokens);

int print_stack(struct Stack *stack);

void print_stack_item(struct StackItem *item);

// Buffer functions declaration
int print_node_array_to_buffer(struct NodeArray *tokens, char *buffer, size_t buffer_size);
int print_stack_to_buffer(struct Stack *stack, char *buffer, size_t buffer_size);

#endif /* PRINTS_H */