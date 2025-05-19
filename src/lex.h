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
};

// Functions edit the stack
enum Fn {
  Repeat = 'l',
  Exec = 'e',
  Map = 'm',
  Fold = 'f',
  Reverse = 'r',
  swap = 'R',
  Duplicate = 'D',
  Pop = 'P', // pop also emits the top value of the stack
  Floor = 'F',
  Ceil = 'C',
};

struct Node {
  enum NodeType type;
  union {
    long i;
    float f;
    enum Operator op;
    enum Modifier modifier;
    enum Fn fn;
    struct NodeArray *defined_fn;
  } value;
};

struct NodeArray {
  struct Node *tokens;
  int len;
  int capacity;
};

#endif /* LEX_H */