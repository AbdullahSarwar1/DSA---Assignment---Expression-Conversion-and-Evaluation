# DSA Assignment 1 – Expression Conversion and Evaluation

## Overview
This program:
1. Reads an **infix** arithmetic expression from standard input.
2. **Tokenizes** it (numbers, variables, operators, brackets).
3. Converts it to **postfix** (Reverse Polish Notation) using the Shunting-Yard algorithm.
4. Prompts the user for the value of each variable (via **stderr**).
5. **Evaluates** the postfix expression and prints the result to **stdout**.

---

## Supported Syntax
| Element | Examples |
|---------|---------|
| Binary operators | `+`, `-`, `*`, `/` |
| Grouping symbols | `( )`, `[ ]`, `{ }` |
| Integer constants | `0`, `42`, `100` |
| Variables | Any C++ identifier: `a`, `x1`, `my_var` |

Operator precedence: `*` and `/` bind tighter than `+` and `-`.  
All bracket types are treated equally and must be properly nested and matched.

---

## Compilation

**Requirements:** g++ with C++17 support (any modern Linux/macOS).

```bash
# Using Make
mingw32-make

# Or manually
g++ -std=c++17 -Wall -O2 -o expression_eval src/main.cpp
```

---

## Usage

```bash
./expression_eval
```

Type (or pipe) your infix expression and press Enter.

### Interactive example
```
$ ./expression_eval
a + b * (c + 2)
Enter value for a: 3
Enter value for b: 5
Enter value for c: 2
a b c 2 + * +
23
```

### Piped input
```bash
echo "a + b * (c + 2)" | ./expression_eval
```

---

## Output
| Stream | Content |
|--------|---------|
| **stdout** | Postfix expression, then evaluated result |
| **stderr** | All variable-value prompts and error messages |

---

## Exit Codes
| Code | Meaning |
|------|---------|
| `0` | Success |
| `1` | Syntax error (bad tokens, mismatched brackets, malformed expression) |
| `2` | Runtime error (missing variable value, stack underflow) |
| `3` | Logical error (division by zero) |

---

## More Examples

| Infix | Postfix | Notes |
|-------|---------|-------|
| `a + b * (c + 2)` | `a b c 2 + * +` | Basic precedence |
| `{x + y} * [z - 1]` | `x y + z 1 - *` | Mixed brackets |
| `a / 0` | `a 0 /` | Division by zero → exit code 3 |
| `a +` | — | Syntax error → exit code 1 |

---

## Project Structure
```
.
├── Makefile
├── README.md
└── main.cpp
```
