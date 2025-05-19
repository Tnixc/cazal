#include "exec.h"
#include "lex.h"
#include <stdio.h>
#include <string.h>

int print_node_array(struct NodeArray *tokens) {
  if (tokens == NULL) {
    return -1;
  }
  for (int i = 0; i < tokens->len; i++) {
    printf("%d: ", i);
    printf("%c -> ", tokens->tokens[i].type);
    if (tokens->tokens[i].type == Int) {
      printf("%ld\n", tokens->tokens[i].value.i);
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
    printf("%ld", item->value.int_value);
  } else if (item->type == FloatItem) {
    printf("%f", item->value.float_value);
  } else if (item->type == DefinedFnItem) {
    printf("DefinedFn");
    printf("(");
    print_node_array(item->value.defined_fn);
    printf(")");
  } else if (item->type == ModifierItem) {
    printf("%c", item->value.modifier);
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

// External reference to buffer_printf defined in wasm.c
extern void buffer_printf(const char *format, ...);

// Implementation of the buffer-oriented printing functions
int print_node_array_to_buffer(struct NodeArray *tokens, char *buffer,
                               size_t buffer_size) {
  if (!tokens || !buffer || buffer_size == 0)
    return -1;

  size_t pos = 0;
  int written = 0;

  for (int i = 0; i < tokens->len && pos < buffer_size - 1; i++) {
    written = snprintf(buffer + pos, buffer_size - pos, "%d: %c -> ", i,
                       tokens->tokens[i].type);
    if (written > 0)
      pos += written;

    if (pos >= buffer_size - 1)
      break;

    if (tokens->tokens[i].type == Int) {
      written = snprintf(buffer + pos, buffer_size - pos, "%ld\n",
                         tokens->tokens[i].value.i);
    } else if (tokens->tokens[i].type == Float) {
      written = snprintf(buffer + pos, buffer_size - pos, "%f\n",
                         tokens->tokens[i].value.f);
    } else if (tokens->tokens[i].type == Operator) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n",
                         tokens->tokens[i].value.op);
    } else if (tokens->tokens[i].type == Modifier) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n",
                         tokens->tokens[i].value.modifier);
    } else if (tokens->tokens[i].type == Fn) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n",
                         tokens->tokens[i].value.fn);
    } else if (tokens->tokens[i].type == DefinedFn) {
      written = snprintf(buffer + pos, buffer_size - pos, "DefinedFn(\n");
      if (written > 0)
        pos += written;

      // Recursively print the tokens in the defined function
      size_t remaining = buffer_size - pos;
      if (remaining > 10) {
        char *sub_buffer = buffer + pos;
        int sub_result = print_node_array_to_buffer(
            tokens->tokens[i].value.defined_fn, sub_buffer, remaining);
        if (sub_result >= 0) {
          pos += strlen(sub_buffer);
        }
      }

      written = snprintf(buffer + pos, buffer_size - pos, ")\n");
    } else {
      written = snprintf(buffer + pos, buffer_size - pos, "\n");
    }

    if (written > 0)
      pos += written;
    if (pos >= buffer_size - 1)
      break;
  }

  buffer[pos] = '\0';
  return 0;
}

int print_stack_to_buffer(struct Stack *stack, char *buffer,
                          size_t buffer_size) {
  if (!stack || !buffer || buffer_size == 0)
    return -1;

  size_t pos = 0;
  int written = 0;

  written = snprintf(buffer + pos, buffer_size - pos, "[ ");
  if (written > 0)
    pos += written;

  for (int i = 0; i <= stack->head && pos < buffer_size - 1; i++) {
    if (stack->items[i].type == IntItem) {
      written = snprintf(buffer + pos, buffer_size - pos, "%ld",
                         stack->items[i].value.int_value);
    } else if (stack->items[i].type == FloatItem) {
      written = snprintf(buffer + pos, buffer_size - pos, "%f",
                         stack->items[i].value.float_value);
    } else if (stack->items[i].type == DefinedFnItem) {
      written = snprintf(buffer + pos, buffer_size - pos, "DefinedFn(...)");
    } else if (stack->items[i].type == ModifierItem) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c",
                         stack->items[i].value.modifier);
    }

    if (written > 0)
      pos += written;
    if (pos >= buffer_size - 1)
      break;

    if (i < stack->head) {
      written = snprintf(buffer + pos, buffer_size - pos, ", ");
      if (written > 0)
        pos += written;
    }
  }

  written = snprintf(buffer + pos, buffer_size - pos, " ]");
  if (written > 0)
    pos += written;

  buffer[pos] = '\0';
  return 0;
}
