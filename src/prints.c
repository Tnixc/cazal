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

// New function to print stack item to a provided buffer
void print_stack_item_to_buffer(struct StackItem *item, char *buffer, size_t *pos, size_t max_size) {
  if (*pos >= max_size - 1) return;

  int written = 0;
  
  if (item->type == IntItem) {
    written = snprintf(buffer + *pos, max_size - *pos, "%d", item->value.int_value);
  } else if (item->type == FloatItem) {
    written = snprintf(buffer + *pos, max_size - *pos, "%f", item->value.float_value);
  } else if (item->type == DefinedFnItem) {
    written = snprintf(buffer + *pos, max_size - *pos, "DefinedFn(...)");
  } else if (item->type == ModifierItem) {
    written = snprintf(buffer + *pos, max_size - *pos, "%c", item->value.modifier);
  }
  
  if (written > 0 && *pos + written < max_size - 4) {
    *pos += written;
    written = snprintf(buffer + *pos, max_size - *pos, " | ");
    if (written > 0) *pos += written;
  }
}

// Print the entire stack to a buffer
int print_stack_to_buffer(struct Stack *stack, char *buffer, size_t buffer_size) {
  if (!buffer || buffer_size == 0) return -1;
  
  size_t pos = 0;
  for (int i = 0; i <= stack->head; i++) {
    print_stack_item_to_buffer(&stack->items[i], buffer, &pos, buffer_size);
  }
  
  if (pos < buffer_size - 1) {
    buffer[pos++] = '\n';
    buffer[pos] = '\0';
  }
  
  return 0;
}

// Print the node array to a buffer
int print_node_array_to_buffer(struct NodeArray *tokens, char *buffer, size_t buffer_size) {
  if (!tokens || !buffer || buffer_size == 0) return -1;
  
  size_t pos = 0;
  for (int i = 0; i < tokens->len && pos < buffer_size - 1; i++) {
    int written = snprintf(buffer + pos, buffer_size - pos, "%d: %c -> ", i, tokens->tokens[i].type);
    if (written > 0) pos += written;
    
    if (pos >= buffer_size - 1) break;
    
    if (tokens->tokens[i].type == Int) {
      written = snprintf(buffer + pos, buffer_size - pos, "%d\n", tokens->tokens[i].value.i);
    } else if (tokens->tokens[i].type == Float) {
      written = snprintf(buffer + pos, buffer_size - pos, "%f\n", tokens->tokens[i].value.f);
    } else if (tokens->tokens[i].type == Operator) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n", tokens->tokens[i].value.op);
    } else if (tokens->tokens[i].type == Modifier) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n", tokens->tokens[i].value.modifier);
    } else if (tokens->tokens[i].type == Fn) {
      written = snprintf(buffer + pos, buffer_size - pos, "%c\n", tokens->tokens[i].value.fn);
    } else if (tokens->tokens[i].type == DefinedFn) {
      written = snprintf(buffer + pos, buffer_size - pos, "DefinedFn(...)\n");
    } else {
      written = snprintf(buffer + pos, buffer_size - pos, "\n");
    }
    
    if (written > 0) pos += written;
  }
  
  return 0;
}
