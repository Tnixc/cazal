#include "exec.h"
#include "lex.h"
#include "prints.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char contents[] = "0 1 2 3 4 5 6 7 8 9 ( 1 + ) map ( * ) fold"; // 10!

  struct NodeArray *tokens = lex(contents);
  print_node_array(tokens);

  struct Stack stack = {-1, 256, NULL};
  stack.items = malloc(sizeof(struct StackItem) * stack.capacity);

  for (int i = 0; i < tokens->len; i++) {
    exec(&tokens->tokens[i], &stack);
    printf("head: %d : ", stack.head);
    print_stack(&stack);
  }
  return 0;
}
