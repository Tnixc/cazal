#include "exec.h"
#include "lex.h"
#include "prints.h"
#include <emscripten.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global state for the WASM environment
static struct NodeArray *g_tokens = NULL;
static struct Stack g_stack = {-1, 256, NULL};
static char *g_result_buffer = NULL;
static size_t g_buffer_size = 0;
static int g_error_state = 0;

// Initialize the stack
EMSCRIPTEN_KEEPALIVE
void init_environment() {
  if (g_stack.items == NULL) {
    g_stack.items = malloc(sizeof(struct StackItem) * g_stack.capacity);
  }
  
  g_stack.head = -1;
  
  if (g_result_buffer == NULL) {
    g_buffer_size = 4096;
    g_result_buffer = malloc(g_buffer_size);
  }
  
  g_error_state = 0;
}

// Clean up resources
EMSCRIPTEN_KEEPALIVE
void cleanup_environment() {
  if (g_tokens) {
    // Would need a proper deep cleanup function
    free(g_tokens->tokens);
    free(g_tokens);
    g_tokens = NULL;
  }
  
  if (g_stack.items) {
    free(g_stack.items);
    g_stack.items = NULL;
  }
  
  if (g_result_buffer) {
    free(g_result_buffer);
    g_result_buffer = NULL;
  }
}

// Custom printf replacement that writes to our buffer
void buffer_printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  
  // Make sure we have enough space
  size_t current_len = g_result_buffer ? strlen(g_result_buffer) : 0;
  size_t remaining = g_buffer_size - current_len;
  if (remaining < 256) {
    g_buffer_size *= 2;
    g_result_buffer = realloc(g_result_buffer, g_buffer_size);
  }
  
  // Append to buffer
  vsprintf(g_result_buffer + current_len, format, args);
  va_end(args);
}

// Override exit function to set error state instead of terminating
void custom_exit(int code) {
  g_error_state = code;
  buffer_printf("Error code: %d\n", code);
  // We don't actually exit in WASM, instead we'll check the error state
}

// WASM-exposed function to lex code
EMSCRIPTEN_KEEPALIVE
int lex_code(const char *code) {
  if (g_tokens) {
    free(g_tokens->tokens);
    free(g_tokens);
  }
  
  g_error_state = 0;
  g_tokens = lex(code);
  return g_tokens ? g_tokens->len : -1;
}

// WASM-exposed function to get serialized tokens
EMSCRIPTEN_KEEPALIVE
const char* get_tokens_string() {
  if (g_result_buffer) g_result_buffer[0] = '\0';
  if (!g_tokens) return g_result_buffer;
  
  print_node_array_to_buffer(g_tokens, g_result_buffer, g_buffer_size);
  return g_result_buffer;
}

// WASM-exposed function to execute code
EMSCRIPTEN_KEEPALIVE
int execute_all() {
  if (!g_tokens) return -1;
  if (g_error_state != 0) return -1;
  
  g_stack.head = -1; // Reset stack
  if (g_result_buffer) g_result_buffer[0] = '\0';
  
  for (int i = 0; i < g_tokens->len; i++) {
    exec(&g_tokens->tokens[i], &g_stack);
    if (g_error_state != 0) return -1;
  }
  
  return g_stack.head;
}

// Helper function to print stack item to buffer
void print_stack_item_to_buffer(struct StackItem *item) {
  if (item->type == IntItem) {
    buffer_printf("%d", item->value.int_value);
  } else if (item->type == FloatItem) {
    buffer_printf("%f", item->value.float_value);
  } else if (item->type == DefinedFnItem) {
    buffer_printf("DefinedFn(...)");
  } else if (item->type == ModifierItem) {
    buffer_printf("%c", item->value.modifier);
  }
  buffer_printf(" | ");
}

// WASM-exposed function to get current stack state
EMSCRIPTEN_KEEPALIVE
const char* get_stack_string() {
  if (g_result_buffer) g_result_buffer[0] = '\0';
  
  for (int i = 0; i <= g_stack.head; i++) {
    print_stack_item_to_buffer(&g_stack.items[i]);
  }
  buffer_printf("\n");
  
  return g_result_buffer;
}

// Execute a single step
EMSCRIPTEN_KEEPALIVE
int execute_step(int step_index) {
  if (!g_tokens || step_index < 0 || step_index >= g_tokens->len || g_error_state != 0) 
    return -1;
    
  exec(&g_tokens->tokens[step_index], &g_stack);
  if (g_error_state != 0) return -1;
  
  return g_stack.head;
}

// Get error state
EMSCRIPTEN_KEEPALIVE
int get_error_state() {
  return g_error_state;
}

// Reset error state
EMSCRIPTEN_KEEPALIVE
void reset_error_state() {
  g_error_state = 0;
}

// Add this function to reset the stack and error state
EMSCRIPTEN_KEEPALIVE
void reset_stack() {
  g_stack.head = -1;
  g_error_state = 0;
  if (g_stack.items == NULL) {
    g_stack.items = malloc(sizeof(struct StackItem) * g_stack.capacity);
  }
  // Optionally clear stack memory for safety
  if (g_stack.items) {
    memset(g_stack.items, 0, sizeof(struct StackItem) * g_stack.capacity);
  }
}

// Main function for standalone WASM execution
int main(int argc, char *argv[]) {
  init_environment();
  
  if (argc < 2) {
    printf("Usage: %s <code>\n", argv[0]);
    return 1;
  }
  
  char *contents = argv[1];
  struct NodeArray *tokens = lex(contents);
  
  if (g_error_state != 0) {
    printf("Error parsing code\n");
    return g_error_state;
  }
  
  print_node_array(tokens);
  
  struct Stack stack = {-1, 256, NULL};
  stack.items = malloc(sizeof(struct StackItem) * stack.capacity);
  
  for (int i = 0; i < tokens->len; i++) {
    exec(&tokens->tokens[i], &stack);
    if (g_error_state != 0) {
      printf("Error executing code\n");
      return g_error_state;
    }
    printf("head: %d : ", stack.head);
    print_stack(&stack);
  }
  
  free(stack.items);
  free(tokens->tokens);
  free(tokens);
  
  cleanup_environment();
  return 0;
} 