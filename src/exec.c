#include "lex.h"
#include "main.h"
#include <stdio.h>

int exec(struct Node *node, struct Stack *stack) {
  switch (node->type) {
  case Int:
    printf("Executing integer: %d\n", node->value.i);
    break;
  case Float:
    printf("Executing float: %f\n", node->value.f);
    break;
  case Operator:
    printf("Executing operator: %c\n", node->value.op);
    break;
  case Modifier:
    printf("Executing modifier: %c\n", node->value.modifier);
    break;
  case Fn:
    printf("Executing function: %c\n", node->value.fn);
    break;
  case DefinedFn:
    printf("Executing defined function\n");
    break;
  default:
    printf("Unknown node type\n");
    return 1;
  }
  return 0;
}
