# Stack

## Overview
This is a stack for any type of data.

> [!NOTE]
> To correctly `print` popped values, give a proper `printer` type of print_t to `stack_ctor`.

## Build
Run `make` to build program in default mode.

`./build/meow` to run program.

> [!NOTE]
> Default mode is `release` mode

---

Run `make release` to build release version of the program.

### Release mode:
- Hash protection and canary protection are disabled.
- Sets compiler optimization level to **O2**.
- Defines **NDEBUG** (disables standard-C assertions and additional information about stack).

---

Run `make debug` to build debug version of the program.

### Debug mode:
- Hash protection and canary protection are available, additional information about stack is stored.
- Sets compiler optimization level to **O0**.
- Adds compilation flag **-ggdb3** (produces extra debugging information).

---

Run `make clean` to remove obj and dependencies files (clean build folder).

