#include "exec.h"
#include "lex.h"
#include "prints.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char contents[] = "1 2 + 4 * 66 -";

  struct NodeArray *tokens = lex(contents);
  print_node_array(tokens);

  struct Stack stack = {-1, 256, NULL};
  stack.items = malloc(sizeof(struct StackItem) * stack.capacity);

  for (int i = 0; i < tokens->len; i++) {
    exec(&tokens->tokens[i], &stack);
    print_stack(&stack);
    printf("head index: %d\n", stack.head);
  }

  return 0;
}
