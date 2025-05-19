#include "lex.h"
#ifndef EXEC_H
#define EXEC_H

enum StackItemType {
  DefinedFnItem,
  IntItem,
  FloatItem,
  ModifierItem,
};

struct StackItem {
  enum StackItemType type;
  union {
    struct NodeArray *defined_fn;
    long int_value;
    float float_value;
    enum Modifier modifier;
    enum Fn function;
  } value;
};

struct Stack {
  int head;
  int capacity;
  struct StackItem *items;
};

struct Pair {
  union {
    long i[2];
    float f[2];
  } value;
  enum {
    IntPair,
    FloatPair,
  } type;
};

union IntOrFloat {
  long i;
  float f;
};

struct IF {
  union IntOrFloat value;
  enum {
    I,
    F,
  } type;
};

// Complete the ExecutionContext struct definition
struct ExecutionContext {
  int in_function;
  int function_id;
  int repeat_count;
  int map_index;
  int parent_token_index;
  int current_token_index;
};

// Declare the global execution context
extern struct ExecutionContext g_exec_context;

// Add function declarations
struct Pair match_types(struct Stack *stack, int index);
struct IF operator_exec(struct Stack *stack, int index, enum Operator op);
int exec(struct Node *node, struct Stack *stack);
int exec_with_context(struct Node *node, struct Stack *stack, int token_index,
                      int function_id, int repeat_iter, int map_idx);
void reset_execution_context(void);

#endif
