# AGENTS.md — OrdAGI Implementation Guide for LLM Agents

This file is the authoritative implementation guide for any LLM agent tasked with
building, extending, or maintaining the OrdAGI codebase. Read it completely before
writing a single line of code. Every architectural decision made here has a
theoretical reason documented in `docs/FRD.md`. When in doubt, read the FRD.

---

## 1. What You Are Building

OrdAGI is a **deterministic, zero-hallucination knowledge reasoning engine** built
in C. It is not a chatbot wrapper. It is not an LLM with a C backend. It is a
physics engine for knowledge — grounded in Corrado Giannantoni's Incipient
Differential Calculus (IDC) and H.T. Odum's Thermodynamic Grammar.

The system has five unified output projections from one mathematical object (the
Ordinal Matrix):
1. Dynamic ODE simulation (IDC state equations)
2. Dynamic emergy accounting (non-conservative Odum algebra)
3. Molecular/bioinformatics modelling (Ordinal Matrix specularity solver)
4. Natural language generation (Odum Thermodynamic Grammar)
5. Self-referential quality evaluation (thermodynamic sentence scoring)

These are NOT separate modules that call each other. They are different projections
of the same Ordinal Matrix. Do not build them as a pipeline. Build the Ordinal Matrix
first; the projections follow from it.

**The single constraint that overrides all others:**
Ordagi must never guess, infer, or statistically predict any output. Every assertion
must be mathematically derived from the CBOR ontology. If a concept is not resolved,
say so. Never fill gaps with plausible-sounding content.

---

## 2. Repository Structure

```
ordagi/
├── AGENTS.md                  ← You are here
├── README.md                  ← User-facing project overview
├── Makefile                   ← All build, test, and run targets
├── Dockerfile                 ← Multi-stage Debian build for Linux ELF binaries
├── .github/
│   └── workflows/
│       └── ci.yml             ← CI: build + test on every push
├── docs/
│   ├── FRD.md                 ← Functional Requirements Document (primary spec)
│   ├── theory/
│   │   ├── giannantoni_2006.pdf
│   │   ├── giannantoni_2010.pdf
│   │   ├── giannantoni_2011.pdf
│   │   └── odum_1983_fig634.png
│   └── api/                   ← Auto-generated from source (doxygen)
├── include/
│   ├── ordinal_tensor.h       ← OrdinalConcept + OrdinalValue structs
│   ├── ordinal_matrix.h       ← OrdinalMatrix struct + specularity solver
│   ├── ice_math.h             ← Tier 1 GL engine + Tier 2 IDC drift correction
│   └── verb_table.h           ← Odum verb-to-alpha compile-time constant
├── src/
│   ├── main.c                 ← REPL entry point
│   ├── memory.c               ← CBOR serialisation/deserialisation
│   ├── parser.c               ← SVO extraction + query depth classification
│   ├── nlg.c                  ← Odum Thermodynamic Grammar NLG engine
│   ├── nlg_quality.c          ← Self-referential sentence quality evaluator
│   ├── ordinal_matrix.c       ← Ordinal Matrix construction + specularity
│   ├── emergy.c               ← Dynamic emergy accounting
│   ├── ode_solver.c           ← IDC-ODE dynamic systems simulation
│   ├── mcp_client.c           ← Universal MCP JSON-RPC client (stdio)
│   └── ordagi_daemon.c        ← Autonomous background learning loop
├── tests/
│   ├── test_runner.c          ← Minimal test harness (no external framework)
│   ├── test_ice_math.c        ← Unit tests: GL weights, quality computation
│   ├── test_memory.c          ← Unit tests: CBOR round-trip serialisation
│   ├── test_parser.c          ← Unit tests: SVO extraction, depth classification
│   ├── test_nlg.c             ← Unit tests: sentence generation per interaction type
│   ├── test_nlg_quality.c     ← Unit tests: sentence quality scoring
│   ├── test_ordinal_matrix.c  ← Unit tests: matrix build, specularity, solve
│   ├── test_mcp_client.c      ← Unit tests: JSON-RPC message construction
│   ├── test_drift.c           ← Benchmark: Mercury precession (target 42.45 sec/cy)
│   └── fixtures/
│       ├── test_ontology.json ← Small bootstrap ontology for test isolation
│       └── test_ontology.cbor ← Pre-built CBOR for load/save tests
├── bin/                       ← Compiled binaries (git-ignored, built by make)
│   ├── ordagi                 ← Main REPL binary
│   └── ordagi-daemon          ← Daemon binary
├── data/
│   ├── ontology.cbor          ← Persistent brain (git-ignored after first run)
│   ├── ontology.json          ← Bootstrap seeds (committed to repo)
│   ├── learned_verbs.json     ← Runtime-grown verb table (git-ignored)
│   └── ice_output.csv         ← Simulation output (git-ignored)
└── host/
    ├── mcp-zotero/            ← Python MCP server: Zotero sqlite bridge
    │   ├── server.py
    │   ├── requirements.txt
    │   └── README.md
    └── mcp-wikipedia/         ← Python MCP server: Wikipedia API wrapper
        ├── server.py
        ├── requirements.txt
        └── README.md
```

### What goes where — hard rules

- **`include/`** — Header files only. No function implementations in headers except
  `static inline` mathematical primitives in `ice_math.h`. Every `.h` file has an
  include guard.
- **`src/`** — All C implementation files. One module per file. No file exceeds 600
  lines; if it does, split it.
- **`tests/`** — One test file per source module. Tests are plain C — no external
  frameworks. The test runner is `tests/test_runner.c`.
- **`bin/`** — Git-ignored. Never commit binaries.
- **`data/`** — `ontology.json` is committed. `ontology.cbor`, `learned_verbs.json`,
  and `ice_output.csv` are git-ignored (generated at runtime).
- **`docs/`** — `FRD.md` is the spec. Never contradict it in code. If code must
  deviate, update the FRD first and document why.

---

## 3. Build System

All interaction with the project goes through `make`. Never instruct a user to run
`gcc` directly.

### 3.1 Required Make Targets

| Target | What it does |
|---|---|
| `make` / `make all` | Build all binaries into `bin/` |
| `make build` | Same as `make all` — explicit alias |
| `make test` | Build and run full test suite; fail with non-zero exit if any test fails |
| `make test-unit` | Run unit tests only (no benchmarks) |
| `make test-drift` | Run Mercury precession benchmark; assert result is 42.45 ± 0.5 sec/cy |
| `make chat` | Build then launch `bin/ordagi` REPL |
| `make daemon` | Build then launch `bin/ordagi-daemon` in foreground |
| `make simulate` | Run ICE benchmark simulation; output to `data/ice_output.csv` |
| `make simulate-bio` | Run Ordinal Matrix solver on test molecular system |
| `make emergy` | Output dynamic emergy accounting from current ontology |
| `make clean` | Remove `bin/` and all generated `.o` files |
| `make docs` | Run doxygen to generate `docs/api/` |
| `make lint` | Run `cppcheck` over `src/` and `include/` |
| `make valgrind` | Run `bin/ordagi` test scenario under valgrind; zero errors required |
| `make init-data` | Bootstrap `data/ontology.cbor` from `data/ontology.json` if not present |

### 3.2 Compiler Flags (non-negotiable)

```makefile
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Werror \
          -Wno-unused-parameter \
          -I./include
LDFLAGS = -lcjson -lcbor -lgsl -lgslcblas -lm
DBGFLAGS = -g -fsanitize=address,undefined
```

- `-Werror` is mandatory. All warnings are errors. No exceptions.
- `-std=c11` is mandatory. No GNU extensions unless unavoidable and documented.
- Debug builds (`make debug`) add `-g -fsanitize=address,undefined`.
- Release builds (`make release`) add `-O2 -DNDEBUG`.

### 3.3 Docker Build

The primary build target runs inside a Debian container to produce Linux ELF
binaries. The Makefile `docker-build` target handles this:

```makefile
docker-build:
	docker build -t ordagi-builder .
	docker run --rm -v $(PWD):/workspace ordagi-builder make build
```

Native macOS builds (`make build-native`) are supported for development but the
Docker build is the reference. If behaviour differs between native and Docker,
the Docker build is correct.

---

## 4. Test-Driven Development Protocol

**Every function must have a test before it has an implementation.**

This is not a preference. The mathematical precision required by IDC means that
untested code cannot be trusted to be correct. A quality score that is off by
a rounding error in the GL weight recurrence will produce wrong Ordinal depths
throughout the entire ontology. Catch it in the test, not in production.

### 4.1 Test Structure

Each test file follows this pattern:

```c
/* tests/test_ice_math.c */
#include "test_runner.h"
#include "ice_math.h"

/* --- Test: GL weight recurrence correctness --- */
static void test_gl_weights_order_half(void) {
    double w[5];
    calculate_fractional_weights(w, 0.5, 5);
    /* w[0] must always be 1.0 */
    ASSERT_DOUBLE_EQ(w[0], 1.0, 1e-10);
    /* w[1] = 1.0 * (1 - (0.5+1)/1) = 1.0 * (-0.5) = -0.5 */
    ASSERT_DOUBLE_EQ(w[1], -0.5, 1e-10);
    /* w[2] = -0.5 * (1 - (0.5+1)/2) = -0.5 * (-0.25) = 0.125 */
    ASSERT_DOUBLE_EQ(w[2], 0.125, 1e-10);
}

static void test_quality_tier1_seed_cardinality(void) {
    /* A seed with cardinality 100.0 and alpha=1.0 (feed-back)
       must converge to a positive quality score */
    double q = compute_quality_tier1(100.0, 1.0);
    ASSERT_TRUE(q > 0.0);
    ASSERT_TRUE(q < 200.0); /* bounded */
}

static void test_quality_tier1_coproduction_higher_than_feedback(void) {
    /* co-production (alpha=0.5) must yield higher quality than
       feed-back (alpha=1.0) for the same cardinality input,
       because deeper memory accumulates more quality */
    double q_co  = compute_quality_tier1(100.0, 0.5);
    double q_fb  = compute_quality_tier1(100.0, 1.0);
    ASSERT_TRUE(q_co > q_fb);
}

void run_ice_math_tests(void) {
    RUN_TEST(test_gl_weights_order_half);
    RUN_TEST(test_quality_tier1_seed_cardinality);
    RUN_TEST(test_quality_tier1_coproduction_higher_than_feedback);
}
```

### 4.2 Test Runner

`tests/test_runner.h` provides a minimal assert macro set:

```c
/* tests/test_runner.h */
#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdio.h>
#include <math.h>

static int _tests_run    = 0;
static int _tests_passed = 0;
static int _tests_failed = 0;

#define ASSERT_TRUE(expr) do { \
    _tests_run++; \
    if (!(expr)) { \
        fprintf(stderr, "FAIL: %s:%d: ASSERT_TRUE(%s)\n", \
                __FILE__, __LINE__, #expr); \
        _tests_failed++; \
    } else { _tests_passed++; } \
} while(0)

#define ASSERT_FALSE(expr)        ASSERT_TRUE(!(expr))
#define ASSERT_INT_EQ(a, b)       ASSERT_TRUE((a) == (b))
#define ASSERT_STR_EQ(a, b)       ASSERT_TRUE(strcmp((a),(b)) == 0)
#define ASSERT_DOUBLE_EQ(a, b, eps) ASSERT_TRUE(fabs((a)-(b)) < (eps))
#define ASSERT_NOT_NULL(ptr)      ASSERT_TRUE((ptr) != NULL)
#define ASSERT_NULL(ptr)          ASSERT_TRUE((ptr) == NULL)

#define RUN_TEST(fn) do { \
    printf("  running %-50s", #fn); \
    fn(); \
    printf("ok\n"); \
} while(0)

#define PRINT_TEST_SUMMARY() do { \
    printf("\n%d tests: %d passed, %d failed\n", \
           _tests_run, _tests_passed, _tests_failed); \
} while(0)

#endif /* TEST_RUNNER_H */
```

### 4.3 TDD Workflow for Each Git Issue

For every git issue in the FRD (Section 8), the implementation sequence is:

```
1. Write the test file for the module (tests/test_<module>.c)
2. Run make test — confirm all new tests FAIL (red)
3. Write the minimum implementation to make tests pass
4. Run make test — confirm all tests PASS (green)
5. Run make valgrind — confirm zero memory errors
6. Run make lint — confirm zero warnings
7. Commit with message: "feat(#N): <description> — tests green, valgrind clean"
```

Never commit code that fails `make test`. Never commit code that has valgrind errors.

### 4.4 Mercury Precession Benchmark (make test-drift)

This is the most important single test in the repository. It validates that the
Tier 2 IDC drift correction is correctly implemented:

```c
/* tests/test_drift.c */
static void test_mercury_precession_tier2(void) {
    /* Reproduce Giannantoni (2006) Mercury precession result.
     * Input: Mercury orbital parameters (semi-major axis, eccentricity, period)
     * Expected: 42.45 ± 0.5 arcseconds per century
     * Traditional calculus gives: 0 arcseconds (complete failure)
     * This test MUST pass before any Tier 2 code is considered correct. */
    double precession = compute_mercury_precession_idc();
    ASSERT_DOUBLE_EQ(precession, 42.45, 0.5);
}
```

If `make test-drift` fails, Tier 2 is not correctly implemented regardless of what
other tests say.

---

## 5. Coding Standards

### 5.1 C Style

- **Indentation:** 4 spaces. No tabs. Ever.
- **Braces:** Opening brace on same line as control statement. Always use braces
  even for single-line bodies.
- **Names:** `snake_case` for all functions and variables. `SCREAMING_SNAKE` for
  constants and macros. `PascalCase` for typedefs only.
- **Line length:** 100 characters maximum.
- **Comments:** Every function has a block comment explaining its purpose, inputs,
  outputs, and any non-obvious mathematical reasoning. Do not describe *what* the
  code does — describe *why* in thermodynamic/mathematical terms.

```c
/*
 * compute_quality_tier1() — Tier 1 Cardinal Quality Score
 *
 * Computes the Grünwald-Letnikov approximation of concept quality.
 * This is a cardinal reduction of the full Ordinal value — correct and
 * useful, but not the complete IDC representation (see Tier 2).
 *
 * The GL recurrence approximates the fractional derivative memory:
 *   w[j] = w[j-1] * (1 - (alpha+1)/j)
 * A higher alpha means less memory retention (closer to standard ODE).
 * A lower alpha means deeper memory (more generative history retained).
 *
 * Parameters:
 *   cardinality  Raw energy quantity of the concept (1st Law conserved)
 *   alpha        Fractional order: 0.5=co-production, 1.0=feed-back, 2.0=inter-action
 *
 * Returns: Q[ICE_STEPS-1] — the converged quality score
 *
 * Ref: Giannantoni (2006) Eq. 2.2, ice_math.h
 */
double compute_quality_tier1(double cardinality, double alpha) {
    /* ... */
}
```

### 5.2 Memory Management

- Every `malloc` is paired with a `free`. No exceptions.
- Every heap allocation is checked for NULL immediately:
  ```c
  OrdinalConcept *node = malloc(sizeof(OrdinalConcept));
  if (!node) {
      fprintf(stderr, "[FATAL] malloc failed: OrdinalConcept\n");
      exit(EXIT_FAILURE);
  }
  ```
- `strncpy` is used instead of `strcpy`. Always. The destination size is always
  passed explicitly.
- Buffer sizes are always `sizeof(dest) - 1` for string operations.
- `valgrind --leak-check=full` must report zero errors and zero leaks on
  `make valgrind`.

### 5.3 Error Handling

- Functions that can fail return an `int` status code: `0` = success, `-1` = error.
- Functions that return a pointer return `NULL` on failure.
- Error messages go to `stderr`, never `stdout`.
- Fatal errors (out of memory, corrupt CBOR) call `exit(EXIT_FAILURE)` with a
  descriptive message.
- Non-fatal errors (concept not found, unknown verb) are returned to the caller —
  never silently ignored.

### 5.4 The Zero-Hallucination Invariant

This invariant must be maintained by every function in the NLG pipeline:

> **A function may only assert a relationship between two concepts if both concepts
> exist as resolved nodes in the knowledge_base[] array with ordinal_depth >= 0.**

In code, this means every NLG function begins with a resolved-concept check:

```c
if (!concept || concept->ordinal_depth < 0) {
    /* Frontier Node — do not generate language about this concept */
    return generate_frontier_response(concept_name);
}
```

Never generate language about a concept by inferring from its name, its aliases,
or partial matches. If it is not in the CBOR brain, it is a Frontier Node.

---

## 6. Module Implementation Order

Implement modules in dependency order. Do not implement a module before its
dependencies are tested and green.

```
Phase 1 (Foundation):
  include/ordinal_tensor.h      ← no dependencies
  include/verb_table.h          ← no dependencies
  include/ice_math.h            ← no dependencies
  src/memory.c                  ← depends on ordinal_tensor.h, libcbor
  src/main.c (REPL skeleton)    ← depends on memory.c

Phase 2 (Mathematics):
  ice_math.h Tier 2 functions   ← depends on Tier 1 tests passing
  src/memory.c CBOR extension   ← depends on OrdinalValue struct

Phase 3 (Language):
  src/parser.c                  ← depends on verb_table.h
  src/nlg.c                     ← depends on ordinal_tensor.h, parser.c
  src/nlg_quality.c             ← depends on nlg.c, ice_math.h

Phase 4 (MCP & Daemon):
  src/mcp_client.c              ← depends on libcjson only
  host/mcp-zotero/server.py     ← independent Python service
  host/mcp-wikipedia/server.py  ← independent Python service
  src/ordagi_daemon.c           ← depends on mcp_client.c, parser.c, memory.c

Phase 5 (Unified Applications):
  include/ordinal_matrix.h      ← depends on ordinal_tensor.h, ice_math.h
  src/ordinal_matrix.c          ← depends on ordinal_matrix.h, gsl
  src/ode_solver.c              ← depends on ordinal_matrix.c, ice_math.h
  src/emergy.c                  ← depends on ordinal_matrix.c
  UserModel + self-model        ← depends on memory.c, ordagi_daemon.c
```

---

## 7. The CBOR Brain — Non-Negotiable Rules

1. **Never store raw pointers in CBOR.** Parent references are always integer IDs.
   Pointers are resolved in a second pass after all records are loaded.
2. **Always append, never rewrite.** New concepts are appended to `ontology.cbor`.
   The file is never read fully into memory and rewritten — only appended.
3. **Two-pass load is mandatory.** Pass 1 loads all records. Pass 2 resolves
   pointers. Any single-pass loading implementation is incorrect.
4. **The bootstrap sequence:** If `data/ontology.cbor` does not exist, run
   `make init-data` which reads `data/ontology.json` and writes the initial CBOR.
   After that, `ontology.json` is never read again unless the CBOR is deleted.

---

## 8. MCP Client — Non-Negotiable Rules

1. **All external data arrives via MCP.** The C kernel never opens a database
   file, never makes HTTP requests directly, never reads from the filesystem
   except `data/ontology.cbor`, `data/learned_verbs.json`, and `data/ontology.json`.
2. **The MCP client speaks JSON-RPC 2.0 over stdio.** One request per line.
   One response per line. No persistent connections.
3. **MCP responses are parsed with cJSON.** Never use `sscanf` or string
   manipulation to parse JSON.
4. **Every MCP call has a timeout.** If the server does not respond within
   `MCP_TIMEOUT_MS` (default: 5000), log the failure and return `NULL`.

---

## 9. What Not To Do

These are the most likely implementation mistakes. Each one has destroyed an
otherwise correct implementation before.

**Do not add an LLM layer.** Not for "polish", not for "better phrasing", not
for "handling edge cases." If Ordagi cannot answer something from its CBOR brain,
it says so. That is the feature, not a bug.

**Do not use a global alpha constant.** Every concept interaction has its own
alpha derived from its verb table entry. A global `ALPHA = 0.95` is a placeholder
from an early prototype. Replace it everywhere.

**Do not store raw pointers in CBOR.** It will produce silent corruption on
reload. Use integer IDs. Resolve pointers in the second load pass.

**Do not build the five output projections as a pipeline.** The ODE solver does
not call the NLG engine. The emergy module does not call the ODE solver. All five
projections read from the same Ordinal Matrix object. If you find yourself passing
output from one module as input to another, you have built a pipeline, not a
projection system.

**Do not skip the drift test.** `make test-drift` is not optional. If Tier 2
mathematics do not reproduce Mercury's precession at 42.45 ± 0.5 sec/cy, the
Tier 2 implementation is wrong regardless of what other tests say.

**Do not generate language about unresolved concepts.** The zero-hallucination
invariant (Section 5.4) must be checked at the entry point of every NLG function.
A concept that is not in the CBOR brain with a resolved ordinal_depth is a
Frontier Node. Say so.

---

## 10. Commit Message Convention

```
<type>(#<issue>): <short description>

<body: what was done and why — reference FRD section if relevant>

Tests: <test file and key assertions>
Valgrind: clean / N errors (if not clean, do not merge)
```

Types: `feat` | `fix` | `test` | `refactor` | `docs` | `chore`

Examples:
```
feat(#3): implement compute_quality_tier1() with per-alpha GL loop

Implements Grünwald-Letnikov weight recurrence and ICE state evolution
for Tier 1 cardinal quality scoring. Alpha is passed per-call, not global.
See FRD Section 2.4 for mathematical specification.

Tests: tests/test_ice_math.c — 6 assertions, all green
Valgrind: clean
```

```
test(#13): add Mercury precession benchmark — make test-drift

Implements test_mercury_precession_idc() using Giannantoni (2006)
orbital parameters. Target: 42.45 ± 0.5 sec/cy.
Currently FAILING — Tier 2 not yet implemented. Expected.

Tests: tests/test_drift.c — 1 assertion, currently red (expected)
Valgrind: N/A
```

---

## 11. Definition of Done

A git issue is done when all of the following are true:

- [ ] All tests for the module pass (`make test-unit` green)
- [ ] `make test-drift` passes if any Tier 2 code was touched
- [ ] `make valgrind` reports zero errors and zero leaks
- [ ] `make lint` reports zero warnings
- [ ] All functions have block comments per Section 5.1
- [ ] No raw pointers in any CBOR serialisation path
- [ ] No global alpha constant anywhere in the module
- [ ] The FRD is consistent with the implementation (update FRD if needed)
- [ ] Commit message follows Section 10 convention

---

## 12. Reference Documents

All theory is in `docs/`. Read the relevant paper before implementing any
mathematical function — do not implement from memory or from this AGENTS.md alone.

| Document | Relevant to |
|---|---|
| `docs/FRD.md` | Everything — primary specification |
| `docs/theory/giannantoni_2006.pdf` | ICE math, Tier 1 GL weights, Tier 2 drift correction, Mercury benchmark |
| `docs/theory/giannantoni_2010.pdf` | Maximum Ordinality Principle, binary/duet/binary-duet functions |
| `docs/theory/giannantoni_2011.pdf` | Ordinal Matrix specularity, O(N-1) reduction, bioinformatics application |
| `docs/theory/odum_1983_fig634.png` | Thermodynamic Grammar, NLG sentence structure, verb-to-energy mapping |