# SKester (Seaker's tester)

**SKester** is a lightweight test runner for C designed to make 
writing and running tests simple, similar to `cargo test` in Rust.

It uses **ELF sections** to automatically register test cases at 
compile time and provides a small CLI runner to discover and execute them.

The project is intentionally minimal and requires only **GCC and an ELF-based system**.

---

# Features

* Lightweight (~1000 LOC runner)
* Header-only test definitions
* Automatic test discovery using ELF sections
* Suite-based organization
* Minimal assertion macros
* Colored terminal output
* No external dependencies

---

# Requirements

* GCC 
* ELF-compatible system (Linux / BSD)

---

# Building SKester

Clone the repository and build the runner:

```bash
git clone https://github.com/seakerOner/skester
cd skester
make
```

The binary will be created in:

```
build/skester
```

Clean build files:

```bash
make clean
```

---

# Writing Tests

Tests are written using the `TEST` macro provided by `skester.h`.

Example:

```c
#include "skester.h"

TEST(math, addition) {
    int result = 2 + 2;
    SKS_EQ(result, 4);
}

TEST(math, comparison) {
    int value = 5;
    SKS_ASSERT(value > 0);
}
```

Each test automatically registers metadata in a special ELF section at compile time.

---

# Assertions

SKester provides a minimal set of assertion macros.

```
SKS_ASSERT(expression)
SKS_ASSERT_MSG(expression, message)
SKS_EQ(a, b)
SKS_NE(a, b)
```

Example:

```c
SKS_ASSERT(ptr != NULL);

SKS_ASSERT_MSG(x > 0, "value must be positive\n");

SKS_EQ(result, expected);

SKS_NE(ptr, NULL);
```

If an assertion fails, the test terminates immediately and the runner reports the failure.

---

# Compiling Test Binaries

Your test binary must include `skester.h` and define `SKESTER_IMPL` in **one** source file.

Example:

```c
#include "skester.h"
#define SKESTER_IMPL
```

Then compile your tests normally with GCC.

Example:

```bash
gcc tests.c -o build/tests
```

---

# Running Tests

Run tests using the `skester` runner.

Run all tests in specific suites:

```bash
skester -f ./tests test -s math utils
```

Run specific test cases:

```bash
skester -f ./tests -s math test add subtract
```

Run a test case across all suites:

```bash
skester -f ./tests test add
```

Run all tests from one suite:

```bash
skester -f ./tests test -s math
```

---

# Listing Tests

List tests inside specific suites:

```bash
skester -f ./tests list math utils
```

Alternative syntax:

```bash
skester -f ./tests list -s math utils
```

List all registered suites and tests:

```bash
skester -f ./tests listall
```

---

# CLI Usage

```
skester -f <test_binary> [COMMANDS]
```

### Flags

```
-f <file>        Path to executable containing tests
-s <suite...>    Select suites
```

### Commands

```
test <cases...>   Run test cases
list <suites...>  List test cases from suites
listall           List all suites
```

---

# Example Output

```
[RUN TEST] math/addition
[PASS] math/addition

[RUN TEST] math/comparison
[PASS] math/comparison
```

If a test fails:

```
Not equal: result != expected
Location: test_math.c:14
[FAIL] math/addition
```

---

# Project Goals

SKester aims to provide:

* A minimal testing tool for C
* A simple developer workflow
* No external dependencies
* Benchmarking Planned

---

# License

MIT

