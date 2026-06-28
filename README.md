# Custom Interpreter Language

A minimal, line-numbered interpreted language written in C — designed for simplicity, low-level control, and direct hardware portability.

---

## Overview

This interpreter executes programs written as numbered instruction sequences. Each line starts with a line number, followed by a command. The language supports integer variables, character strings, 5 general-purpose registers, arithmetic, conditionals, loops, and basic I/O.

```
001 debug off
002 unix yes
003 decl int counter 0
004 expel_c "Hello, World!"
005 expel_c "cp"
006 kill
```

---

## Core Concepts

- **All variables are global and integer** (unless declared as `char`)
- **5 registers** are pre-declared: `r0` through `r4` — usable anywhere a variable can be used
- **Line numbers** are mandatory; every instruction must be prefixed with its line number (`xxx`)
- **`"cp"`** is a special string constant meaning *newline* (carriage print)
- Programs can be typed interactively or loaded from a `.txt` file

---

## Setup

Conventionally, the first two lines of a program are:

```
001 debug on/off
002 unix yes/no
```

| Command | Effect |
|---|---|
| `debug on` | Enables verbose execution logging |
| `debug off` | Silent execution |
| `unix yes` | Target is Linux or macOS (`clear` for screen) |
| `unix no` | Target is Windows (`cls` for screen) |

---

## Variables & Registers

### Declaring variables

```
xxx decl int  <name> <value>
xxx decl char <name> "<text>"
```

- `int` variables hold integer values
- `char` variables hold strings (up to 255 characters)
- Default value if omitted is `0`

**Examples:**
```
010 decl int  score  0
011 decl char label  "Player Score:"
```

### Registers

Five pre-declared integer registers: `r0`, `r1`, `r2`, `r3`, `r4`.
They behave identically to `int` variables but require no declaration.

---

## Output

### Print a number
```
xxx expel_n <register/variable>
```

### Print text or a character value
```
xxx expel_c <register>          → prints the letter at position r[n] (a=1, z=26)
xxx expel_c <variable>          → prints string if char, or letter if int
xxx expel_c "<text>"            → prints literal text
xxx expel_c "cp"                → prints a newline
```

**Examples:**
```
020 expel_c "Score: "
021 expel_n score
022 expel_c "cp"
```

---

## Input

```
xxx input_n <register/variable>
```

Reads an integer from stdin into a register or variable.

---

## Data Movement

### Move a value
```
xxx mov <source> to <destination>
```
Source and destination can each be a register, variable, or literal number.

```
030 mov 42    to r0
031 mov r0    to score
032 mov score to r1
```

### Swap two values
```
xxx swap <a> <b>
```
Works with any combination of registers and variables.

---

## Arithmetic

All arithmetic operates **in-place** on the first operand.

| Instruction | Operation |
|---|---|
| `add to <a> <b>` | `a += b` |
| `subb from <a> <b>` | `a -= b` |
| `times <a> by <b>` | `a *= b` |
| `divide <a> by <b>` | `a /= b` |

Each operand can be a register, variable, or integer literal.

```
040 add to r0 5
041 subb from score r1
042 times r0 by score
043 divide score by 2
```

---

## Control Flow

### `if` — Conditional skip

```
xxx if(<a> <op> <b>) <N>
```

If the condition is **false**, skips the next `N` instructions.  
Operators: `=`, `<`, `>`

```
050 if(score > 10) 2
051 expel_c "You lose"
052 goto 053
053 expel_c "You win!"
```

### `goto` — Unconditional jump

```
xxx goto <line>
```

Jumps execution to the specified line number.

### `while` — Conditional loop

```
xxx while(<a> <op> <b>) <N>
```

Executes the next `N` instructions repeatedly while the condition holds.

```
060 while(r0 < 5) 3
061 expel_n r0
062 expel_c "cp"
063 add to r0 1
```

### `for` — Count loop

```
xxx for(<a> <op> <b>) <N>
xxx for(<var> <start> to <end>) <N>
```

The second form initialises `var` to `start` and loops while `var < end`, auto-incrementing.

```
070 for(counter 0 to 5) 3
071 expel_c "step "
072 expel_n counter
073 expel_c "cp"
```

---

## Utility Commands

| Command | Description |
|---|---|
| `delay <N>ms` | Pause for N milliseconds |
| `delay <N>s` | Pause for N seconds |
| `cls` | Clear the screen |
| `kill` | Terminate the program |
| `loadscript <file.txt>` | Load and run an external script (replaces current program) |
| `>// <text>` | Comment — ignored at runtime |

---

## Demo Program

```
001 debug off
002 unix yes
003 >// ==============================
004 >// DEMO — CUSTOM INTERPRETER
005 >// ==============================
006 decl int counter 0
007 decl int limit 5
008 decl int temp 0
009 decl char titolo "DEMO INTERPRETER"
010 decl char linea "-------------------"
011 expel_c titolo
012 expel_c "cp"
013 expel_c linea
014 expel_c "cp"
015 delay 500ms
016 expel_c "FOR loop:"
017 expel_c "cp"
018 for(counter 0 to 5) 4
019 expel_c "counter = "
020 expel_n counter
021 expel_c "cp"
022 delay 300ms
023 expel_c "cp"
024 expel_c "Math:"
025 expel_c "cp"
026 mov 10 to counter
027 add to counter 5
028 expel_c "10 + 5 = "
029 expel_n counter
030 expel_c "cp"
031 subb from counter 3
032 expel_c "15 - 3 = "
033 expel_n counter
034 expel_c "cp"
035 times counter by 2
036 expel_c "12 * 2 = "
037 expel_n counter
038 expel_c "cp"
039 divide counter by 4
040 expel_c "24 / 4 = "
041 expel_n counter
042 expel_c "cp"
043 delay 500ms
044 expel_c "cp"
045 expel_c "IF test:"
046 expel_c "cp"
047 if(counter = 6) 2
048 expel_c "Counter is NOT 6"
049 goto 051
050 expel_c "Counter is 6!"
051 expel_c "cp"
052 expel_c "WHILE loop:"
053 expel_c "cp"
054 mov 0 to temp
055 while(temp < 3) 4
056 expel_c "temp = "
057 expel_n temp
058 expel_c "cp"
059 add to temp 1
060 delay 500ms
061 expel_c "cp"
062 expel_c "SWAP test:"
063 expel_c "cp"
064 mov 100 to counter
065 mov 200 to temp
066 swap counter temp
067 expel_c "counter: "
068 expel_n counter
069 expel_c "cp"
070 expel_c "temp: "
071 expel_n temp
072 expel_c "cp"
073 delay 500ms
074 expel_c linea
075 expel_c "cp"
076 expel_c "END OF DEMO"
077 expel_c "cp"
078 expel_c linea
079 expel_c "cp"
080 kill
```

---

## How to Build & Run

### Compile
```bash
gcc interpreter.c -o interpreter
```

### Run interactively
```bash
./interpreter
```
Type your program line by line, then type `run` to execute. Type `cls` to clear and restart.

### Run from file
```bash
./interpreter my_program.txt
```

Script file format — one instruction per line, with its line number:
```
001 debug off
002 unix yes
003 decl int x 10
004 expel_n x
005 kill
```

---

## Limitations & Notes

- Maximum **256 integer variables** and **256 char variables**
- Maximum **1024 lines** per program
- Variable names: up to **15 characters**
- String values: up to **255 characters**
- `char` variables used with `expel_c` print their string content directly; `int` variables used with `expel_c` print the corresponding letter (`1`→`a`, `2`→`b`, …, `26`→`z`)
- `function` and `connect` commands are **not yet implemented**
- Division by zero is caught and reported, but does not halt execution

---

## Planned Features

- [ ] `function <name> (hex_id) from <line> to <line>` — subroutine support
- [ ] `connect to <SSID> with <PSW>` — WiFi connectivity (for embedded targets)
