#include "exec.h"
#include "lex.h"
#include <stdio.h>

int print_node_array(struct NodeArray *tokens) {
  if (tokens == NULL) {
    return -1;
  }
  for (int i = 0; i < tokens->len; i++) {
    printf("%d: ", i);
    printf("%c -> ", tokens->tokens[i].type);
    if (tokens->tokens[i].type == Int) {
      printf("%d\n", tokens->tokens[i].value.i);
    } else if (tokens->tokens[i].type == Float) {
      printf("%f\n", tokens->tokens[i].value.f);
    } else if (tokens->tokens[i].type == Operator) {
      printf("%c\n", tokens->tokens[i].value.op);
    } else if (tokens->tokens[i].type == Modifier) {
      printf("%c\n", tokens->tokens[i].value.modifier);
    } else if (tokens->tokens[i].type == Fn) {
      printf("%c\n", tokens->tokens[i].value.fn);
    } else if (tokens->tokens[i].type == DefinedFn) {
      printf("DefinedFn");
      printf("(-------------------------------\n");
      print_node_array(tokens->tokens[i].value.defined_fn);
      printf(")--------------------------------\n");
    } else {
      printf("\n");
    }
  }
  return 0;
}

void print_stack_item(struct StackItem *item) {
  if (item->type == IntItem) {
    printf("%d", item->value.int_value);
  } else if (item->type == FloatItem) {
    printf("%f", item->value.float_value);
  } else if (item->type == DefinedFnItem) {
    printf("DefinedFn");
    printf("(");
    print_node_array(item->value.defined_fn);
    printf(")");
  } else if (item->type == ModifierItem) {
    printf("%c", item->value.modifier);
  } else if (item->type == FnItem) {
    printf("%c", item->value.function);
  }
  printf(" | ");
}

int print_stack(struct Stack *stack) {
  for (int i = 0; i <= stack->head; i++) {
    print_stack_item(&stack->items[i]);
  }
  printf("\n");
  return 0;
}
