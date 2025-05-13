#include "lex.h"

int main() {
  char contents[] = "3.14 0xff 0b11111110 08 /* comment 123 */ ~ 12 duplicate 12 ^";
  struct TokenArray *tokens = lex(contents);
  return 0;
}
