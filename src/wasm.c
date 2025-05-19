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
  size_t remaining = g_buffer_size - strlen(g_result_buffer);
  if (remaining < 256) {
    g_buffer_size *= 2;
    g_result_buffer = realloc(g_result_buffer, g_buffer_size);
  }
  
  // Append to buffer
  vsprintf(g_result_buffer + strlen(g_result_buffer), format, args);
  va_end(args);
}

// WASM-exposed function to lex code
EMSCRIPTEN_KEEPALIVE
int lex_code(const char *code) {
  if (g_tokens) {
    free(g_tokens->tokens);
    free(g_tokens);
  }
  
  g_tokens = lex(code);
  return g_tokens ? g_tokens->len : -1;
}

// WASM-exposed function to get serialized tokens
EMSCRIPTEN_KEEPALIVE
const char* get_tokens_string() {
  if (g_result_buffer) g_result_buffer[0] = '\0';
  if (!g_tokens) return g_result_buffer;
  
  // Redirect print_node_array output to our buffer
  FILE *orig_stdout = stdout;
  // This would need to be implemented differently in a real solution
  // as we can't easily redirect stdout in WASM, but the concept is:
  // Temporarily replace printf with our buffer_printf
  
  print_node_array(g_tokens);
  
  return g_result_buffer;
}

// WASM-exposed function to execute code
EMSCRIPTEN_KEEPALIVE
int execute_all() {
  if (!g_tokens) return -1;
  
  g_stack.head = -1; // Reset stack
  if (g_result_buffer) g_result_buffer[0] = '\0';
  
  for (int i = 0; i < g_tokens->len; i++) {
    exec(&g_tokens->tokens[i], &g_stack);
    
    // Append execution state to result buffer
    buffer_printf("head: %d : ", g_stack.head);
    
    // Would need to modify print_stack to write to buffer
    for (int j = 0; j <= g_stack.head; j++) {
      print_stack_item_to_buffer(&g_stack.items[j]);
    }
    buffer_printf("\n");
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
  if (!g_tokens || step_index < 0 || step_index >= g_tokens->len) 
    return -1;
    
  exec(&g_tokens->tokens[step_index], &g_stack);
  return g_stack.head;
} 