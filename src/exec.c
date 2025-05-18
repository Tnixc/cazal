#include "exec.h"
#include "lex.h"
#include <stdio.h>
#include <stdlib.h>

int exec(struct Node *node, struct Stack *stack) {
  switch (node->type) {
  case Int:
    stack->items[stack->head + 1].type = IntItem;
    stack->items[stack->head + 1].value.int_value = node->value.i;
    stack->head += 1;
    break;
  case Float:
    stack->items[stack->head + 1].type = FloatItem;
    stack->items[stack->head + 1].value.float_value = node->value.f;
    stack->head += 1;
    break;
  case Operator: {
    int idx = stack->head - 1;
    if (node->value.op == BitwiseNot) {
      idx = stack->head;
      if (stack->items[idx].type == IntItem) {
        stack->items[idx].value.int_value = ~stack->items[idx].value.int_value;
      } else {
        printf("Error: BitwiseNot operator is not supported for floats\n");
        exit(-1);
      }
    } else {
      struct IF r = operator_exec(stack, stack->head, node->value.op);
      if (r.type == I) {
        stack->items[idx].type = IntItem;
        stack->items[idx].value.int_value = r.value.i;
      } else {
        stack->items[idx].type = FloatItem;
        stack->items[idx].value.float_value = r.value.f;
      }
    }
    stack->head = idx;
  } break;
  case Modifier:
  case Fn:
  case DefinedFn:
    break;
  }
  return 0;
}

struct IF operator_exec(struct Stack *stack, int index, enum Operator op) {
  struct IF result;
  struct Pair p = match_types(stack, index);
  if (p.type == IntPair) {
    result.type = I;
  } else {
    result.type = F;
  }
  switch (op) {
  case Plus:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] + p.value.i[1];
    } else {
      result.value.f = p.value.f[0] + p.value.f[1];
    }
    break;
  case Minus:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] - p.value.i[1];
    } else {
      result.value.f = p.value.f[0] - p.value.f[1];
    }
    break;
  case Multiply:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] * p.value.i[1];
    } else {
      result.value.f = p.value.f[0] * p.value.f[1];
    }
    break;
  case Divide:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] / p.value.i[1];
    } else {
      result.value.f = p.value.f[0] / p.value.f[1];
    }
    break;
  case Modulo:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] % p.value.i[1];
    } else {
      printf("Error: Modulo operator is not supported for floats\n");
      exit(-1);
    }
    break;
  case BitwiseAnd:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] & p.value.i[1];
    } else {
      printf("Error: BitwiseAnd operator is not supported for floats\n");
      exit(-1);
    }
    break;
  case BitwiseOr:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] | p.value.i[1];
    } else {
      printf("Error: BitwiseOr operator is not supported for floats\n");
      exit(-1);
    }
    break;
  case BitwiseXor:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] ^ p.value.i[1];
    } else {
      printf("Error: BitwiseXor operator is not supported for floats\n");
      exit(-1);
    }
    break;
  case BitwiseNot:
    break;
  }
  return result;
}

struct Pair match_types(struct Stack *stack, int index) {
  struct Pair p;
  if (stack->items[index].type == stack->items[index - 1].type) {
    if (stack->items[index].type == IntItem) {
      p.type = IntPair;
      p.value.i[0] = stack->items[index - 1].value.int_value;
      p.value.i[1] = stack->items[index].value.int_value;
    } else {
      p.type = FloatPair;
      p.value.f[0] = stack->items[index - 1].value.float_value;
      p.value.f[1] = stack->items[index].value.float_value;
    }
  } else {
    if (stack->items[index - 1].type == IntItem &&
        stack->items[index].type == FloatItem) {
      p.type = FloatPair;
      p.value.f[0] = (float)stack->items[index - 1].value.int_value;
      p.value.f[1] = stack->items[index].value.float_value;
    } else {
      p.type = FloatPair;
      p.value.f[0] = stack->items[index - 1].value.float_value;
      p.value.f[1] = (float)stack->items[index].value.int_value;
    }
  }
  return p;
}