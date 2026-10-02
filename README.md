# M2 Starter - Extensible Processing Architecture

Read the M2 specification before coding. This starter begins from a working M1 processing baseline and introduces the M2 extension points.

## M2 in one view

M2 adds architectural depth rather than a new retrieval feature set. You will:

1. complete the three supplied strategy interfaces as safe abstract polymorphic base classes;
2. refactor `ProcessingCore` so it owns and uses the selected strategies through those interfaces;
3. preserve M1 behavior under the default configuration;
4. support constructor-injected alternative strategies at runtime;
5. use `std::unique_ptr` and move-only ownership safely;
6. write meaningful student tests; and
7. explain and reason about your implementation in `DESIGN.md`.

M2 does **not** introduce a new retrieval algorithm. Its purpose is to make the M1 processing architecture extensible, safely owned, testable, and well understood.

## Build and run

```bash
mkdir build && cd build
cmake ..
make
ctest --output-on-failure
```

The untouched starter is expected to compile, but the M2 runtime-composition public tests will fail until the required implementation is completed.

## Public-interface design tasks

Unlike M1, three strategy headers are intentionally student-completed:

- `include/aiws/chunking_strategy.hpp`
- `include/aiws/retrieval_strategy.hpp`
- `include/aiws/context_strategy.hpp`

The required class names, operation names, signatures, return types, const qualification, and namespaces are fixed integration contracts. Your responsibility is to complete each as a safe abstract polymorphic interface, including correct polymorphic destruction and required pure-virtual behavior.

`include/aiws/processing_core.hpp` remains course-supplied as the stable integration contract.

## Tests

- `tests/public_tests.cpp` - course-provided public tests. Read and run this file, but do not modify it.
- `tests/student_tests.cpp` - scaffold for your own meaningful M2 tests. These tests are manually assessed.
- additional instructor-controlled hidden tests are used by Gradescope.

## AI-assisted development

AI assistance is permitted. You remain responsible for understanding all submitted code and design decisions. Working code alone is not sufficient evidence of mastery; `DESIGN.md` is part of the assessed technical-understanding evidence.

## Submission

Submit:

- `src/`
- the three student-modified strategy headers listed above
- `tests/student_tests.cpp`
- `DESIGN.md`

Do not modify or submit generated build products. Do not present course-provided tests as student-authored work.
