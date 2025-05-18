#ifndef LEX_H
#define LEX_H

struct NodeArray *lex(char contents[]);

enum NodeType {
  Int = 'i',
  Float = 'f',
  Operator = 'o',

  // () are used to denote functions. Functions get pushed onto the stack
  Modifier = 'M',
  Fn = 'F',
  DefinedFn = 'D',
};

// Operators pop the top 2 values from the stack, and push the result back onto
// the stack
enum Operator {
  Plus = '+',
  Minus = '-',
  Multiply = '*',
  Divide = '/',
  Modulo = '%',
  BitwiseAnd = '&',
  BitwiseOr = '|',
  BitwiseXor = '^',
  BitwiseNot = '~',
};

// Modifiers pop a modifier onto the stack that affects the next function
enum Modifier {
  Keep = 'k',
  Repeat = 'l',
};

// Functions edit the stack
enum Fn {
  Map = 'm',
  Fold = 'f',
  Reverse = 'r',
  Rotate = 'R',
  Swap = 'S',
  Duplicate = 'D',
  Pop = 'P', // pop also emits the top value of the stack
};

struct Node {
  enum NodeType type;
  union {
    int i;
    float f;
    char op;
    char modifier;
    char fn;
    struct NodeArray *defined_fn;
  } value;
};

struct NodeArray {
  struct Node *tokens;
  int len;
  int capacity;
};

#endif /* LEX_H */