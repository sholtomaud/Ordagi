#ifndef ORDINAL_TENSOR_H
#define ORDINAL_TENSOR_H

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

#endif /* ORDINAL_TENSOR_H */
