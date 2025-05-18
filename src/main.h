#ifndef MAIN_H
#define MAIN_H

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

#endif /* MAIN_H */ 