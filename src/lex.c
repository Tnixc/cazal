#include "lex.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NodeArray *lex(char contents[]) {
  struct NodeArray *tokens = malloc(sizeof(struct NodeArray));
  tokens->len = 0;
  tokens->capacity = 128; // Initial capacity
  tokens->tokens = malloc(sizeof(struct Node) * tokens->capacity);

  for (int i = 0; i < strlen(contents); i++) {
    // Check if we need to resize the array
    if (tokens->len >= tokens->capacity) {
      tokens->capacity *= 2;
      struct Node *new_tokens =
          realloc(tokens->tokens, sizeof(struct Node) * tokens->capacity);
      tokens->tokens = new_tokens;
    }

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
                      {"floor", Floor, Fn},
                      {"ceil", Ceil, Fn},
                      {NULL, 0, 0}};

      int found = 0;
      for (int k = 0; op_table[k].name != NULL; k++) {
        if (strcmp(operator, op_table[k].name) == 0) {
          found = 1;

          tokens->tokens[tokens->len].type = op_table[k].type;
          tokens->tokens[tokens->len].value.op = op_table[k].op;
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

    else if (strchr("+-*/%&|^~", contents[i]) != NULL) {
      tokens->tokens[tokens->len].type = Operator;

      switch (contents[i]) {
      case '+':
        tokens->tokens[tokens->len].value.op = Plus;
        break;
      case '-':
        tokens->tokens[tokens->len].value.op = Minus;
        break;
      case '*':
        tokens->tokens[tokens->len].value.op = Multiply;
        break;
      case '/':
        tokens->tokens[tokens->len].value.op = Divide;
        break;
      case '%':
        tokens->tokens[tokens->len].value.op = Modulo;
        break;
      case '&':
        tokens->tokens[tokens->len].value.op = BitwiseAnd;
        break;
      case '|':
        tokens->tokens[tokens->len].value.op = BitwiseOr;
        break;
      case '^':
        tokens->tokens[tokens->len].value.op = BitwiseXor;
        break;
      case '~':
        tokens->tokens[tokens->len].value.op = BitwiseNot;
        break;
      }
      tokens->len += 1;
    } else if (contents[i] == '(') {
      int j = 1; // Start at 1 to account for the opening parenthesis
      int depth = 1;

      // Find the matching closing parenthesis
      while (depth > 0 && i + j < strlen(contents)) {
        if (contents[i + j] == '(') {
          depth++;
        } else if (contents[i + j] == ')') {
          depth--;
        }
        j++;
      }

      if (depth != 0) {
        fprintf(stderr, "Unmatched parenthesis\n");
        free(tokens->tokens);
        free(tokens);
        exit(1);
      }

      // Extract the content between parentheses
      int content_len =
          j - 2; // Subtract 2 for the opening and closing parentheses
      char *defined_contents_fn = malloc(sizeof(char) * (content_len + 1));
      if (defined_contents_fn == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(tokens->tokens);
        free(tokens);
        exit(1);
      }

      strncpy(defined_contents_fn, contents + i + 1, content_len);
      defined_contents_fn[content_len] = '\0';

      struct NodeArray *defined_fn = lex(defined_contents_fn);
      free(defined_contents_fn);

      tokens->tokens[tokens->len].type = DefinedFn;
      tokens->tokens[tokens->len].value.defined_fn = defined_fn;
      tokens->len++;

      i += j - 1; // Move past the closing parenthesis
    }
  }
  return tokens;
}
