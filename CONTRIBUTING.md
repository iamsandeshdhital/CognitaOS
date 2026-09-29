# Contributing to CognitaOS

Thank you for your interest in contributing to CognitaOS! This document provides guidelines and instructions for contributing.

---

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)

---

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

---

## Getting Started

### Prerequisites

- C++20 compiler (GCC 13+, Clang 17+, MSVC 2022+)
- CMake 3.28+
- Ninja build system
- Python 3.11+ (for code generation scripts)

### Clone and Build

```bash
git clone https://github.com/iamsandeshdhital/CognitaOS.git
cd CognitaOS
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Debug
ninja
```

### Run Tests

```bash
ctest --output-on-failure
```

---

## Development Workflow

1. **Fork** the repository
2. **Create a branch** from `main`:
   ```bash
   git checkout -b feature/my-feature
   ```
3. **Make your changes** following our coding standards
4. **Write tests** for new functionality
5. **Update documentation** as needed
6. **Commit** with a clear message:
   ```
   feat: add photonic interconnect driver

   Implements the silicon photonics HAL driver for
   sub-nanosecond inter-crystal communication.
   ```
7. **Push** to your fork
8. **Open a Pull Request**

---

## Coding Standards

### C++ Style

- Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with modifications
- Use 4-space indentation (no tabs)
- Maximum line length: 100 characters
- Use `struct` for passive data, `class` for active objects
- Prefer `auto` for complex types
- Use `nullptr`, not `NULL` or `0`

### Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Types | PascalCase | `struct AttentionScheduler` |
| Functions | PascalCase | `ComputeAttention()` |
| Variables | camelCase | `attentionWeights` |
| Member variables | trailing_ | `query_` |
| Constants | SCREAMING_SNAKE | `MAX_ENTITIES` |
| Macros | SCREAMING_SNAKE | `COG_MAGIC` |
| Files | snake_case | `attention_scheduler.c` |

### Comments

```c
/**
 * @brief Computes attention weights for all runnable entities.
 *
 * This is the core scheduling decision function. It uses a single-pass
 * attention mechanism to simultaneously decide what runs where, for
 * how long, with what memory, at what power state.
 *
 * @param query The system context query vector (dim = model_dim)
 * @param keys Entity key vectors [num_entities, model_dim]
 * @param values Entity value vectors [num_entities, model_dim]
 * @param num_entities Number of runnable entities
 * @param output Output allocation matrix
 * @return COG_OK on success, negative error code on failure
 */
int attention_schedule(const float *query,
                       const float *keys,
                       const float *values,
                       uint32_t num_entities,
                       struct attention_schedule_output *output);
```

---

## Testing

### Unit Tests

Every new function must have unit tests. Place tests in `tests/unit/`.

```c
#include <cognita/test.h>

TEST(AttentionScheduler, BasicScheduling) {
    struct attention_scheduler *sched = attention_scheduler_create();
    ASSERT_NE(sched, NULL);

    struct attention_schedule_output output;
    int ret = attention_schedule_compute(sched, &output);

    ASSERT_EQ(ret, COG_OK);
    ASSERT_GT(output.allocation_count, 0);

    attention_scheduler_destroy(sched);
}
```

### Integration Tests

Test component interactions in `tests/integration/`.

### Benchmarks

Performance benchmarks in `tests/benchmarks/`. All benchmarks must include baseline comparison.

```bash
ninja bench
```

---

## Documentation

- Update `docs/` for any architectural changes
- Update `CHANGELOG.md` for user-visible changes
- Add doc comments to all public API functions
- Update `README.md` if adding new features

---

## Pull Request Process

1. Ensure all tests pass: `ninja test`
2. Ensure code is formatted: `ninja format`
3. Ensure no compiler warnings: `ninja clean && ninja`
4. Update documentation
5. Request review from at least one maintainer
6. Address review comments
7. Squash commits if requested
8. Maintainer merges

---

## Commit Message Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`

Example:
```
feat(kernel): add multi-horizon temporal scheduling

Implements the Temporal Resource Fabric that couples scheduling
decisions across five time scales from nanoseconds to minutes.

Closes #123
```

---

## Questions?

Open a [GitHub Discussion](https://github.com/iamsandeshdhital/CognitaOS/discussions) or reach out to the maintainers.

---

Thank you for contributing to CognitaOS!
