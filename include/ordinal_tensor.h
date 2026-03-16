#ifndef ORDINAL_TENSOR_H
#define ORDINAL_TENSOR_H

/*
 * include/ordinal_tensor.h — Core Data Structures for Ordagi
 *
 * This file defines the fundamental data structures for the Ordagi
 * reasoning engine: the OrdinalValue and the OrdinalConcept.
 *
 * Grounded in Giannantoni's Incipient Differential Calculus (IDC),
 * these structures represent knowledge not as static nodes, but as
 * generative thermodynamic entities.
 *
 * Ref: FRD Section 2.3, AGENTS.md Section 2
 */

#define MAX_CONCEPTS   4096    /* Raised to support daemon-driven growth */
#define MAX_ALIASES    10
#define MAX_NAME_LEN   64
#define MAX_ALIAS_LEN  32

/*
 * OrdinalValue: Tier 2 full Ordinal representation.
 *
 * Represents the internal structure of a generative process as a
 * binary, duet, or binary-duet function pair.
 *
 * branch_pos: The positive solution branch or primary duet solution.
 * branch_neg: The negative solution branch or secondary duet solution.
 * is_resolved: Flag indicating if Tier 2 mathematics have been computed.
 *
 * NOTE: Phase 1 implements Tier 1 (cardinal approximation).
 *       Phase 2 upgrades to full Tier 2 Ordinality.
 */
typedef struct {
    double branch_pos;
    double branch_neg;
    int    is_resolved;   /* 0 = Tier 1 only; 1 = Tier 2 computed */
} OrdinalValue;

/*
 * OrdinalConcept: The fundamental unit of the knowledge ontology.
 *
 * Every concept in Ordagi is an Ordinal node with a real generative
 * history, a thermodynamic state, and interaction metadata.
 */
typedef struct OrdinalConcept {
    /* --- Identity --- */
    char name[MAX_NAME_LEN];
    char aliases[MAX_ALIASES][MAX_ALIAS_LEN];
    int  alias_count;
    int  id;                       /* Stable integer ID for CBOR serialisation */

    /* --- Thermodynamic State: Tier 1 (Cardinal Approximation) --- */
    double cardinality;            /* Raw energy/quantity — conserved (1st Law) */
    double quality;                /* Scalar quality — cardinal shadow of Ordinality */
    int    ordinal_depth;          /* Generative depth from foundational seeds */

    /* --- Thermodynamic State: Tier 2 (Full Ordinality — Phase 2) --- */
    OrdinalValue ordinal_value;    /* Binary/duet/binary-duet function pair */

    /* --- Generative History (serialised as integer IDs, never raw pointers) --- */
    int parent_A_id;               /* 0 = no parent (seed) */
    int parent_B_id;               /* 0 = no parent (seed) */

    /* --- Interaction Metadata --- */
    char   interaction_type[32];   /* "seed"|"co-production"|"inter-action"|"feed-back"|"dissipation" */
    double alpha;                  /* Fractional order: 0.5 | 2.0 | 1.0 | N/A */
    char   verb[32];               /* Odum energy verb */
    char   adverb[32];             /* Odum adverb modifier */

    /* --- Runtime only (NOT serialised to CBOR) --- */
    struct OrdinalConcept *parent_A;   /* Resolved at load time from parent_A_id */
    struct OrdinalConcept *parent_B;   /* Resolved at load time from parent_B_id */
    int visited;                       /* Traversal flag — reset before each query */
} OrdinalConcept;

#endif /* ORDINAL_TENSOR_H */
