#include "lex.h"
#ifndef EXEC_H
#define EXEC_H

enum StackItemType {
  DefinedFnItem,
  IntItem,
  FloatItem,
  ModifierItem,
  FnItem,
};

struct StackItem {
  enum StackItemType type;
  union {
    struct NodeArray *defined_fn;
    int int_value;
    float float_value;
    enum Modifier modifier;
    enum Fn function;
  } value;
};

struct Stack {
  int head;
  struct StackItem items[256];
};

struct Pair {
  union {
    int i[2];
    float f[2];
  } value;
  enum {
    IntPair,
    FloatPair,
  } type;
};

union IntOrFloat {
  int i;
  float f;
};

struct IF {
  union IntOrFloat value;
  enum {
    I,
    F,
  } type;
};

struct Pair match_types(struct Stack *stack, int index);

struct IF operator_exec(struct Stack *stack, int index, enum Operator op);

int exec(struct Node *node, struct Stack *stack);

#endif
