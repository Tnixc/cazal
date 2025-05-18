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
      printf("DefinedFn\n");
      printf("(-------------------------------\n");
      print_node_array(tokens->tokens[i].value.defined_fn);
      printf(")--------------------------------\n");
    } else {
      printf("\n");
    }
  }
  return 0;
}
