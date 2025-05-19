#include "exec.h"
#include "lex.h"
#include "prints.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Define the global execution context
struct ExecutionContext g_exec_context = {0};

int exec(struct Node *node, struct Stack *stack) {
  if (stack->head >= stack->capacity) {
    stack->capacity *= 2;
    stack->items =
        realloc(stack->items, sizeof(struct StackItem) * stack->capacity);
  }
  switch (node->type) {
  case Int:
    stack->head += 1;
    stack->items[stack->head].type = IntItem;
    stack->items[stack->head].value.int_value = node->value.i;
    break;
  case Float:
    stack->head += 1;
    stack->items[stack->head].type = FloatItem;
    stack->items[stack->head].value.float_value = node->value.f;
    break;
  case Operator: {
    int idx = stack->head;
    if (node->value.op == BitwiseNot) {
      if (stack->items[idx].type == IntItem) {
        stack->items[idx].value.int_value = ~stack->items[idx].value.int_value;
      } else {
        printf("Error: BitwiseNot operator is only for ints\n");
        exit(-1);
      }
    } else {
      if (stack->items[idx].type == ModifierItem) {
        if (stack->items[idx].value.modifier == Keep) {
          idx = stack->head;
          stack->head -= 1;
        }
      } else {
        idx = stack->head - 1;
      }

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
  case Fn:
    switch (node->value.fn) {
    case Repeat: {
      if (stack->items[stack->head].type != DefinedFnItem) {
        printf("Error: Repeat requires a function in head\n");
        exit(-1);
      }
      stack->head -= 2;
      if (stack->items[stack->head + 1].type != IntItem) {
        printf("Error: Repeat requires an integer count in head - 2 \n");
        exit(-1);
      }
      long repeat_count = stack->items[stack->head + 1].value.int_value;
      struct NodeArray *defined_fn =
          stack->items[stack->head + 2].value.defined_fn;
      for (int i = 0; i < repeat_count; i++) {
        g_exec_context.repeat_count = i + 1;
        g_exec_context.parent_token_index = stack->head + 2;

        for (int j = 0; j < defined_fn->len; j++) {
          exec_with_context(&defined_fn->tokens[j], stack, j, defined_fn->len,
                            i, -1);
        }
      }
      break;
    }
    case Exec: {
      if (stack->items[stack->head].type != DefinedFnItem) {
        printf("Error: Exec requires a function\n");
        exit(-1);
      }
      stack->head -= 1;
      struct NodeArray *defined_fn =
          stack->items[stack->head + 1].value.defined_fn;
      g_exec_context.parent_token_index = stack->head + 1;
      for (int i = 0; i < defined_fn->len; i++) {
        exec_with_context(&defined_fn->tokens[i], stack, i, defined_fn->len, 0,
                          -1);
      }
      break;
    }
    case Map: {
      if (stack->items[stack->head].type != DefinedFnItem) {
        printf("Error: Map requires a function\n");
        exit(-1);
      }
      stack->head -= 1;
      struct NodeArray *defined_fn =
          stack->items[stack->head + 1].value.defined_fn;
      for (int i = 0; i <= stack->head; i++) {
        struct StackItem this = stack->items[i];
        struct Stack virtual_stack = {0, 32, NULL};
        virtual_stack.items = malloc(sizeof(struct StackItem) * 32);
        virtual_stack.items[0] = this;
        g_exec_context.parent_token_index = stack->head + 1;
        for (int j = 0; j < defined_fn->len; j++) {
          exec_with_context(&defined_fn->tokens[j], &virtual_stack, j,
                            defined_fn->len, 0, i);
        }
        stack->items[i] = virtual_stack.items[virtual_stack.head];
        free(virtual_stack.items);
      }
    } break;
    case Fold: {
      stack->head -= 1;
      struct NodeArray *defined_fn =
          stack->items[stack->head + 1].value.defined_fn;

      int iterations = 0;
      int max_iterations = 1000;

      while (stack->head > 0 && iterations < max_iterations) {
        for (int i = 0; i < defined_fn->len; i++) {
          exec(&defined_fn->tokens[i], stack);
        }
        iterations++;
      }

      if (iterations >= max_iterations) {
        printf("Warning: Fold operation hit maximum iteration limit\n");
      }
      break;
    }
    case Reverse: {
      int left = 0;
      int right = stack->head;
      while (left < right) {
        struct StackItem temp = stack->items[left];
        stack->items[left] = stack->items[right];
        stack->items[right] = temp;
        left++;
        right--;
      }
      break;
    }
    case swap: {
      if (stack->items[stack->head].type != IntItem) {
        printf("Error: swap requires an integer argument\n");
        exit(-1);
      }
      long n = stack->items[stack->head].value.int_value;
      stack->head -= 1;

      struct StackItem temp = stack->items[stack->head];
      stack->items[stack->head] = stack->items[stack->head - n];
      stack->items[stack->head - n] = temp;
      break;
    }
    case Duplicate: {
      stack->head += 1;
      stack->items[stack->head] = stack->items[stack->head - 1];
      break;
    }
    case Pop: {
      print_stack_item(&stack->items[stack->head]);
      printf("\n");
      stack->head -= 1;
      break;
    }
    case Floor: {
      if (stack->items[stack->head].type == FloatItem) {
        stack->items[stack->head].value.int_value =
            (long)floor(stack->items[stack->head].value.float_value);
        stack->items[stack->head].type = IntItem;
      } else {
        printf("Error: Floor operator is only for floats\n");
        exit(-1);
      }
      break;
    }
    case Ceil: {
      if (stack->items[stack->head].type == FloatItem) {
        stack->items[stack->head].value.int_value =
            (long)ceil(stack->items[stack->head].value.float_value);
        stack->items[stack->head].type = IntItem;
      } else {
        printf("Error: Ceil operator is only for floats\n");
        exit(-1);
      }
    } break;
    }
    break;
  case DefinedFn:
    stack->head += 1;
    stack->items[stack->head].type = DefinedFnItem;
    stack->items[stack->head].value.defined_fn = node->value.defined_fn;
    break;
  case Modifier: {
    switch (node->value.modifier) {
    case Keep:
      stack->head += 1;
      stack->items[stack->head].type = ModifierItem;
      stack->items[stack->head].value.modifier = node->value.modifier;
      break;
    }
  }
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
      printf("Error: Modulo operator is only for ints\n");
      exit(-1);
    }
    break;
  case BitwiseAnd:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] & p.value.i[1];
    } else {
      printf("Error: BitwiseAnd operator is only for ints\n");
      exit(-1);
    }
    break;
  case BitwiseOr:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] | p.value.i[1];
    } else {
      printf("Error: BitwiseOr operator is only for ints\n");
      exit(-1);
    }
    break;
  case BitwiseXor:
    if (p.type == IntPair) {
      result.value.i = p.value.i[0] ^ p.value.i[1];
    } else {
      printf("Error: BitwiseXor operator is only for ints\n");
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

// Add the implementation for exec_with_context
int exec_with_context(struct Node *node, struct Stack *stack, int token_index,
                      int function_id, int repeat_iter, int map_idx) {
  // Save previous context
  int prev_repeat_count = g_exec_context.repeat_count;
  int prev_parent_token_index = g_exec_context.parent_token_index;
  int prev_token_index = g_exec_context.current_token_index;

  // Update context
  if (token_index >= 0) {
    g_exec_context.current_token_index = token_index;
  }

  if (function_id >= 0) {
    g_exec_context.in_function = 1;
    g_exec_context.function_id = function_id;
  }

  if (repeat_iter >= 0) {
    g_exec_context.repeat_count = repeat_iter + 1;
  }

  if (map_idx >= 0) {
    g_exec_context.map_index = map_idx;
  }

  // Execute the node
  int result = exec(node, stack);

  // Restore previous context
  g_exec_context.repeat_count = prev_repeat_count;
  g_exec_context.parent_token_index = prev_parent_token_index;
  g_exec_context.current_token_index = prev_token_index;

  return result;
}

// Add reset function
void reset_execution_context(void) {
  g_exec_context.in_function = 0;
  g_exec_context.function_id = -1;
  g_exec_context.repeat_count = 0;
  g_exec_context.map_index = -1;
  g_exec_context.parent_token_index = -1;
  g_exec_context.current_token_index = -1;
}