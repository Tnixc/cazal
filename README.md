# Cazal Stack-Based Programming Language

Cazal is a stack-based programming language where operations are performed on values stored in a stack. Programs are evaluated from left to right, pushing values onto the stack and executing operations that manipulate the stack.

## Core Concepts

### Stack

The stack is the central data structure in Cazal. Operations:

- Push values onto the stack
- Pop values off the stack
- Manipulate values on the stack

### Functions

Functions in Cazal are defined using parentheses `( )`. When a function is encountered during parsing, it's treated as a single unit that can be pushed onto the stack and executed later.

## Data Types

Cazal supports the following data types:

1. **Int** - Integer values
2. **Float** - Floating-point values
3. **DefinedFn** - User-defined functions enclosed in parentheses

## Operators

Operators are in postfix notation. For example, `a b +` means "push `a`, push `b`, add them together".

### Arithmetic Operators

| Operator | Description            |
| -------- | ---------------------- |
| `+`      | Addition               |
| `-`      | Subtraction            |
| `*`      | Multiplication         |
| `/`      | Division               |
| `%`      | Modulo (integers only) |

### Bitwise Operators

| Operator | Description         |
| -------- | ------------------- |
| `&`      | Bitwise AND         |
| `\|`     | Bitwise OR          |
| `^`      | Bitwise XOR         |
| `~`      | Bitwise NOT (unary) |

## Functions

### Stack Manipulation

| Function    | Description                                                                                                                                        | Effect                           |
| ----------- | -------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------- |
| `duplicate` | Duplicates the top value                                                                                                                           | [a] → [a a]                      |
| `pop`       | Removes and prints the top value                                                                                                                   | [a] → []                         |
| `reverse`   | Reverses the entire stack                                                                                                                          | [a b c] → [c b a]                |
| [n] `swap`  | swaps elements on the stack, takes top value as which item(starting from the top, so `1 swap` swaps the top 2 items, excluding the 1) to swap with | [a b c d e] 2 swap → [a b e d c] |

### Higher-Order Functions

| Function             | Description                                                                 |
| -------------------- | --------------------------------------------------------------------------- |
| [fn] `map`           | Applies a function to each element on the stack                             |
| [fn] `fold`          | Combines stack elements using a function                                    |
| [count, fn] `repeat` | Repeats a function n times, where n is the second-to-top value on the stack |
| [fn] `exec`          | Executes a function once (1 repeat)                                         |

### Type Conversion

| Function | Description                              |
| -------- | ---------------------------------------- |
| `floor`  | Converts a float to int by rounding down |
| `ceil`   | Converts a float to int by rounding up   |

## Examples

### Basic Arithmetic

```
2 3 +
```

Pushes 2 and 3 onto the stack, then adds them, resulting in 5.

### Factorial Calculation

```
0 1 2 3 4 5 6 7 8 9 ( 1 + ) map ( * ) fold
```

This calculates 10!:

1. Push numbers 0-9 onto the stack
2. Map the function `( 1 + )` to each element, resulting in 1-10
3. Fold using multiplication, calculating the product: 10!

### Using Repeat

```
1 ( 2 * ) 5 repeat
```

This pushes 1, 5 onto the stack, then repeats the function `( 2 * )` which doubles the value on the stack. The result is 5 \* 2^5 = 160.

### Using Map and Custom Functions

```
1 2 3 4 ( 2 + 3 * ) map
```

This pushes 1, 2, 3, 4 onto the stack, then applies the function `( 2 + 3 * )` to each element. For each value `n`, it computes `(n+2)*3`.

## Error Handling

Cazal provides error messages for common issues:

- Type mismatches (e.g., using bitwise operations on floats)
- Unmatched parentheses
- Unknown operators or functions
