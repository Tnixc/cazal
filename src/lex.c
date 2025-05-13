#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
  Int,
  Float,
  Operator,
};

enum Operator {
  Plus = '+',
  Minus = '-',
  Multiply = '*',
  Divide = '/',
  Modulo = '%',
  Rotate = 'R',
  Swap = 'S',
  Duplicate = 'D',
  Pop = 'P',
  BitwiseAnd = '&',
  BitwiseOr = '|',
  BitwiseXor = '^',
  BitwiseNot = '~',
};

struct Token {
  enum TokenType type;
  union {
    int i;
    float f;
    char operator;
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
      if (contents[i] == '0' && contents[i + 1] == 'x') {
        base = Hexadecimal;
        cut = 2;
      } else if (contents[i] == '0' && contents[i + 1] == 'b') {
        base = Binary;
        cut = 2;
      } else if (contents[i] == '0' && contents[i + 1] == 'o') {
        base = Octal;
        cut = 2;
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

      if (strcmp(operator, "rotate") == 0) {
        tokens->tokens[tokens->len].value.operator= Rotate;
      } else if (strcmp(operator, "swap") == 0) {
        tokens->tokens[tokens->len].value.operator= Swap;
      } else if (strcmp(operator, "duplicate") == 0) {
        tokens->tokens[tokens->len].value.operator= Duplicate;
      } else if (strcmp(operator, "pop") == 0) {
        tokens->tokens[tokens->len].value.operator= Pop;
      }
      tokens->tokens[tokens->len].type = Operator;
      i += j;
      tokens->len += 1;
    } else if (contents[i] == '/' && contents[i + 1] == '*') {
      while (contents[i] != '*' || contents[i + 1] != '/') {
        i += 1;
      }
      i += 2;
    } else if (contents[i] == '+') {
      tokens->tokens[tokens->len].value.operator= Plus;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '-') {
      tokens->tokens[tokens->len].value.operator= Minus;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '*') {
      tokens->tokens[tokens->len].value.operator= Multiply;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '/') {
      tokens->tokens[tokens->len].value.operator= Divide;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '%') {
      tokens->tokens[tokens->len].value.operator= Modulo;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '&') {
      tokens->tokens[tokens->len].value.operator= BitwiseAnd;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '|') {
      tokens->tokens[tokens->len].value.operator= BitwiseOr;
      tokens->tokens[tokens->len].type = Operator;
    } else if (contents[i] == '^') {
      tokens->tokens[tokens->len].value.operator= BitwiseXor;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    } else if (contents[i] == '~') {
      tokens->tokens[tokens->len].value.operator= BitwiseNot;
      tokens->tokens[tokens->len].type = Operator;
      tokens->len += 1;
    }
  }

  for (int i = 0; i < tokens->len; i++) {
    printf("%d: ", i);
    printf("%d -> ", tokens->tokens[i].type);
    if (tokens->tokens[i].type == Int) {
      printf("%d\n", tokens->tokens[i].value.i);
    } else if (tokens->tokens[i].type == Float) {
      printf("%f\n", tokens->tokens[i].value.f);
    } else if (tokens->tokens[i].type == Operator) {
      printf("%c\n", tokens->tokens[i].value.operator);
    }
  }
  return tokens;
}
