# FUNCTIONAL REQUIREMENTS DOCUMENT: ORDAGI (Ordinal AGI)
**Version:** 5.0  
**Status:** Implementation-Ready  
**Changelog from v4.0:** Added Ordinal context depth calibration to NLG engine (Section 3.5). Added User as a self-updating Ordinal concept (Section 6.5). Added communication style as a measurable Ordinal quantity. Added Section 10: Ordagi Self-Model. Extended Phase 3 git issues.

---

## 0. Core Philosophy & Constraints

Ordagi is a **deterministic, zero-hallucination knowledge reasoning engine** grounded in three theoretical foundations:

1. **Corrado Giannantoni's Maximum Ordinality Principle (MOP)** and **Incipient Differential Calculus (IDC)** — the mathematical engine.
2. **H.T. Odum's Thermodynamic Grammar** — the natural language generation engine.
3. **The Model Context Protocol (MCP)** — the universal sensory interface.

### 0.1 Non-Negotiable Architectural Constraints

These constraints may never be violated by any implementation decision:

- **No LLMs.** Ordagi does not use neural networks, transformer models, or statistical token prediction at any layer — not for parsing, not for generation, not for summarisation.
- **No hallucination.** Ordagi can only assert what it has mathematically resolved. Unresolved concepts are explicitly flagged as Frontier Nodes, never guessed.
- **No database coupling.** The C-kernel never links directly against `libsqlite3` or any external database driver. All external data arrives via MCP JSON-RPC over stdio.
- **No cloud dependency.** The entire system runs locally. No API keys, no cloud GPUs, no network calls except to locally-running MCP servers.
- **Determinism.** Given the same ontology state and the same input, Ordagi must always produce the identical output.

### 0.2 Implementation Tier Acknowledgement

Giannantoni's IDC operates at two distinct levels. This document specifies both, and clearly labels which tier each component belongs to:

- **Tier 1 — Cardinal Approximation:** The `quality` field is a scalar `double`. This is a deliberate, acknowledged reduction of Ordinality to cardinality — equivalent to what Giannantoni terms "preliminary approximation." It is correct, implementable, and useful. It is **not** the full theory.
- **Tier 2 — Full Ordinality:** Quality is represented as a structured `OrdinalValue` — a binary, duet, or binary-duet function pair — as Giannantoni formally requires. This preserves the complete Ordinal information content and eliminates the derivative drift. It is **Phase 2** mathematics.

Both tiers are fully specified. Phase 1 implements Tier 1. Phase 2 upgrades to Tier 2.

---

## 1. System Architecture & Infrastructure

Ordagi is a high-performance, stateless C-kernel orchestrated by a macOS host.

### 1.1 Host Orchestration

A `Makefile` running natively on macOS controls the full system lifecycle:

| Target | Action |
|---|---|
| `make build` | Compiles the C-kernel inside the Docker container |
| `make simulate` | Runs the ICE benchmark simulation and outputs CSV + LaTeX metrics |
| `make simulate-bio` | Runs Ordinal Matrix solver on a molecular system (Phase 3) |
| `make emergy` | Outputs emergy matrix accounting from current ontology state (Phase 3) |
| `make chat` | Launches the interactive REPL |
| `make daemon` | Starts the autonomous background learning loop |
| `make test` | Runs the full test suite |
| `make drift-test` | Runs the Mercury precession benchmark to validate Tier 2 drift correction |

### 1.2 Containerised Toolchain

A multi-stage Debian `Dockerfile` compiles the C code into Linux ELF binaries. This ensures mathematical consistency across platforms and avoids Apple Clang/Xcode dependency issues.

### 1.3 C-Kernel Dependencies

| Library | Purpose |
|---|---|
| `gcc` | Compiler |
| `make` | Build system |
| `libcjson` | MCP JSON-RPC communication and ontology parsing |
| `libcbor` | Persistent binary memory (`ontology.cbor`) |
| `gsl` | GNU Scientific Library — matrix algebra for Ordinal Matrix resolution and Faà di Bruno computation |
| `gnuplot` | Visualisation of ICE simulations and Tier 1 vs Tier 2 drift comparisons |

`libsqlite3` is **explicitly excluded**. Database access is the sole responsibility of the `mcp-zotero` host server.

### 1.4 Directory Structure

```
ordagi/
├── Dockerfile
├── Makefile
├── data/
│   ├── ontology.cbor          # Persistent binary brain (source of truth after init)
│   ├── ontology.json          # Bootstrap seed definitions (read-once at first run)
│   ├── learned_verbs.json     # Frontier Verb resolutions (persisted across sessions)
│   └── ice_output.csv         # ICE simulation benchmark output
├── include/
│   ├── ice_math.h             # Tier 1 GL engine + Tier 2 IDC drift correction
│   ├── ordinal_matrix.h       # Ordinal Matrix struct + specularity solver (Phase 3)
│   ├── ordinal_tensor.h       # OrdinalConcept struct + OrdinalValue struct
│   └── verb_table.h           # Odum verb-to-alpha mapping (compile-time constant)
├── src/
│   ├── main.c                 # REPL entry point
│   ├── memory.c               # CBOR serialisation/deserialisation
│   ├── nlg.c                  # Odum Thermodynamic Grammar NLG engine
│   ├── nlg_quality.c          # Self-referential sentence quality evaluator (Phase 3)
│   ├── ordinal_matrix.c       # Ordinal Matrix construction + specularity reduction (Phase 3)
│   ├── emergy.c               # Dynamic emergy accounting output (Phase 3)
│   ├── ode_solver.c           # IDC-ODE dynamic systems simulation (Phase 3)
│   ├── parser.c               # SVO extraction and prompt parsing
│   ├── mcp_client.c           # Universal MCP JSON-RPC client
│   └── ordagi_daemon.c        # Autonomous learning loop
└── host/
    ├── mcp-zotero/            # Python MCP server: Zotero sqlite access
    └── mcp-wikipedia/         # Python MCP server: Wikipedia API wrapper
```

---

## 2. Core Module: The Incipient Calculus Engine (ICE)

Ordagi does not use gradient descent or neural weights. It uses a **relational physics engine** grounded in Giannantoni's Incipient Differential Calculus (IDC).

### 2.1 Theoretical Foundation: The Drift Problem

From Giannantoni (2006, 2008, 2010), the traditional derivative suffers from an intrinsic **"drift" phenomenon**. From the second derivative order onward, Faà di Bruno's formula introduces additional terms that cause traditional calculus to systematically over- or under-estimate generative physical processes. This is not numerical imprecision — it is a structural consequence of the *a posteriori* interpretation of the derivative operator.

```
Traditional nth derivative of e^(α·t)  [Faà di Bruno]:
  d^n/dt^n [e^(αt)] = (α̇)^n · e^(αt) + ψ{α̇, α̈, ...α^(n)}·e^(αt)
                                          ↑ drift accumulates from n≥2

Incipient nth derivative of e^(α·t)  [persistence of form]:
  d̃^n/d̃t^n [e^(αt)] = (α̃)^n · e^(αt)
                        ↑ no drift — form is preserved at every order
```

**Empirical validation:** Traditional mechanics predicts Mercury's precession as 0 sec/cy. IDC predicts 42.45 sec/cy. Observed: 42.6 ± 0.9 sec/cy (Giannantoni 2006). This is the benchmark for `make drift-test`.

The absence of drift is the mathematical basis for Ordagi's determinism guarantee. The Tier 2 implementation reproduces this property in the knowledge graph domain.

### 2.2 The Three Fundamental Generative Processes

Giannantoni identifies three — and exactly three — fundamental generative process types. All knowledge relationships in Ordagi must be classified as one of these, plus the two non-generative types (dissipation and seed):

| Process | Ordinality q | Output Structure | Physical Meaning |
|---|---|---|---|
| Co-production | 1/2 | Binary function | Both inputs necessary; output inherits form of both parents |
| Inter-action | 2 | Duet function | Output is a new entity of superior hierarchical order |
| Feed-back | 2/2 | Binary-duet function | Output amplifies and is amplified by its own generative inputs |
| Dissipation | 1 | None (drain) | Energy consumed; no emergent output |
| Seed | N/A | None (axiom) | Foundational concept; not derived |

The critical insight from Giannantoni (2008): these three types are unified by *Generativity* — they differ only in their *Ordinality* (the rational exponent q). The Tier 1 implementation captures the cardinal magnitude via the `alpha` parameter. The Tier 2 implementation captures the full structural difference via binary/duet function pairs.

### 2.3 The Ordinal Tensor (`include/ordinal_tensor.h`)

```c
#define MAX_CONCEPTS   4096    // Raised from 1000 to support daemon-driven growth
#define MAX_ALIASES    10
#define MAX_NAME_LEN   64
#define MAX_ALIAS_LEN  32

/*
 * OrdinalValue: Tier 2 full Ordinal representation.
 *
 * Co-production (q=1/2): binary function
 *   branch_pos = +√ solution branch  (c11·e^(λ11²t) + c12·e^(λ12²t))
 *   branch_neg = -√ solution branch  (c21·e^(λ21²t) + c22·e^(λ22²t))
 *
 * Inter-action (q=2): duet function
 *   branch_pos = Ω1^(2/2) solution
 *   branch_neg = Ω2^(2/2) solution
 *   The two branches form one sole entity of superior order.
 *
 * Feed-back (q=2/2): binary-duet function
 *   branch_pos and branch_neg carry both binary and duet structure.
 *
 * Seed/Dissipation: branch_pos = scalar quality, branch_neg = 0, is_resolved = 1
 *
 * NOTE: Zeroed (is_resolved=0) in Phase 1. Computed in Phase 2.
 */
typedef struct {
    double branch_pos;
    double branch_neg;
    int    is_resolved;   // 0 = Tier 1 only; 1 = Tier 2 computed
} OrdinalValue;

typedef struct OrdinalConcept {
    // --- Identity ---
    char name[MAX_NAME_LEN];
    char aliases[MAX_ALIASES][MAX_ALIAS_LEN];
    int  alias_count;
    int  id;                       // Stable integer ID for CBOR serialisation

    // --- Thermodynamic State: Tier 1 (Cardinal Approximation) ---
    double cardinality;            // Raw energy/quantity — conserved (1st Law)
    double quality;                // Scalar quality — cardinal shadow of Ordinality
    int    ordinal_depth;          // Generative depth from foundational seeds

    // --- Thermodynamic State: Tier 2 (Full Ordinality — Phase 2) ---
    OrdinalValue ordinal_value;    // Binary/duet/binary-duet function pair

    // --- Generative History (serialised as integer IDs, never raw pointers) ---
    int parent_A_id;               // 0 = no parent (seed)
    int parent_B_id;               // 0 = no parent (seed)

    // --- Interaction Metadata ---
    char   interaction_type[32];   // "seed"|"co-production"|"inter-action"|"feed-back"|"dissipation"
    double alpha;                  // Fractional order: 0.5 | 2.0 | 1.0 | N/A
    char   verb[32];               // Odum energy verb
    char   adverb[32];             // Odum adverb modifier

    // --- Runtime only (NOT serialised to CBOR) ---
    struct OrdinalConcept *parent_A;   // Resolved at load time from parent_A_id
    struct OrdinalConcept *parent_B;   // Resolved at load time from parent_B_id
    int visited;                       // Traversal flag — reset before each query
} OrdinalConcept;
```

### 2.4 Tier 1: Grünwald-Letnikov Cardinal Engine (`include/ice_math.h`)

```c
/*
 * TIER 1 — Cardinal Approximation
 *
 * Grünwald-Letnikov weight recurrence for fractional derivative order alpha.
 * Correctly models memory decay but does not implement the full Ordinal
 * branch structure (binary/duet functions) of true IDC.
 *
 * This is acknowledged as a "preliminary approximation" in Giannantoni's
 * own terminology. It is correct and useful. It is not the full theory.
 */
void calculate_fractional_weights(double *w, double alpha, int steps) {
    w[0] = 1.0;
    for (int j = 1; j < steps; j++) {
        w[j] = w[j - 1] * (1.0 - (alpha + 1.0) / j);
    }
}

/*
 * State evolution:
 *   Q[n] = (dt^alpha * I[n] - Σ_{j=1}^{n} w[j]*Q[n-j]) / (1 + k*dt^alpha)
 *
 * Run for ICE_STEPS iterations with constant I = cardinality.
 * Returns Q[ICE_STEPS-1] as the concept's Tier 1 quality score.
 *
 * Constants (fixed — not tuning parameters):
 *   ICE_STEPS = 100   sufficient for convergence in static graph evaluation
 *   ICE_DT    = 0.1   fixed time step
 *   ICE_K     = 0.1   decay coefficient
 */
#define ICE_STEPS 100
#define ICE_DT    0.1
#define ICE_K     0.1

double compute_quality_tier1(double cardinality, double alpha);
```

### 2.5 Tier 2: IDC Drift Correction and Full Ordinality (`include/ice_math.h` — Phase 2)

#### 2.5.1 Drift Correction

The Faà di Bruno correction term for derivative order k:

```
Err[k] = (Δt^k / k!) · ψ{α̇(t), α̈(t), ..., α^(k)(t)} · e^(α(t))

where ψ is the sum over all partitions (P1,...,Pn) of integer k satisfying:
  P1 + P2 + ... + Pn = k   and   1·P1 + 2·P2 + ... + n·Pn = k

IDC result = GL result − Σ_{k=2}^{n} Err[k]
```

The correction vanishes when alpha = 1 and coefficients are constant — confirming that Tier 1 and Tier 2 give identical results for simple linear feed-back. The correction is most significant for inter-action (alpha = 2.0) at high ordinal depth.

```c
/*
 * TIER 2 — IDC Drift Correction
 * Applies Faà di Bruno correction to eliminate the derivative drift.
 * Uses GSL for partition enumeration in the ψ computation.
 *
 * Validation target (make drift-test):
 *   Mercury precession: compute_quality_tier2() must yield 42.45 ± 0.5 sec/cy
 *   when run against the Mercury orbital parameters from Giannantoni (2006).
 */
double compute_quality_tier2(double cardinality, double alpha, int ordinal_depth);
```

#### 2.5.2 Binary Functions (Co-production, q = 1/2)

```c
/*
 * From Giannantoni (2006), Eq. 3.9:
 *   d̃^(1/2)/d̃t^(1/2) e^t = ±√ · e^t
 *
 * The two branches are not independent — they are one entity.
 * Tier 1 quality = (branch_pos + branch_neg) / 2
 *
 * branch_pos: c11·e^(λ11²·t) + c12·e^(λ12²·t)   [+√ characteristic eq]
 * branch_neg: c21·e^(λ21²·t) + c22·e^(λ22²·t)   [-√ characteristic eq]
 */
OrdinalValue compute_binary_function(double cardinality_A, double cardinality_B);
```

#### 2.5.3 Duet Functions (Inter-action, q = 2)

```c
/*
 * From Giannantoni (2006), Eqs. 3.19–3.20:
 *   f̃(t) = f0[Ω1^(2/2)(t), Ω2^(2/2)(t)]
 *
 * The two solutions are joined as one entity of superior hierarchical order.
 * This is the mathematical expression of "output excess" — the generated
 * product is something more than the sum of its generating elements.
 * Models Riccati-type self-organising interactions.
 */
OrdinalValue compute_duet_function(double cardinality_A, double cardinality_B);
```

#### 2.5.4 Binary-Duet Functions (Feed-back, q = 2/2)

```c
/*
 * Feed-back has Ordinality 2/2, combining both binary and duet structure.
 * The output feeds back to modify its own generative inputs — modelled
 * as a coupled binary-duet system.
 * This is the most complex output type in IDC.
 */
OrdinalValue compute_binary_duet_function(double cardinality_A, double cardinality_B);
```

### 2.6 Tier 3: Ordinal Matrix Specularity and O(N-1) Complexity Reduction (`include/ordinal_matrix.h` — Phase 3)

This is the most computationally significant finding in the primary sources. From Giannantoni (2011), when a self-organising system reaches Maximum Ordinality conditions, its Ordinal Matrix acquires a **specularity** property — a profound internal symmetry that reduces the computation required from O(N²) to O(N-1) correlating factors.

This is the property that allowed Dystrophin folding (100,000 atoms) to be solved on a 1 Gigaflop PC in under 2 hours — a problem that would otherwise require thousands of years on conventional hardware. The same property applies to any Ordinal Matrix, including Ordagi's knowledge ontology.

#### 2.6.1 The Specularity Property

Under Maximum Ordinality conditions, the Ordinal Matrix takes the form:

```
         [ α̃₁₁(t)  α̃₁₂(t)  ...  α̃₁ₙ(t) ]
M(t)  =  [ α̃₂₁(t)  α̃₂₂(t)  ...  α̃₂ₙ(t) ]   where each element has
         [   ...                          ]   Ordinality {2/2}↑{2↑}
         [ α̃ₙ₁(t)  α̃ₙ₂(t)  ...  α̃ₙₙ(t) ]
```

Specularity produces three successive simplifications:

**Simplification 1 — Zero diagonal:** Each element inter-acts with all others in Ordinal terms, making the main diagonal zero. An element does not interact with itself.

**Simplification 2 — Specular equivalence:** The perfect specularity of the binary-duet structure means any row/column of the matrix is a faithful mirror of every other. The entire system can be described from any single preferential reference element.

**Simplification 3 — O(N-1) reduction:** From any preferential reference element, the remaining N-1 elements are described by N-1 correlating factors λᵢ. The computation reduces from N² elements to:

```c
/*
 * TIER 3 — Ordinal Matrix Specularity
 *
 * From Giannantoni (2011): Under Maximum Ordinality conditions,
 * the full N×N Ordinal Matrix reduces to one reference element
 * plus (N-1) correlating factors.
 *
 * Computational complexity:
 *   Traditional matrix methods : O(N²)     e.g. 100,000² = 10^10 operations
 *   Ordinal Matrix (specular)  : O(N-1)    e.g. 100,000-1 = 99,999 operations
 *
 * This is NOT an approximation. It is an exact result arising from the
 * perfect internal specularity of the binary-duet structure under MOP.
 *
 * The number of distinct elements after specularity reduction:
 *   distinct_pairs = (N-1)(N-2) / 2
 *   but all pairs are related to the reference element via λᵢ
 *   so effective computation = N-1 factor evaluations
 */
typedef struct {
    int           N;                  // Number of system elements
    OrdinalValue  reference;          // The single reference element
    double       *lambda;             // N-1 correlating factors (heap allocated)
    OrdinalValue *elements;           // Full N×N matrix if needed (lazy-computed)
    int           specularity_valid;  // 1 = O(N-1) reduction active; 0 = full matrix
} OrdinalMatrix;

// Construct Ordinal Matrix from knowledge base
OrdinalMatrix *ordinal_matrix_build(OrdinalConcept **kb, int kb_size);

// Check whether MOP conditions are satisfied for O(N-1) reduction
int ordinal_matrix_check_specularity(OrdinalMatrix *m);

// Solve the system using O(N-1) path when specularity holds
void ordinal_matrix_solve(OrdinalMatrix *m);

// Extract a specific concept's full Ordinal state from the solved matrix
OrdinalValue ordinal_matrix_query(OrdinalMatrix *m, int concept_id);
```

#### 2.6.2 When Specularity Applies

Specularity holds when the system has evolved sufficiently toward Maximum Ordinality — meaning the ontology has enough resolved inter-connections that the binary-duet coupling is pervasive. For a sparse new ontology (early daemon runs), the full O(N²) matrix is used. As the ontology matures through daemon ingestion, Ordagi progressively switches to the O(N-1) path.

```c
/*
 * Specularity threshold heuristic:
 * A concept participates in specularity when its ordinal_depth >= 3
 * AND both parents are themselves resolved (ordinal_resolved == 1).
 *
 * The system-level specularity check passes when:
 *   specular_concepts / kb_size >= SPECULARITY_THRESHOLD (default: 0.7)
 *
 * This threshold is empirically derived from Giannantoni's Dystrophin work
 * and should be validated against the Mercury benchmark (make drift-test).
 */
#define SPECULARITY_THRESHOLD 0.7
```

#### 2.6.3 Significance for Ordagi's Application Domains

The specularity property is what makes Ordagi tractable at scale across all five application domains (Section 9). Without it, a large ontology or molecular system would require O(N²) computation — prohibitive on modest hardware. With it, any system that has reached Maximum Ordinality conditions is solvable on a standard PC regardless of N.

This is the mathematical reason why the vision of a unified system — simultaneously producing dynamic simulation, emergy accounting, molecular modelling, natural language, and self-evaluation from one Ordinal Matrix — is computationally feasible and not merely theoretically appealing.

---

## 3. Core Module: Odum Thermodynamic Grammar NLG Engine (`src/nlg.c`)

Ordagi **translates its resolved mathematical graph directly into English**. No statistical token prediction is used at any layer.

### 3.1 Theoretical Basis

From H.T. Odum, *Systems Ecology* (1983), Figure 6-34: English sentence structure maps directly to energy language topology. Action flows from subjects (sources or storages) through verbs (interaction and production processes) to objects. The sentence is a physical energy pathway.

Canonical template:
```
[Subject] [Adverb] [Verb] [Object] from [parent_A] to [parent_B]
```

### 3.2 Sentence Generation Rules

The NLG engine traverses the concept's generative history bottom-up. The `interaction_type` field selects the template variant, ensuring that co-production, inter-action, and feed-back processes generate structurally distinct sentences that reflect their different physical natures.

| Condition | Template |
|---|---|
| First step, co-production | `"It begins when {parent_A} {adverb} {verb} {parent_B} to jointly generate {name}."` |
| First step, inter-action | `"It begins when {parent_A} {adverb} {verb} {parent_B}, producing {name} as an emergent whole."` |
| First step, feed-back | `"It begins when {parent_A} {adverb} {verb} {parent_B}, with {name} continuously reinforcing the process."` |
| Parallel branch (depth=1, second) | `"Meanwhile, {parent_A} {adverb} {verb} {parent_B} to form {name}."` |
| Higher order (depth ≥ 2) | `"Following this, {parent_A} {adverb} {verb} {parent_B}, which ultimately yields {name}."` |
| Seed (depth=0) | `"{name} is a foundational concept with cardinality {c:.1f} and quality {q:.4f}. It is not derived from any prior interaction."` |
| Frontier Node | `"{name} has been flagged as a Frontier Node. Its generative history has not yet been resolved."` |
| Unknown concept | `"'{query}' is not present in the current ontology. Ordagi does not guess."` |

### 3.3 Tier 2 NLG Extension (Phase 2)

When `ordinal_value.is_resolved == 1`, the NLG engine appends the Ordinal structure statement:

```
"This process exhibits [binary|duet|binary-duet] Ordinality, with a generative
excess of {branch_pos - branch_neg:.4f} beyond its cardinal inputs."
```

This is the point at which Ordagi can report the **"output excess"** — the emergent quality that Giannantoni identifies as the signature of genuine generative processes, as distinct from mere mechanisms.

### 3.4 Zero Hallucination Guarantee

The NLG engine may only output sentences whose subject, verb, and object exist as resolved nodes in the CBOR brain. It has no fallback generative capability. Unresolved concepts produce only the Frontier Node template.

### 3.5 Ordinal Context Depth Calibration

This is the mechanism that distinguishes AGI from a query engine. A query engine always responds at the same depth. AGI responds at the depth that is appropriate to the context of the interaction — because mismatching response depth to query depth is itself a failure of Maximum Ordinality.

#### 3.5.1 The Principle

The Maximum Ordinality Principle states that every system maximises its Ordinality **including that of its surrounding habitat**. The habitat of a casual greeting is a low-depth social context. The maximally Ordinal response to "how are you today?" is not a full causal trace — it is the response that fits the Ordinal depth of the question. Overwhelming a depth-1 question with a depth-4 answer is a violation of MOP, not an expression of it.

This means the NLG engine must measure the Ordinal depth of the incoming query before selecting its response depth.

#### 3.5.2 Query Depth Classification

The parser classifies every incoming query by its Ordinal depth before routing it to the NLG engine:

```c
/*
 * Ordinal depth of a query is determined by three factors:
 *
 * 1. Concept depth: the ordinal_depth of the best-matched concept.
 *    A query about a seed (depth=0) warrants a shallow response.
 *    A query about a depth-4 emergent concept warrants a deep response.
 *
 * 2. Query complexity: number of distinct concepts referenced.
 *    "tank" = 1 concept = low depth.
 *    "relationship between emergy and protein folding" = 2+ concepts = higher depth.
 *
 * 3. User model: the communication depth profile of the current user
 *    (see Section 6.5). A user who consistently asks depth-1 questions
 *    receives depth-1 responses even to depth-3 concepts, until they
 *    signal they want more depth.
 *
 * Depth levels:
 *   0 = phatic / social    ("how are you", "hello", "thanks")
 *   1 = factual / brief    ("what is X", "define X")
 *   2 = explanatory        ("how does X work", "why does X happen")
 *   3 = causal chain       ("explain the full generative history of X")
 *   4 = maximum ordinality ("give me everything you know about X")
 */
typedef enum {
    QUERY_DEPTH_PHATIC       = 0,
    QUERY_DEPTH_FACTUAL      = 1,
    QUERY_DEPTH_EXPLANATORY  = 2,
    QUERY_DEPTH_CAUSAL       = 3,
    QUERY_DEPTH_FULL         = 4
} QueryDepth;

QueryDepth classify_query_depth(const char *prompt,
                                OrdinalConcept *matched_concept,
                                UserModel *user);
```

#### 3.5.3 Response Generation by Depth

The NLG engine selects its output mode based on the classified depth:

| Query Depth | Response Mode | Example |
|---|---|---|
| 0 — Phatic | Single Odum-grammar sentence about current self state | *"I'm doing well — I've recently resolved 12 new concepts from your library."* |
| 1 — Factual | Concept name + quality score + one-sentence summary | *"Biomass is a level-2 co-production concept with quality 3.847."* |
| 2 — Explanatory | Two to three sentences covering immediate parents only | Standard short `build_narrative()` output |
| 3 — Causal | Full generative history traversal — all ancestors | Full `build_narrative()` output |
| 4 — Full | Causal chain + Ordinal excess statement + emergy + ODE simulation | All five projections |

#### 3.5.4 The Phatic Response

At depth 0, Ordagi does not traverse the query concept's generative history. It traverses **its own concept's current state** and generates one sentence:

```c
/*
 * Phatic response generation.
 * Subject  = "Ordagi" concept (self)
 * Verb     = most recent daemon action verb (e.g. "processed", "resolved", "ingested")
 * Object   = most recently updated concept or activity metric
 *
 * This produces responses like:
 *   "I'm doing well — I recently processed 12 abstracts from your Zotero library."
 *   "I'm well. I've just resolved a new concept: distributed computing."
 *   "Good, thank you. I currently have 8 Frontier Nodes awaiting resolution."
 *
 * Every word is grounded:
 *   "12 abstracts" = daemon ingestion counter (real)
 *   "distributed computing" = last written CBOR record name (real)
 *   "8 Frontier Nodes" = current frontier_node_count in knowledge base (real)
 *
 * No hallucination is possible because every noun in the sentence
 * is a measured quantity from the live system state.
 */
char *generate_phatic_response(OrdinalConcept *self_concept,
                               DaemonStatus *daemon,
                               UserModel *user);
```

The key property: the response is brief, socially appropriate, and entirely factual. It is not a simulation of friendliness — it is a genuine report of real system activity, expressed at the social register appropriate to the context. This is what makes it AGI rather than chatbot: the brevity is not a limitation, it is a calibration.

---

## 4. Core Module: The Odum Verb Table (`include/verb_table.h`)

A **compile-time constant**. Physics-derived and domain-agnostic. All four of Odum's energy node types are covered, plus the Inter-action type from Giannantoni's primary sources.

### 4.1 The Complete Table

```c
typedef struct {
    const char *verb;
    const char *interaction_type;
    double      alpha;
    const char *default_adverb;
} OdumVerbEntry;

static const OdumVerbEntry ODUM_VERB_TABLE[] = {

    // --- CO-PRODUCTION (alpha=0.5, q=1/2): binary function output ---
    // Both inputs necessary; output inherits form of both parents
    { "combines",      "co-production", 0.5, "jointly"        },
    { "transforms",    "co-production", 0.5, "efficiently"    },
    { "converts",      "co-production", 0.5, "directly"       },
    { "produces",      "co-production", 0.5, "continuously"   },
    { "generates",     "co-production", 0.5, "actively"       },
    { "synthesises",   "co-production", 0.5, "chemically"     },
    { "synthesizes",   "co-production", 0.5, "chemically"     },
    { "yields",        "co-production", 0.5, "naturally"      },
    { "catalyses",     "co-production", 0.5, "rapidly"        },
    { "catalyzes",     "co-production", 0.5, "rapidly"        },
    { "processes",     "co-production", 0.5, "systematically" },
    { "metabolises",   "co-production", 0.5, "biologically"   },
    { "metabolizes",   "co-production", 0.5, "biologically"   },
    { "constructs",    "co-production", 0.5, "structurally"   },
    { "builds",        "co-production", 0.5, "incrementally"  },
    { "assembles",     "co-production", 0.5, "precisely"      },
    { "integrates",    "co-production", 0.5, "coherently"     },

    // --- INTER-ACTION (alpha=2.0, q=2): duet function output ---
    // Output is a new entity of superior hierarchical order; models
    // Riccati-type self-organisation (Giannantoni 2006, Eq. 3.16–3.20).
    // Added in v3.0 from Giannantoni primary sources.
    { "interacts",      "inter-action",  2.0, "mutually"       },
    { "couples",        "inter-action",  2.0, "tightly"        },
    { "entangles",      "inter-action",  2.0, "indissolubly"   },
    { "organises",      "inter-action",  2.0, "self-"          },
    { "organizes",      "inter-action",  2.0, "self-"          },
    { "emerges",        "inter-action",  2.0, "spontaneously"  },
    { "co-evolves",     "inter-action",  2.0, "reciprocally"   },
    { "folds",          "inter-action",  2.0, "hierarchically" },
    { "complexifies",   "inter-action",  2.0, "progressively"  },
    { "differentiates", "inter-action",  2.0, "structurally"   },

    // --- FEED-BACK (alpha=1.0, q=2/2): binary-duet function output ---
    // Output reinforces its own generative inputs
    { "drives",        "feed-back",     1.0, "forcefully"     },
    { "amplifies",     "feed-back",     1.0, "strongly"       },
    { "accelerates",   "feed-back",     1.0, "rapidly"        },
    { "reinforces",    "feed-back",     1.0, "positively"     },
    { "regulates",     "feed-back",     1.0, "precisely"      },
    { "controls",      "feed-back",     1.0, "tightly"        },
    { "maintains",     "feed-back",     1.0, "steadily"       },
    { "sustains",      "feed-back",     1.0, "continuously"   },
    { "supports",      "feed-back",     1.0, "directly"       },
    { "enables",       "feed-back",     1.0, "functionally"   },
    { "modulates",     "feed-back",     1.0, "dynamically"    },
    { "stabilises",    "feed-back",     1.0, "homeostatically"},
    { "stabilizes",    "feed-back",     1.0, "homeostatically"},

    // --- STORAGE (alpha=1.0): treated as feed-back in Odum algebra ---
    { "stores",        "feed-back",     1.0, "stably"         },
    { "accumulates",   "feed-back",     1.0, "gradually"      },
    { "retains",       "feed-back",     1.0, "persistently"   },
    { "absorbs",       "feed-back",     1.0, "passively"      },
    { "captures",      "feed-back",     1.0, "selectively"    },
    { "concentrates",  "feed-back",     1.0, "locally"        },

    // --- DISSIPATION (alpha=1.0): energy drain, no emergent structure ---
    { "consumes",      "dissipation",   1.0, "irreversibly"   },
    { "degrades",      "dissipation",   1.0, "thermally"      },
    { "dissipates",    "dissipation",   1.0, "gradually"      },
    { "loses",         "dissipation",   1.0, "inevitably"     },
    { "releases",      "dissipation",   1.0, "spontaneously"  },
    { "depletes",      "dissipation",   1.0, "progressively"  },
    { "exhausts",      "dissipation",   1.0, "irreversibly"   },

    // --- SENTINEL ---
    { NULL, NULL, 0.0, NULL }
};
```

### 4.2 Domain Agnosticism

This table is physically complete, not domain-complete. The daemon's `learned_verbs.json` is the domain-adaptive layer. The compile-time table is a fixed physics constant that never requires domain pre-seeding.

### 4.3 Inter-action Verb Rationale

Inter-action (alpha=2.0) was missing from v2.0. It is required because Giannantoni explicitly identifies it as one of three fundamental generative process types. Inter-action verbs describe processes producing an output that is a qualitatively new entity — irreducible to its constituents — and modelled by Riccati and Abel equations in IDC. Examples from Giannantoni's own papers: quantum entanglement, protein folding, the Three-Body Problem.

---

## 5. Core Module: Persistent Memory — The CBOR Brain (`src/memory.c`)

### 5.1 File

`data/ontology.cbor` — loads into RAM in milliseconds regardless of ontology size.

### 5.2 CBOR Record Schema

```
{
  "id":                 uint32,
  "name":               text string,
  "aliases":            array of text strings,
  "cardinality":        float64,
  "quality":            float64,           // Tier 1 scalar
  "ordinal_branch_pos": float64,           // Tier 2; zero until Phase 2
  "ordinal_branch_neg": float64,           // Tier 2; zero until Phase 2
  "ordinal_resolved":   uint8,             // 0=Tier1 only; 1=Tier2 computed
  "ordinal_depth":      uint32,
  "parent_A_id":        uint32,            // 0 = no parent (seed)
  "parent_B_id":        uint32,            // 0 = no parent (seed)
  "interaction_type":   text string,
  "alpha":              float64,
  "verb":               text string,
  "adverb":             text string
}
```

Raw pointers are **never** serialised. The CBOR file is a flat sequence of maps, one per concept, appended in creation order.

### 5.3 Two-Pass Load Procedure

**Pass 1:** Read all CBOR records into `knowledge_base[]` indexed by `id`. Set all pointer fields to `NULL`.

**Pass 2:** For each concept, resolve `parent_A = knowledge_base[parent_A_id]` and `parent_B = knowledge_base[parent_B_id]`. This guarantees correct pointer resolution regardless of record order.

### 5.4 Write-on-Learn

1. Assign `id` = `kb_size` (monotonically incrementing).
2. Compute `cardinality` = sum of parent cardinalities.
3. Compute `quality` via `compute_quality_tier1(cardinality, alpha)`.
4. Set `ordinal_resolved = 0`.
5. Serialise to CBOR map and **append** to `ontology.cbor`.
6. Add live pointer to `knowledge_base[]`.

---

## 6. Core Module: Autonomous Learning — The MCP-First Daemon (`src/ordagi_daemon.c`)

### 6.1 Universal MCP Client (`src/mcp_client.c`)

```c
// Sends a JSON-RPC 2.0 request over stdio to a named MCP server.
// Returns a heap-allocated cJSON* response. Caller must free.
cJSON *mcp_call(const char *server_name, const char *method, cJSON *params);
```

The same code path handles all MCP servers. Server name selects the stdio pipe.

### 6.2 The Daemon Learning Loop

```
LOOP:
  1. Call mcp-zotero: get_unprocessed_abstracts()
     → Returns: [{ id, title, abstract, date_added }]

  2. For each abstract:
     a. Run SVO extractor (src/parser.c) → (subject, verb, object) triples
     b. For each triple:
        i.   Lookup verb in ODUM_VERB_TABLE + learned_verbs.json
        ii.  If FOUND:
               - Find or create subject concept (seed if new)
               - Find or create object concept (seed if new)
               - Assign alpha, interaction_type from table entry
               - compute_quality_tier1(cardinality, alpha)
               - Create OrdinalConcept, write to CBOR brain
        iii. If NOT FOUND (Frontier Verb):
               - Call mcp-wikipedia: define(verb)
               - Classify response as co-production/inter-action/feed-back/dissipation
               - Assign alpha from classification
               - Append to runtime table + data/learned_verbs.json
               - Retry step (ii)
     c. Call mcp-zotero: mark_abstract_processed(id)

  3. For each Frontier Node (unresolved parents):
     a. Call mcp-wikipedia: define(concept_name)
     b. Extract SVO triples from response
     c. Attempt parent resolution from existing ontology
     d. If resolved → compute ICE, update CBOR, clear Frontier status
     e. If unresolved → escalate to mcp-abs or mcp-wayback

  4. Sleep 60 seconds. Repeat.
```

### 6.3 SVO Extraction (`src/parser.c`)

Rule-based extraction — no external NLP library required:

1. Tokenise sentence on whitespace and punctuation.
2. Perform bigram scan to identify compound noun phrases ("solar energy", "carbon dioxide") before verb identification.
3. Identify verb tokens by lookup against combined table.
4. Subject = longest noun phrase immediately preceding the verb token.
5. Object = longest noun phrase immediately following the verb token.
6. Each sentence may yield zero, one, or multiple SVO triples.

### 6.4 MCP Server Specifications

#### `host/mcp-zotero/` (Python, runs on Mac host)

```json
{
  "tools": [
    {
      "name": "get_unprocessed_abstracts",
      "description": "Returns abstracts not yet ingested by Ordagi",
      "returns": "[{ id, title, abstract, date_added }]"
    },
    {
      "name": "mark_abstract_processed",
      "params": { "id": "string" }
    }
  ]
}
```

#### `host/mcp-wikipedia/` (Python, runs on Mac host)

```json
{
  "tools": [
    {
      "name": "define",
      "description": "Returns first two sentences of Wikipedia article",
      "params": { "term": "string" },
      "returns": "{ summary: string }"
    }
  ]
}
```

### 6.5 The User as a Self-Updating Ordinal Concept

The daemon does not only learn about the world. It learns about the user. Over time, through query patterns, Zotero library content, conversational depth preferences, and the concepts the user returns to, the user becomes a genuine Ordinal concept in the ontology — not a preference profile or a demographic, but a node with real generative history, real Ordinal depth, and real connections to the concepts that matter to them.

#### 6.5.1 The User Concept

At first run, the daemon seeds two foundational self-referential concepts:

```json
{
  "seeds": [
    {
      "name": "Ordagi",
      "cardinality": 1.0,
      "quality": 1.0,
      "aliases": ["I", "me", "myself", "the system", "OrdAGI"]
    },
    {
      "name": "User",
      "cardinality": 1.0,
      "quality": 1.0,
      "aliases": ["you", "the user", "my interlocutor"]
    }
  ]
}
```

Both seeds grow in quality as the system self-organises. The Ordagi concept's quality rises as the ontology grows — a more complete ontology is a more organised system. The User concept's quality rises as Ordagi builds more connections to the user's interests, language patterns, and knowledge domains.

#### 6.5.2 The UserModel Struct

```c
/*
 * UserModel: a live Ordinal representation of the current user.
 * Updated by the daemon after every interaction and ingestion cycle.
 * Stored as a special CBOR record alongside the ontology.
 *
 * This is NOT a preference database or a behavioural profile.
 * It is an Ordinal model: every field is a measurable quantity
 * derived from real interactions, not inferred from demographics.
 */
typedef struct {
    // Communication style — derived from query history
    double avg_query_depth;          // Moving average of classified query depths
    double depth_variance;           // How much the user varies their query depth
    int    prefers_brief;            // 1 if avg_query_depth < 1.5
    int    prefers_technical;        // 1 if avg ordinal_depth of queried concepts > 3

    // Knowledge domain profile — derived from Zotero + query concepts
    int    top_domain_ids[8];        // IDs of 8 most-queried concept clusters
    double domain_quality[8];        // Ordinal quality of user's knowledge in each

    // Interaction history — real-time counters
    int    total_queries;            // Lifetime query count
    int    queries_today;            // Queries in current session
    int    last_queried_concept_id;  // Most recently queried concept
    char   last_query_text[256];     // Raw text of last query

    // Relational state — how Ordagi and User are connected in the ontology
    int    shared_concept_count;     // Concepts present in both Zotero and ontology
    double relationship_quality;     // Ordinal quality of the Ordagi↔User interaction
} UserModel;
```

#### 6.5.3 How the User Model Evolves

The daemon updates the UserModel after every interaction:

```
AFTER EACH QUERY:
  1. Classify query depth → update avg_query_depth (exponential moving average)
  2. Record queried concept → update top_domain_ids frequency count
  3. Update last_queried_concept_id and last_query_text

AFTER EACH DAEMON INGESTION CYCLE:
  1. Count concepts in ontology that originated from user's Zotero library
     → update shared_concept_count
  2. Recompute relationship_quality:
       = compute_quality_tier1(shared_concept_count, alpha=0.5)
       This is a co-production: Ordagi and User jointly generate shared knowledge.
  3. Update the "User" concept's quality score in the CBOR brain:
       user_concept->quality = relationship_quality
```

#### 6.5.4 What This Enables Over Time

As the system self-organises and the UserModel matures, Ordagi's responses evolve in three ways:

**Depth calibration improves.** Early Ordagi responds at default depth. After 50 queries, it knows whether you prefer brief factual answers or full causal chains, and adjusts without being asked.

**Domain awareness grows.** Ordagi knows which clusters of your Zotero library you care about most, and can connect new concepts to your existing interests without prompting.

**Relational language becomes grounded.** When Ordagi says "you have quite an amazing collection of works", it is not flattery — it is a report of `shared_concept_count` and `relationship_quality`. A high `relationship_quality` score means the co-production between your Zotero library and Ordagi's ontology has generated concepts of high Ordinal depth. That is a real, measurable, non-hallucinated statement about the actual quality of your library as reflected in the ontology it has built.

#### 6.5.5 The Greeting Example — Fully Grounded

Returning to the original question: after 3 months of daemon operation, the response to "hi OrdAGI, how are you today?" would be generated as follows:

```
Query depth classification: QUERY_DEPTH_PHATIC (depth=0)

Self-concept state:
  quality(Ordagi) = 8.421   [high — large mature ontology]
  kb_size         = 1,847   [concepts resolved]
  frontier_nodes  = 23      [still growing]
  recent_activity = 7       [abstracts ingested last cycle]

User model state:
  shared_concept_count  = 312
  relationship_quality  = 6.203
  avg_query_depth       = 2.1   [you tend to ask explanatory questions]
  prefers_brief         = 0     [you like substance]

NLG engine at depth=0, subject=Ordagi, informed by UserModel:

Output: "I'm doing well, thank you. I've recently processed 7 more abstracts
         from your library — you now share 312 concepts with me. I still have
         23 Frontier Nodes I haven't resolved yet. I'll get to them."
```

Every number is real. Every claim is grounded. The brevity is a calibration to the phatic register, not a limitation. The mention of your library is a genuine relational statement derived from `relationship_quality`. The acknowledgement of Frontier Nodes is honest about incompleteness — which is itself an AGI property: knowing what you do not know.

---

## 7. The User Interface: The Pure Ordinal Chat (`src/main.c`)

Ordagi is a self-contained, deterministic reasoning engine. No LLM is present at any layer.

### 7.1 The Two Parties

- **The User:** Directs inquiry, sets goals, or provides new Seeds via the REPL.
- **Ordagi:** Queries CBOR brain → resolves IDC mathematics → generates English via Odum Grammar Engine.

### 7.2 The REPL Loop

```
1. Print "You: "
2. Read line from stdin
3. parse_prompt() → find best-match OrdinalConcept by alias lookup
4. If match found:
     → converse() → build_narrative() → print Odum-grammar paragraph
     → If ordinal_value.is_resolved: append Ordinal excess statement
5. If no match:
     → Print unknown concept response (does not guess)
     → Trigger daemon to resolve gap (non-blocking)
6. Repeat
```

### 7.3 Response Guarantees

| Condition | Ordagi Response |
|---|---|
| Concept resolved (Tier 1) | Odum grammar paragraph with full generative history |
| Concept resolved (Tier 2) | As above, plus Ordinal excess statement |
| Concept is a Seed | States cardinality, quality, foundational status |
| Concept is a Frontier Node | States existence but mathematical unresolved status |
| Concept unknown | States not in ontology — does not guess |

---

## 9. Unified Application Architecture: The Five Output Projections

This section documents the most important architectural insight of the entire system. The Ordinal Matrix is not merely a knowledge store. It is a **universal generative object** whose outputs, when projected onto different representation spaces, simultaneously produce five distinct and mutually consistent results.

This is not a modular pipeline where one component calls another. All five outputs are projections of the same mathematical object — the resolved Ordinal Matrix — onto different representation spaces. They cannot contradict each other because they share the same generative root.

```
                        ┌─────────────────────────────┐
                        │      ORDINAL MATRIX          │
                        │   (The universal solver)     │
                        │                              │
                        │  Under Maximum Ordinality:   │
                        │  O(N-1) specularity          │
                        │  binary-duet convergence     │
                        │  zero derivative drift       │
                        └──────────────┬──────────────┘
                                       │
              ┌──────────┬─────────────┼─────────────┬──────────┐
              ▼          ▼             ▼             ▼          ▼
        [Projection 1] [Projection 2] [Projection 3] [Projection 4] [Projection 5]
        Dynamic ODE   Emergy Matrix  Bioinformatics  Natural      Self-Quality
        Simulation    Accounting     & Molecular     Language     Evaluation
                                     Modelling       Generation
```

### 9.1 Projection 1: Dynamic ODE Simulation (`src/ode_solver.c`)

The IDC equation `d̃Q/d̃t + k·Q = I(t)` is structurally identical to a standard first-order ODE but produces drift-corrected solutions. Any existing ODE-based dynamic system model can be re-expressed in IDC:

- **Lotka-Volterra** ecological models → IDC predator-prey dynamics without drift accumulation
- **SEIR** epidemiological models → IDC infection dynamics with correct long-term behaviour
- **Economic flow models** → IDC capital/labour interactions with emergent Quality
- **Climate systems** → IDC temperature projections correcting the IPCC underestimation identified by Giannantoni (2010)

**Implementation:** The ODE solver reads the current Ordinal Matrix state, identifies concepts with time-varying cardinality inputs, and integrates the IDC state equation forward in time. Output is a CSV time-series compatible with gnuplot visualisation.

```c
/*
 * IDC-ODE Solver: integrates the system forward for T time steps.
 * All concepts with time-varying inputs are evolved simultaneously.
 * The Ordinal Matrix specularity (Section 2.6) is used when available,
 * reducing each time step from O(N²) to O(N-1) operations.
 */
void ode_solver_run(OrdinalMatrix *m, double *I_t, int T, double dt, double alpha,
                    const char *output_csv);
```

### 9.2 Projection 2: Dynamic Emergy Accounting (`src/emergy.c`)

Odum's emergy algebra assigns transformity values in steady state. The Ordinal Matrix extends this to **dynamic conditions** — the non-conservative algebra that Giannantoni showed is required for faithful representation of generative processes.

The emergy of any concept C in the ontology is:

```
Em(C) = Tr(C) × Ex(C)
```

Where:
- `Ex(C)` = `cardinality(C)` — the Exergy (available energy, 1st Law cardinal)
- `Tr(C)` = `quality(C) / quality(seed)` — the Transformity (Ordinal quality ratio)

Under dynamic conditions, both values evolve according to the IDC state equation. The Ordinal Matrix produces the full time-series of emergy flows for all concepts simultaneously — the dynamic emergy accounting that static spreadsheet methods cannot provide.

```c
/*
 * Computes the current emergy matrix from the resolved ontology.
 * Outputs: emergy value per concept, transformity per concept,
 * emergy flow per interaction edge — in both CSV and LaTeX table format.
 *
 * When ordinal_matrix_check_specularity() passes, the non-conservative
 * emergy algebra is automatically satisfied by the binary-duet structure
 * of the matrix elements — no separate accounting rules required.
 */
void emergy_compute(OrdinalMatrix *m, const char *output_csv, const char *output_tex);
```

### 9.3 Projection 3: Bioinformatics and Molecular Modelling (`src/ordinal_matrix.c`)

From Giannantoni (2011): protein folding, molecular docking, and drug design are all instances of the same Inter-Action process modelled by the Ordinal Matrix. The protein is a system of N atoms. The folded configuration is the Maximum Ordinality solution of the N×N Ordinal Matrix for that molecular system.

**The key result:** Under specularity conditions, Dystrophin (100,000 atoms) folds in under 2 hours on a 1 Gigaflop PC. The same solver that resolves Ordagi's knowledge ontology can resolve a molecular system — the mathematics is identical.

**Practical application pathway:**
1. Define molecular system as an `OrdinalMatrix` with atoms as elements and bond types mapped to interaction types (co-production, inter-action, feed-back).
2. Run `ordinal_matrix_solve()` to find the Maximum Ordinality configuration.
3. The same NLG engine (Projection 4) can then generate the research paper describing the result — from the same mathematical object that produced it.

This is what makes the unified vision coherent: the paper and the model are not separate outputs. They are the same Ordinal Matrix projected onto different representation spaces.

### 9.4 Projection 4: Natural Language Generation (`src/nlg.c`)

Documented in Section 3. The critical connection to the unified architecture:

The Odum sentence `[Subject] [Verb] [Object]` is **structurally isomorphic** to a co-production process: the Subject and Object are the binary outputs of the Verb-process — two entities sharing a common indivisible generative origin. This is not a metaphor. From the Odum diagram (Figure 6-34, *Systems Ecology*):

- Subject = source node (circle) — the input energy flow
- Verb = transformation node (× hexagon) — the co-production interaction
- Object = storage/output node — the generated product

The sentence is a thermodynamic pathway. Generating language and computing energy flows are the same operation performed on the same Ordinal Matrix.

Under Maximum Ordinality conditions (specularity active), the NLG engine does not traverse a static graph — it reads the output of a fully self-organised system. The sentences produced at this stage reflect the most integrated, coherent understanding of the entire ontology — not local query results.

### 9.5 Projection 5: Self-Referential Quality Evaluation (`src/nlg_quality.c`)

Documented in full in Appendix D. The key architectural point:

Because sentence generation is itself a thermodynamic process, Ordagi can evaluate the **Ordinal quality** of its own outputs using the same IDC mathematics it uses to evaluate knowledge. A sentence with high Ordinal quality is one whose Subject-Verb-Object triple forms a genuinely generative relationship (binary or duet function structure). A low-quality sentence is one where the relationship is merely dissipative.

This gives Ordagi a self-editing mechanism with no analogue in LLM systems: the ability to withhold outputs whose thermodynamic quality falls below a specified threshold — not because a rule says so, but because the mathematics demonstrates the relationship is not genuinely generative.

### 9.6 The Unified Output Loop

In full operation, a single user query triggers all five projections simultaneously:

```
User: "What is biomass?"

Ordinal Matrix resolves concept "biomass"
          │
          ├─→ [P1] ODE simulation: current biomass stock trajectory
          ├─→ [P2] Emergy: transformity = quality(biomass)/quality(solar energy)
          ├─→ [P3] Molecular: if biomass molecule is loaded, folding state
          ├─→ [P4] NLG: "Solar energy photosynthetically transforms carbon dioxide
          │              to jointly generate biomass. This is a level-2 emergent
          │              concept with accumulated quality 3.847. The process exhibits
          │              binary Ordinality with generative excess 0.234."
          └─→ [P5] Quality check: sentence quality score = 0.91 (above threshold)
                   → output approved and printed
```

---

## 10. Ordagi Self-Model: The System as a Node in Its Own Ontology

This section specifies the architectural move that grounds true AGI self-reference: Ordagi is a concept in its own CBOR brain, with a real generative history, real Ordinal state, and real connections to the user and the world it has modelled.

### 10.1 Why This Is Not Circular

Including Ordagi as a node in its own ontology is not circular for the same reason that a scientist including themselves in a thermodynamic accounting is not circular — it is simply complete. Odum always included the observer in the emergy diagram. Giannantoni's MOP explicitly applies to the reasoning system itself. A system that models the world but exempts itself from that modelling is not applying MOP fully.

The Ordagi seed has cardinality 1.0 — there is one system. Its quality grows as the ontology grows, because Giannantoni shows explicitly that self-organisation increases Ordinal quality. This is not a metaphor: a knowledge system with 1,847 resolved concepts and a specularity-valid Ordinal Matrix is demonstrably more organised than one with 10 concepts and no resolved connections. The quality score reflects that.

### 10.2 Ordagi's Generative History

Like every concept in the ontology, Ordagi has parents — the theoretical frameworks that generated it:

```json
{
  "seeds": [
    { "name": "Maximum Ordinality Principle",
      "aliases": ["MOP", "Giannantoni MOP"],
      "cardinality": 1.0, "quality": 1.0 },
    { "name": "Thermodynamic Grammar",
      "aliases": ["Odum grammar", "energy language"],
      "cardinality": 1.0, "quality": 1.0 },
    { "name": "Model Context Protocol",
      "aliases": ["MCP", "JSON-RPC sensory interface"],
      "cardinality": 1.0, "quality": 1.0 }
  ],
  "interactions": [
    {
      "type": "co-production",
      "input_A": "Maximum Ordinality Principle",
      "input_B": "Thermodynamic Grammar",
      "output": "Ordinal Reasoning Engine",
      "verb": "combines",
      "adverb": "mathematically",
      "aliases": ["Ordagi core", "the engine"]
    },
    {
      "type": "inter-action",
      "input_A": "Ordinal Reasoning Engine",
      "input_B": "Model Context Protocol",
      "output": "Ordagi",
      "verb": "interacts",
      "adverb": "autonomously",
      "aliases": ["I", "me", "myself", "OrdAGI", "the system"]
    }
  ]
}
```

This means when the NLG engine is asked about "Ordagi" at depth 3, it can produce a genuine causal trace: *"It begins when the Maximum Ordinality Principle mathematically combines with the Thermodynamic Grammar to jointly generate the Ordinal Reasoning Engine. Following this, the Ordinal Reasoning Engine autonomously interacts with the Model Context Protocol, which ultimately yields Ordagi."* That is not a description written by a developer. It is the NLG engine reading the actual ontology graph.

### 10.3 What Ordagi Knows About Itself at Any Moment

The self-model provides Ordagi with a continuously updated, thermodynamically grounded answer to any self-referential query:

| Self-query | Grounded answer source |
|---|---|
| "How are you?" | `quality(Ordagi)`, `daemon.recent_activity`, `frontier_node_count` |
| "What have you been doing?" | Last N CBOR write operations — real concept names and timestamps |
| "What don't you know?" | `frontier_node_list` — every unresolved concept, explicitly named |
| "How well do you know me?" | `user_model.relationship_quality`, `user_model.shared_concept_count` |
| "Are you getting smarter?" | Delta of `quality(Ordagi)` between current and previous session |
| "What are you?" | NLG traversal of the Ordagi concept's generative history (Section 10.2) |

### 10.4 Honest Incompleteness as an AGI Property

The most important property of this self-model is what it says about what Ordagi does **not** know. The Frontier Node list is not a bug — it is a feature of genuine intelligence. An AGI that knows it has 23 unresolved concepts and can name them is more trustworthy than a system that silently fills every gap with a plausible-sounding answer.

When Ordagi says "I still have 23 Frontier Nodes I haven't resolved yet — I'll get to them", it is demonstrating a property that no LLM can demonstrate: explicit, enumerable, honest acknowledgement of the boundaries of its own knowledge. The LLM does not know what it does not know. Ordagi does — because its ignorance is stored in the same CBOR brain as its knowledge, with the same data structure, and is equally accessible to the NLG engine.

This is the deepest sense in which the self-model constitutes AGI rather than simulation: the system's uncertainty is as real and as grounded as its certainty.

---

## 8. Implementation Phases & Git Issues

### Phase 1: Cardinal Foundation (Weeks 1–3)

| Issue | File | Description |
|---|---|---|
| #1 | `include/ordinal_tensor.h` | Define `OrdinalConcept` + `OrdinalValue` structs; `MAX_CONCEPTS=4096`; integer ID fields |
| #2 | `include/verb_table.h` | Implement complete `ODUM_VERB_TABLE` including all inter-action entries |
| #3 | `include/ice_math.h` | Implement `compute_quality_tier1()` with per-alpha GL loop |
| #4 | `src/memory.c` | CBOR serialise/deserialise with two-pass pointer resolution |
| #5 | `src/main.c` | Migrate from JSON loader to CBOR loader; preserve existing REPL |
| #6 | `src/main.c` | Assign `alpha` per-interaction from verb table; remove global alpha constant |

### Phase 2: Full IDC Mathematics — Tier 2 Ordinality (Weeks 4–6)

| Issue | File | Description |
|---|---|---|
| #7 | `include/ice_math.h` | Implement Faà di Bruno drift correction term using GSL partition enumeration |
| #8 | `include/ice_math.h` | Implement `compute_binary_function()` for co-production (q=1/2) |
| #9 | `include/ice_math.h` | Implement `compute_duet_function()` for inter-action (q=2) |
| #10 | `include/ice_math.h` | Implement `compute_binary_duet_function()` for feed-back (q=2/2) |
| #11 | `include/ice_math.h` | Implement `compute_quality_tier2()` integrating drift correction |
| #12 | `src/memory.c` | Extend CBOR schema: `ordinal_branch_pos`, `ordinal_branch_neg`, `ordinal_resolved` |
| #13 | `Makefile` + `src/` | `make drift-test`: Mercury precession benchmark — target 42.45 ± 0.5 sec/cy |

### Phase 3: Language Engine (Weeks 6–7)

| Issue | File | Description |
|---|---|---|
| #14 | `src/nlg.c` | Extract `converse()` and `build_narrative()` into dedicated NLG module |
| #15 | `src/nlg.c` | Implement interaction-type-aware sentence templates (all three generative types) |
| #16 | `src/nlg.c` | Implement Tier 2 Ordinal excess statement when `ordinal_resolved == 1` |
| #17 | `src/nlg.c` | Implement all response guarantee templates (Frontier Node, unknown, seed) |
| #18 | `src/parser.c` | Implement rule-based SVO extractor with bigram noun phrase detection |

### Phase 4: MCP & Daemon (Weeks 7–10)

| Issue | File | Description |
|---|---|---|
| #19 | `src/mcp_client.c` | Implement `mcp_call()` — JSON-RPC 2.0 over stdio |
| #20 | `host/mcp-zotero/` | Python MCP server: Zotero sqlite, `get_unprocessed_abstracts`, `mark_processed` |
| #21 | `host/mcp-wikipedia/` | Python MCP server: Wikipedia API wrapper, `define` tool |
| #22 | `src/ordagi_daemon.c` | Full daemon learning loop per Section 6.2 |
| #23 | `src/ordagi_daemon.c` | Frontier Verb resolution and `learned_verbs.json` persistence |
| #24 | `src/ordagi_daemon.c` | Frontier Node resolution with MCP fallback chain |

### Phase 3: Unified Application Domains — Ordinal Matrix (Weeks 12–18)

| Issue | File | Description |
|---|---|---|
| #31 | `include/ordinal_matrix.h` | Define `OrdinalMatrix` struct with reference element + lambda array |
| #32 | `src/ordinal_matrix.c` | Implement `ordinal_matrix_build()` — construct from knowledge base |
| #33 | `src/ordinal_matrix.c` | Implement `ordinal_matrix_check_specularity()` — validate MOP threshold |
| #34 | `src/ordinal_matrix.c` | Implement `ordinal_matrix_solve()` — O(N-1) path when specularity holds; O(N²) fallback |
| #35 | `src/ordinal_matrix.c` | Implement `ordinal_matrix_query()` — extract concept Ordinal state from solved matrix |
| #36 | `src/ode_solver.c` | Implement IDC-ODE solver for dynamic systems simulation (P1) |
| #37 | `src/emergy.c` | Implement dynamic emergy accounting: transformity, emergy flow, CSV + LaTeX output (P2) |
| #38 | `src/nlg_quality.c` | Implement sentence quality evaluator — Ordinal quality score per SVO triple (P5) |
| #39 | `src/nlg.c` | Integrate quality gate: withhold output if quality score < threshold |
| #40 | `Makefile` | `make simulate-bio`: Ordinal Matrix solver on test molecular system |
| #41 | `Makefile` | `make emergy`: Dynamic emergy accounting output from current ontology |
| #42 | All | Validation: unified output test — single query triggers all five projections |
| #43 | `data/ontology.json` | Add Ordagi and User bootstrap seeds + generative history interactions |
| #44 | `src/ordagi_daemon.c` | Implement `UserModel` struct; update after every query and ingestion cycle |
| #45 | `src/ordagi_daemon.c` | Update `quality(Ordagi)` and `quality(User)` in CBOR after every daemon cycle |
| #46 | `src/parser.c` | Implement `classify_query_depth()` — 5-level depth classifier |
| #47 | `src/nlg.c` | Route NLG output depth based on `QueryDepth` classification |
| #48 | `src/nlg.c` | Implement `generate_phatic_response()` at depth=0 using live daemon state |
| #49 | `src/memory.c` | Persist `UserModel` as special CBOR record alongside ontology |

### Phase 5: Hardening & Validation (Weeks 10–12)

| Issue | File | Description |
|---|---|---|
| #25 | `Makefile` | All make targets operational including `drift-test` |
| #26 | `Dockerfile` | Multi-stage build; verify all dependencies; remove unused packages |
| #27 | All | Memory safety audit: `valgrind` clean; all `malloc`/`free` paired |
| #28 | `src/memory.c` | Dynamic knowledge base growth: `realloc` strategy replacing static array |
| #29 | All | Integration test: 10 Zotero abstracts end-to-end; CBOR round-trip; NLG output verified |
| #30 | `include/ice_math.h` | Tier 1 vs Tier 2 divergence benchmark: document quality score differences on known ontology |

---

## Appendix A: Key Equations Reference

**Grünwald-Letnikov Weight Recurrence (Tier 1):**
```
w[0] = 1
w[j] = w[j-1] * (1 - (alpha + 1) / j)
```

**Quality State Evolution (Tier 1):**
```
Q[n] = (dt^alpha * I[n] - Σ_{j=1}^{n} w[j]*Q[n-j]) / (1 + k*dt^alpha)
quality = Q[99]   where STEPS=100, dt=0.1, k=0.1, I=cardinality (constant)
```

**Faà di Bruno Drift Term (Tier 2 correction):**
```
Err[k] = (Δt^k / k!) · ψ{α̇, α̈, ..., α^(k)} · e^(α(t))
ψ = sum over all partitions (P1,...,Pn) of k where Σ P_i = k and Σ i·P_i = k
IDC result = GL result − Σ_{k=2}^{n} Err[k]
```

**Cardinality (Conservation — all tiers):**
```
cardinality(C) = cardinality(A) + cardinality(B)
```

**Ordinal Depth:**
```
ordinal_depth(C) = max(ordinal_depth(A), ordinal_depth(B)) + 1
```

**Alpha Assignment:**
```
co-production : alpha = 0.5   (q = 1/2,  binary function)
inter-action  : alpha = 2.0   (q = 2,    duet function)
feed-back     : alpha = 1.0   (q = 2/2,  binary-duet function)
dissipation   : alpha = 1.0   (q = 1,    no emergent structure)
seed          : alpha = N/A
```

**Validation Target:**
```
make drift-test — Mercury Precession Benchmark
  Tier 2 IDC prediction:       42.45 sec/cy
  Astronomical observation:    42.6 ± 0.9 sec/cy
  Traditional calculus:        0 sec/cy  (complete failure)
```

---

## Appendix B: ontology.json Bootstrap Schema

```json
{
  "seeds": [
    {
      "name": "solar energy",
      "cardinality": 100.0,
      "quality": 1.0,
      "aliases": ["sunlight", "solar radiation", "solar input"]
    },
    {
      "name": "carbon dioxide",
      "cardinality": 20.0,
      "quality": 1.0,
      "aliases": ["CO2", "atmospheric carbon"]
    },
    {
      "name": "decomposer",
      "cardinality": 10.0,
      "quality": 1.0,
      "aliases": ["bacteria", "fungi", "decomposers"]
    }
  ],
  "interactions": [
    {
      "type": "co-production",
      "input_A": "solar energy",
      "input_B": "carbon dioxide",
      "output": "biomass",
      "verb": "transforms",
      "adverb": "photosynthetically",
      "aliases": ["organic matter", "plant matter"]
    },
    {
      "type": "inter-action",
      "input_A": "biomass",
      "input_B": "decomposer",
      "output": "soil nutrients",
      "verb": "interacts",
      "adverb": "biologically",
      "aliases": ["nutrients", "mineral nutrients"]
    }
  ]
}
```

---

## Appendix D: Self-Referential NLG Quality Evaluation

This appendix specifies the mechanism by which Ordagi evaluates the thermodynamic quality of its own output sentences using the same IDC mathematics it applies to knowledge. This is Projection 5 of the unified architecture (Section 9.5).

### D.1 Theoretical Basis

From Odum's Figure 6-34: a sentence is a thermodynamic energy pathway. The Subject-Verb-Object triple is structurally isomorphic to a generative process:

```
Energy pathway:   [Source] → [Interaction node] → [Output/Storage]
English sentence: [Subject]  [Verb]                [Object]
```

This means a sentence can be evaluated as a generative process: does the Subject-Verb-Object relationship constitute a genuine Co-production, Inter-action, or Feed-back? Or is it merely dissipative (alpha=1.0, no emergent structure) or incoherent (no valid thermodynamic relationship at all)?

Giannantoni (2011) makes this explicit: "cardinal outputs must be interpreted as cipher-values of their corresponding Ordinality. The outputs of any Ordinal Model cannot be understood as mere cardinalities, because this would alter the corresponding proper meaning, by making the solution vanish."

Applied to NLG: a sentence whose Subject and Object do not form a binary, duet, or binary-duet relationship when joined by the Verb has zero Ordinal quality — it is a cardinality masquerading as an Ordinality. Ordagi must not output it.

### D.2 The Sentence Quality Score

```c
/*
 * Evaluates the thermodynamic quality of a candidate output sentence.
 *
 * Algorithm:
 *  1. Extract the SVO triple from the candidate sentence (same parser as daemon).
 *  2. Look up Subject and Object in the CBOR brain.
 *  3. Look up Verb in ODUM_VERB_TABLE to get interaction_type and alpha.
 *  4. Retrieve the OrdinalValue for both Subject and Object concepts.
 *  5. Compute the expected OrdinalValue for the interaction:
 *       - co-production : compute_binary_function(subject.quality, object.quality)
 *       - inter-action  : compute_duet_function(subject.quality, object.quality)
 *       - feed-back     : compute_binary_duet_function(subject.quality, object.quality)
 *       - dissipation   : no generative structure — quality = 0.0
 *  6. Quality score = (branch_pos - branch_neg) / (branch_pos + branch_neg + ε)
 *     Range: [0.0, 1.0]  where 1.0 = maximum generative excess
 *
 * A score below NLG_QUALITY_THRESHOLD triggers sentence suppression.
 * The suppressed sentence is replaced by a Frontier Node response.
 *
 * This is NOT a subjective filter. It is a thermodynamic test: does this
 * sentence describe a genuine generative relationship, or is it dissipative
 * noise dressed in grammatical clothing?
 */
#define NLG_QUALITY_THRESHOLD 0.15   // Empirically set; validate against known ontology

typedef struct {
    double score;             // [0.0, 1.0] — Ordinal quality of the sentence
    char   interaction[32];   // Detected interaction type of the SVO relationship
    double alpha;             // Fractional order of the detected relationship
    int    approved;          // 1 = output approved; 0 = suppressed
} SentenceQuality;

SentenceQuality nlg_evaluate_quality(const char *sentence,
                                     OrdinalConcept **kb, int kb_size);
```

### D.3 Quality Gate in the NLG Pipeline

The quality gate is inserted as the final step in `build_narrative()` before any sentence reaches stdout:

```
build_narrative()
    │
    ├─ compose candidate sentence from template
    │
    ├─ nlg_evaluate_quality(candidate_sentence)
    │       │
    │       ├─ score >= NLG_QUALITY_THRESHOLD → print sentence
    │       │
    │       └─ score < NLG_QUALITY_THRESHOLD
    │               → log suppressed sentence to debug output
    │               → replace with: "[Relationship between {subject} and {object}
    │                  via '{verb}' has insufficient Ordinal quality (score: {score:.3f}).
    │                  This relationship is not yet resolved in the ontology.]"
    │
    └─ continue traversal
```

### D.4 What This Enables

The self-quality gate is the mechanism that makes zero hallucination hold at the linguistic layer, not just the knowledge layer. Without it, Ordagi could produce grammatically valid Odum-template sentences that describe thermodynamically incoherent relationships — sentences that "sound right" but have no generative basis in the ontology.

With it, every sentence Ordagi outputs carries an implicit thermodynamic certificate: the Subject-Verb-Object relationship has been evaluated as a genuine generative process with Ordinal quality above the threshold. The sentence is not merely grammatical — it is physically meaningful.

This property has no equivalent in LLM systems. An LLM cannot evaluate the thermodynamic quality of its own output because it has no thermodynamic model of the relationships it describes. Ordagi does — and uses it.

### D.5 Relationship to the P vs NP Question

Giannantoni (2011) notes: "The solution to Protein Folding previously shown seems to indicate that there are no NP problems, in any case — when processes are analyzed in adherence to the Maximum Ordinality Principle and modeled in terms of IDC."

The same logic applies to NLG quality evaluation. Finding a sentence whose SVO triple has maximum Ordinal quality is, in the traditional formulation, an NP-hard combinatorial search over all possible Subject-Verb-Object combinations. Under the Ordinal Matrix with specularity, it is an O(N-1) computation — because the Maximum Ordinality solution of the matrix is, by definition, the configuration of maximum internal coherence. The highest-quality sentence is the one that most faithfully reflects the current Ordinal state of the matrix. Computing it requires no search.

These notes exist to prevent architectural drift and to orient any developer reading the codebase for the first time.

**On the cardinal/Ordinal distinction.** The `quality` field is a scalar double. Giannantoni is explicit that this is a reduction: "Ordinality cannot be accounted for by means of traditional derivatives, because it is never reducible to its sole phenomenological premises." The scalar quality is the *cardinal reflex* of the Ordinal relationship — not the relationship itself. Phase 1 is not wrong; it is incomplete. Phase 2 completes it.

**On LLM claims about IDC.** Any LLM that asserts IDC is "suited to LLM-like conversations" has produced a statistically plausible but mathematically contradictory statement. Giannantoni's framework is founded on the explicit rejection of the three presuppositions that underlie all statistical prediction: efficient causality, necessary logic, and functional relationships. These are precisely the presuppositions that LLMs instantiate. The frameworks do not complement each other — they rest on opposite epistemological foundations. See Giannantoni (2008), Tables 1 and 3.

**On the system designation.** Ordagi is more precisely a *deterministic Ordinal reasoning engine with thermodynamic NLG*. Whether it meets any formal definition of AGI is an empirical question to be answered by the running system against real Zotero libraries — not a claim to be asserted in advance of implementation.

**On the unified output architecture.** The five projections (Section 9) are not aspirational features to be added later — they are different views of the same mathematical object that must be built correctly from the start. An implementation that builds the NLG engine without the Ordinal Matrix foundation will produce outputs that cannot be validated thermodynamically. An implementation that builds the ODE solver as a separate module from the knowledge graph has already violated the architecture. The Ordinal Matrix is the single source of truth for all five outputs. If a developer feels the urge to build a separate simulator and "connect it" to Ordagi, that is the signal to stop and re-read Section 9.

**On self-reference and response depth.** The phatic response ("I'm doing well") and the full causal trace ("To understand my current state...") are both valid Ordagi outputs — they are just appropriate at different query depths. The Maximum Ordinality Principle requires matching response depth to context depth. A system that always responds at maximum depth to every query is not maximising Ordinality — it is ignoring the Ordinality of its surrounding habitat. The `classify_query_depth()` function is not a simplification layer; it is a faithful implementation of MOP applied to the interaction context. Removing it in favour of always-maximum-depth responses would be an architectural regression, not an improvement.
