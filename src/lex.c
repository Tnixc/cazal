#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
  Int = 'i',
  Float = 'f',
  Operator = 'o',

  OpenParen = '(',
  CloseParen = ')',
  // () are used to denote functions. Functions get pushed onto the stack

  Modifier = 'M',
  Fn = 'F',
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

struct Token {
  enum TokenType type;
  union {
    int i;
    float f;
    char operator;
    char modifier;
    char fn;
  } value;
};

struct TokenArray {
  struct Token tokens[4096];
  int len;
};

struct TokenArray *lex(char contents[]) {
  struct TokenArray *tokens = malloc(sizeof(struct TokenArray));
  tokens->len = 0;
  for (int i = 0; i < strlen(contents); i++) {
    if (isdigit(contents[i])) {
      char number[60];
      int j = 0;
      int has_float = 0;

      enum Base {
        Decimal = 10,
        Hexadecimal = 16,
        Octal = 8,
        Binary = 2,
      };
      enum Base base = Decimal;

      int cut = 0;
      if (contents[i] == '-') {
        number[j] = contents[i];
        j += 1;
      }
      if (contents[i] == '0') {
        switch (contents[i + 1]) {
        case 'x':
          cut = 2;
          base = Hexadecimal;
          break;
        case 'b':
          base = Binary;
          cut = 2;
          break;
        case 'o':
          base = Octal;
          cut = 2;
          break;
        }
      }

      while (isdigit(contents[i + j + cut]) ||
             (contents[i + j + cut] == '.' && has_float == 0) ||
             (strchr("abcdef", contents[i + j + cut]) != NULL)) {
        if (contents[i + j + cut] == '.') {
          has_float = 1;
        }

        number[j] = contents[i + j + cut];
        j += 1;
      }

      number[j] = '\0';

      if (has_float) {
        tokens->tokens[tokens->len].type = Float;
        tokens->tokens[tokens->len].value.f = atof(number);
      } else {
        long v = strtol(number, NULL, (base));
        tokens->tokens[tokens->len].type = Int;
        tokens->tokens[tokens->len].value.i = v;
      }

      i += j + cut;
      tokens->len += 1;
    } else if (isalpha(contents[i])) {
      char operator[60];
      operator[0] = contents[i];
      int j = 1;
      while (isalpha(contents[i + j])) {
        operator[j] = contents[i + j];
        j += 1;
      }
      operator[j] = '\0';

      struct {
        const char *name;
        char op;
        char type;
      } op_table[] = {{"rotate", Rotate, Fn},
                      {"swap", Swap, Fn},
                      {"duplicate", Duplicate, Fn},
                      {"pop", Pop, Fn},
                      {"map", Map, Fn},
                      {"fold", Fold, Fn},
                      {"repeat", Repeat, Modifier},
                      {"keep", Keep, Modifier},
                      {NULL, 0, 0}};

      int found = 0;
      for (int k = 0; op_table[k].name != NULL; k++) {
        if (strcmp(operator, op_table[k].name) == 0) {
          found = 1;

          tokens->tokens[tokens->len].type = op_table[k].type;
          tokens->tokens[tokens->len].value.operator= op_table[k].op;
          break;
        }
      }

      if (!found) {
        printf("Unknown operator: %s\n", operator);
      } else {
        tokens->len += 1;
      }
      i += j;
    } else if (contents[i] == '/' && contents[i + 1] == '*') {
      while (contents[i] != '*' || contents[i + 1] != '/') {
        i += 1;
      }
      i += 2; // comments
    }

    else if (strchr("+-*/%&|^~()", contents[i]) != NULL) {
      tokens->tokens[tokens->len].type = Operator;

      switch (contents[i]) {
      case '+':
        tokens->tokens[tokens->len].value.operator= Plus;
        break;
      case '-':
        tokens->tokens[tokens->len].value.operator= Minus;
        break;
      case '*':
        tokens->tokens[tokens->len].value.operator= Multiply;
        break;
      case '/':
        tokens->tokens[tokens->len].value.operator= Divide;
        break;
      case '%':
        tokens->tokens[tokens->len].value.operator= Modulo;
        break;
      case '&':
        tokens->tokens[tokens->len].value.operator= BitwiseAnd;
        break;
      case '|':
        tokens->tokens[tokens->len].value.operator= BitwiseOr;
        break;
      case '^':
        tokens->tokens[tokens->len].value.operator= BitwiseXor;
        break;
      case '~':
        tokens->tokens[tokens->len].value.operator= BitwiseNot;
        break;
      case '(':
        tokens->tokens[tokens->len].type = OpenParen;
        break;
      case ')':
        tokens->tokens[tokens->len].type = CloseParen;
        break;
      }

      tokens->len += 1;
    }
  }

  for (int i = 0; i < tokens->len; i++) {
    printf("%d: ", i);
    printf("%c -> ", tokens->tokens[i].type);
    if (tokens->tokens[i].type == Int) {
      printf("%d\n", tokens->tokens[i].value.i);
    } else if (tokens->tokens[i].type == Float) {
      printf("%f\n", tokens->tokens[i].value.f);
    } else if (tokens->tokens[i].type == Operator) {
      printf("%c\n", tokens->tokens[i].value.operator);
    } else if (tokens->tokens[i].type == Modifier) {
      printf("%c\n", tokens->tokens[i].value.modifier);
    } else if (tokens->tokens[i].type == Fn) {
      printf("%c\n", tokens->tokens[i].value.fn);
    } else {
      printf("\n");
    }
  }
  return tokens;
}
