#include "lex.h"
#include "prints.h"

enum State {
  map,
  fold,
  repeat,
  keep,
};

struct Stack {
  union {
    int int_value;
    float float_value;
  } stack;
  enum State state;
};

int main(int argc, char *argv[]) {
  char contents[] = "3.14 0xff 0b11111110 08 /* comment 123 */ ~ 12 duplicate "
                    "12 map fold repeat keep ^"
                    "( 2 *) ( 3 ( 4 + 4) map ) map";

  struct NodeArray *tokens = lex(contents);

  print_node_array(tokens);

  return 0;
}
