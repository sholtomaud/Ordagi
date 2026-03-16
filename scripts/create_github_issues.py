import os
import sys
import json
import subprocess

# List of issues based on FRD Section 8 and AGENTS.md
ISSUES = [
    # Phase 1: Cardinal Foundation
    {
        "title": "Phase 1: Define Core Data Structures in include/ordinal_tensor.h",
        "body": """Implement the fundamental data structures for Ordagi as specified in FRD Section 2.3 and AGENTS.md Section 2.

- Define `OrdinalValue` struct (Tier 2 ready).
- Define `OrdinalConcept` struct with Identity, Tier 1 State, Tier 2 State, Generative History, and Interaction Metadata.
- Set `MAX_CONCEPTS=4096`.
- Use integer ID fields for stable serialisation.

cc @Jules""",
        "labels": ["phase:1", "component:kernel", "type:feature"]
    },
    {
        "title": "Phase 1: Implement Odum Verb Table in include/verb_table.h",
        "body": """Implement the complete `ODUM_VERB_TABLE` as specified in FRD Section 4.1.

- Include all interaction types: co-production (alpha=0.5), inter-action (alpha=2.0), feed-back (alpha=1.0), and dissipation (alpha=1.0).
- Ensure it is a compile-time constant.
- Add sentinel entry.

cc @Jules""",
        "labels": ["phase:1", "component:kernel", "type:feature"]
    },
    {
        "title": "Phase 1: Implement Tier 1 Cardinal Quality Engine in include/ice_math.h",
        "body": """Implement the Grünwald-Letnikov cardinal engine as specified in FRD Section 2.4 and AGENTS.md Section 5.1.

- Implement `calculate_fractional_weights()`.
- Implement `compute_quality_tier1()` with ICE state evolution loop.
- Use fixed constants: `ICE_STEPS=100`, `ICE_DT=0.1`, `ICE_K=0.1`.

cc @Jules""",
        "labels": ["phase:1", "component:math", "type:feature"]
    },
    {
        "title": "Phase 1: Implement CBOR Persistent Memory with Two-Pass Loading in src/memory.c",
        "body": """Implement CBOR serialisation and deserialisation as specified in FRD Section 5 and AGENTS.md Section 7.

- Implement `ontology.cbor` loading and saving.
- **Mandatory**: Two-pass load procedure (Pass 1: load records; Pass 2: resolve parent pointers via IDs).
- Never store raw pointers in CBOR.

cc @Jules""",
        "labels": ["phase:1", "component:memory", "type:feature"]
    },
    {
        "title": "Phase 1: Migrate REPL to CBOR Loader in src/main.c",
        "body": """Update the main REPL entry point to use the new CBOR memory system.

- Replace JSON loader with CBOR loader.
- Ensure `make init-data` correctly bootstraps from `ontology.json`.
- Preserve existing REPL loop functionality.

cc @Jules""",
        "labels": ["phase:1", "component:repl", "type:refactor"]
    },
    {
        "title": "Phase 1: Implement Per-Interaction Alpha Assignment",
        "body": """Refactor the system to use alpha values derived from the verb table for each interaction.

- Remove any global alpha constants.
- Assign `alpha` and `interaction_type` per-interaction during concept creation/learning.

cc @Jules""",
        "labels": ["phase:1", "component:kernel", "type:refactor"]
    },

    # Phase 2: Full IDC Mathematics
    {
        "title": "Phase 2: Implement Faà di Bruno Drift Correction in include/ice_math.h",
        "body": """Implement the Tier 2 drift correction as specified in FRD Section 2.5.1.

- Implement the Faà di Bruno correction term to eliminate derivative drift.
- Use GSL for partition enumeration in the psi computation.

cc @Jules""",
        "labels": ["phase:2", "component:math", "type:feature"]
    },
    {
        "title": "Phase 2: Implement Binary Function (q=1/2) for Co-production",
        "body": """Implement `compute_binary_function()` as specified in FRD Section 2.5.2.

- Model co-production where both inputs are necessary.
- Output inherits form of both parents.

cc @Jules""",
        "labels": ["phase:2", "component:math", "type:feature"]
    },
    {
        "title": "Phase 2: Implement Duet Function (q=2) for Inter-action",
        "body": """Implement `compute_duet_function()` as specified in FRD Section 2.5.3.

- Model inter-action producing an emergent whole of superior hierarchical order.

cc @Jules""",
        "labels": ["phase:2", "component:math", "type:feature"]
    },
    {
        "title": "Phase 2: Implement Binary-Duet Function (q=2/2) for Feed-back",
        "body": """Implement `compute_binary_duet_function()` as specified in FRD Section 2.5.4.

- Model feed-back where output reinforces its own generative inputs.

cc @Jules""",
        "labels": ["phase:2", "component:math", "type:feature"]
    },
    {
        "title": "Phase 2: Integrate Tier 2 Quality Computation",
        "body": """Implement `compute_quality_tier2()` integrating drift correction and the new function types.

- Combine GL results with Faà di Bruno corrections.
- Ensure correct branch selection for binary/duet functions.

cc @Jules""",
        "labels": ["phase:2", "component:math", "type:feature"]
    },
    {
        "title": "Phase 2: Extend CBOR Schema for Tier 2 Ordinal Values",
        "body": """Extend the CBOR serialisation in `src/memory.c` to support Tier 2 fields.

- Add `ordinal_branch_pos`, `ordinal_branch_neg`, and `ordinal_resolved`.
- Ensure backward compatibility with Tier 1 records if possible.

cc @Jules""",
        "labels": ["phase:2", "component:memory", "type:feature"]
    },
    {
        "title": "Phase 2: Implement Mercury Precession Drift Benchmark (make test-drift)",
        "body": """Implement the Mercury precession benchmark as specified in FRD Section 2.1 and AGENTS.md Section 4.4.

- Target result: 42.45 ± 0.5 arcseconds per century.
- This is the definitive test for Tier 2 correctness.

cc @Jules""",
        "labels": ["phase:2", "component:test", "type:test"]
    },

    # Phase 3: Language Engine
    {
        "title": "Phase 3: Modularize NLG into src/nlg.c",
        "body": """Extract all Natural Language Generation logic into a dedicated module.

- Move `converse()` and `build_narrative()` from `main.c` to `src/nlg.c`.
- Define clean interface in `include/nlg.h`.

cc @Jules""",
        "labels": ["phase:3", "component:nlg", "type:refactor"]
    },
    {
        "title": "Phase 3: Implement Interaction-Aware Sentence Templates in NLG",
        "body": """Implement the interaction-type-aware sentence templates as specified in FRD Section 3.2.

- Different templates for co-production, inter-action, and feed-back.
- Handle different narrative steps (First step, parallel branch, higher order).

cc @Jules""",
        "labels": ["phase:3", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 3: Implement Tier 2 Ordinal Excess Statements in NLG",
        "body": """Extend NLG to report Ordinal excess when Tier 2 data is resolved.

- Append statements like \"This process exhibits binary Ordinality, with a generative excess of X...\"
- Refer to FRD Section 3.3.

cc @Jules""",
        "labels": ["phase:3", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 3: Implement Response Guarantee Templates (Frontier, Seed, Unknown)",
        "body": """Implement all standard response templates to ensure zero hallucination.

- Factual responses for Seeds.
- Mathematical unresolved status for Frontier Nodes.
- Explicit "not in ontology" for unknown concepts.
- Refer to FRD Section 3.2.

cc @Jules""",
        "labels": ["phase:3", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 3: Implement Rule-Based SVO Extractor in src/parser.c",
        "body": """Implement the Subject-Verb-Object extractor as specified in FRD Section 6.3.

- Bigram scan for compound noun phrases.
- Verb identification via table lookup.
- Longest noun phrase detection for S and O.

cc @Jules""",
        "labels": ["phase:3", "component:parser", "type:feature"]
    },

    # Phase 4: MCP & Daemon
    {
        "title": "Phase 4: Implement Universal MCP JSON-RPC Client in src/mcp_client.c",
        "body": """Implement the MCP client as specified in FRD Section 6.1 and AGENTS.md Section 8.

- JSON-RPC 2.0 over stdio.
- Parse responses with cJSON.
- Implement timeouts.

cc @Jules""",
        "labels": ["phase:4", "component:mcp", "type:feature"]
    },
    {
        "title": "Phase 4: Implement Python MCP Server for Zotero",
        "body": """Implement the Zotero MCP bridge in `host/mcp-zotero/`.

- Provide `get_unprocessed_abstracts` and `mark_abstract_processed`.
- Access local Zotero sqlite database.

cc @Jules""",
        "labels": ["phase:4", "component:mcp", "type:feature"]
    },
    {
        "title": "Phase 4: Implement Python MCP Server for Wikipedia",
        "body": """Implement the Wikipedia MCP bridge in `host/mcp-wikipedia/`.

- Provide `define` tool to return article summaries.

cc @Jules""",
        "labels": ["phase:4", "component:mcp", "type:feature"]
    },
    {
        "title": "Phase 4: Implement Autonomous Learning Loop in src/ordagi_daemon.c",
        "body": """Implement the full daemon learning loop as specified in FRD Section 6.2.

- Ingest from Zotero.
- Extract SVOs.
- Map to thermodynamic types.
- Update CBOR brain.

cc @Jules""",
        "labels": ["phase:4", "component:daemon", "type:feature"]
    },
    {
        "title": "Phase 4: Implement Frontier Verb Resolution and Persistence",
        "body": """Implement logic to handle and resolve unknown verbs (Frontier Verbs).

- Query Wikipedia for definitions.
- Classify into interaction types.
- Persist in `data/learned_verbs.json`.

cc @Jules""",
        "labels": ["phase:4", "component:daemon", "type:feature"]
    },
    {
        "title": "Phase 4: Implement Frontier Node Resolution with MCP Fallback Chain",
        "body": """Implement the background resolution of Frontier Nodes.

- Use Wikipedia, ABS, or Wayback fallback.
- Resolve parents and compute quality.

cc @Jules""",
        "labels": ["phase:4", "component:daemon", "type:feature"]
    },

    # Phase 3 (Unified Application Domains) - Re-grouped logically
    {
        "title": "Phase 5: Define OrdinalMatrix and Specularity Structures in include/ordinal_matrix.h",
        "body": """Define the structures for the Tier 3 Ordinal Matrix as specified in FRD Section 2.6.

- Define `OrdinalMatrix` with reference element and lambda correlates.
- Define specularity threshold constants.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:feature"]
    },
    {
        "title": "Phase 5: Implement Ordinal Matrix Construction in src/ordinal_matrix.c",
        "body": """Implement `ordinal_matrix_build()` to construct the matrix from the knowledge base.

- Populate matrix elements from ontology relationships.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:feature"]
    },
    {
        "title": "Phase 5: Implement MOP Specularity Validation in src/ordinal_matrix.c",
        "body": """Implement `ordinal_matrix_check_specularity()` as specified in FRD Section 2.6.2.

- Validate if the ontology has reached the Maximum Ordinality conditions for O(N-1) reduction.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:feature"]
    },
    {
        "title": "Phase 5: Implement O(N-1) Specularity Solver in src/ordinal_matrix.c",
        "body": """Implement the Tier 3 solver as specified in FRD Section 2.6.1.

- Use O(N-1) path when specularity holds.
- Use O(N^2) fallback for sparse matrices.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:feature"]
    },
    {
        "title": "Phase 5: Implement Ordinal Matrix Query Interface",
        "body": """Implement `ordinal_matrix_query()` to extract specific concept states from the solved matrix.

- Bridge the Matrix solver with the rest of the application.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:feature"]
    },
    {
        "title": "Phase 5: Implement IDC-ODE Dynamic Systems Simulator in src/ode_solver.c",
        "body": """Implement Projection 1: Dynamic ODE Simulation as specified in FRD Section 9.1.

- Re-express standard ODE models (Lotka-Volterra, etc.) in IDC terms.
- Output CSV for gnuplot.

cc @Jules""",
        "labels": ["phase:5", "component:simulation", "type:feature"]
    },
    {
        "title": "Phase 5: Implement Dynamic Emergy Accounting in src/emergy.c",
        "body": """Implement Projection 2: Dynamic Emergy Accounting as specified in FRD Section 9.2.

- Compute transformities and emergy flows.
- Output CSV and LaTeX tables.

cc @Jules""",
        "labels": ["phase:5", "component:emergy", "type:feature"]
    },
    {
        "title": "Phase 5: Implement Self-Referential Sentence Quality Evaluator in src/nlg_quality.c",
        "body": """Implement Projection 5: Self-Referential Quality Evaluation as specified in FRD Appendix D.

- Evaluate thermodynamic quality of SVO triples in candidate sentences.
- Use IDC mathematics to score sentence coherence.

cc @Jules""",
        "labels": ["phase:5", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 5: Integrate Quality Gate into NLG Pipeline",
        "body": """Insert the quality gate as the final step in `build_narrative()`.

- Withhold outputs below `NLG_QUALITY_THRESHOLD`.
- Zero hallucination at the linguistic layer.

cc @Jules""",
        "labels": ["phase:5", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 5: Add make simulate-bio Target for Molecular Solver",
        "body": """Implement Projection 3: Bioinformatics/Molecular Modelling.

- Add `make simulate-bio` target.
- Solve a test molecular system (e.g., protein folding) using the Ordinal Matrix.
- Refer to FRD Section 9.3.

cc @Jules""",
        "labels": ["phase:5", "component:matrix", "type:task"]
    },
    {
        "title": "Phase 5: Add make emergy Target for Dynamic Emergy Output",
        "body": """Operationalise the emergy accounting target.

- Add `make emergy` to Makefile.
- Ensure it produces valid accounting for the current ontology.

cc @Jules""",
        "labels": ["phase:5", "component:emergy", "type:task"]
    },

    # Phase 6: Self-Model & Depth Calibration
    {
        "title": "Phase 6: Implement Unified Output Validation Test",
        "body": """Validate the unified output architecture as specified in FRD Section 9.6.

- Ensure a single query correctly triggers all five projections (ODE, Emergy, Molecular, NLG, Quality).

cc @Jules""",
        "labels": ["phase:6", "component:test", "type:test"]
    },
    {
        "title": "Phase 6: Add Bootstrap Seeds for Ordagi and User Self-Models",
        "body": """Initialise the self-model seeds in `data/ontology.json` as specified in FRD Section 10.2.

- Add seeds for MOP, Thermodynamic Grammar, MCP.
- Add interactions yielding "Ordagi" and "User".

cc @Jules""",
        "labels": ["phase:6", "component:data", "type:data"]
    },
    {
        "title": "Phase 6: Implement UserModel and Interaction Tracking",
        "body": """Implement the `UserModel` struct and tracking logic as specified in FRD Section 6.5.2.

- Track communication style, knowledge domains, and interaction history.

cc @Jules""",
        "labels": ["phase:6", "component:daemon", "type:feature"]
    },
    {
        "title": "Phase 6: Implement Automatic Quality Updates for Self-Concepts",
        "body": """Implement logic to update `quality(Ordagi)` and `quality(User)` as specified in FRD Section 6.5.4.

- Quality of Ordagi grows with ontology size.
- Quality of User grows with shared concept count.

cc @Jules""",
        "labels": ["phase:6", "component:daemon", "type:feature"]
    },
    {
        "title": "Phase 6: Implement 5-Level Query Depth Classifier in src/parser.c",
        "body": """Implement `classify_query_depth()` as specified in FRD Section 3.5.

- Classify queries into PHATIC, FACTUAL, EXPLANATORY, CAUSAL, or FULL.

cc @Jules""",
        "labels": ["phase:6", "component:parser", "type:feature"]
    },
    {
        "title": "Phase 6: Implement Depth-Based NLG Response Routing",
        "body": """Route NLG output based on classified query depth as specified in FRD Section 3.5.3.

- Calibration of response depth to interaction context.

cc @Jules""",
        "labels": ["phase:6", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 6: Implement Phatic Response Generation (Depth 0)",
        "body": """Implement `generate_phatic_response()` as specified in FRD Section 3.5.4.

- Ground phatic responses in real system state (processed abstracts, frontier node count, etc.).

cc @Jules""",
        "labels": ["phase:6", "component:nlg", "type:feature"]
    },
    {
        "title": "Phase 6: Persist UserModel in CBOR",
        "body": """Ensure the `UserModel` is persisted as a special CBOR record alongside the ontology.

- Load and save on every session.

cc @Jules""",
        "labels": ["phase:6", "component:memory", "type:feature"]
    },

    # Phase 7: Hardening & Validation
    {
        "title": "Phase 7: Ensure All Makefile Targets are Operational",
        "body": """Finalise the build system as specified in AGENTS.md Section 3.

- Ensure `make build`, `make test`, `make chat`, `make daemon`, `make simulate`, `make docs`, `make lint`, `make valgrind` all work.

cc @Jules""",
        "labels": ["phase:7", "component:build", "type:task"]
    },
    {
        "title": "Phase 7: Optimize and Validate Multi-Stage Dockerfile",
        "body": """Ensure reproducible builds via Docker as specified in AGENTS.md Section 3.3.

- Multi-stage build for minimal binary size.
- Verify Linux ELF compatibility.

cc @Jules""",
        "labels": ["phase:7", "component:build", "type:task"]
    },
    {
        "title": "Phase 7: Conduct Full Memory Safety Audit (Valgrind)",
        "body": """Ensure zero memory leaks and errors across the entire codebase.

- `make valgrind` must be green for all major operations.

cc @Jules""",
        "labels": ["phase:7", "component:test", "type:test"]
    },
    {
        "title": "Phase 7: Implement Dynamic Knowledge Base Growth (realloc)",
        "body": """Replace static concept array with a dynamic growth strategy using `realloc`.

- Support growth beyond `MAX_CONCEPTS` if necessary.

cc @Jules""",
        "labels": ["phase:7", "component:memory", "type:refactor"]
    },
    {
        "title": "Phase 7: Conduct End-to-End Integration Test (10 Abstracts)",
        "body": """Run end-to-end integration test as specified in FRD Section 8.

- Ingest 10 Zotero abstracts.
- Verify CBOR round-trip.
- Verify correct NLG output for learned concepts.

cc @Jules""",
        "labels": ["phase:7", "component:test", "type:test"]
    },
    {
        "title": "Phase 7: Document Tier 1 vs Tier 2 Divergence Benchmark",
        "body": """Measure and document the divergence in quality scores between Tier 1 and Tier 2.

- Demonstrate the mathematical significance of drift correction on a known ontology.

cc @Jules""",
        "labels": ["phase:7", "component:docs", "type:test"]
    },

    # Infrastructure issues from AGENTS.md
    {
        "title": "Infra: Setup Test Harness in tests/test_runner.c",
        "body": """Implement the minimal test harness as specified in AGENTS.md Section 4.2.

- Define assertion macros.
- Implement `RUN_TEST` and `PRINT_TEST_SUMMARY` macros.
- Ensure it supports the TDD workflow.

cc @Jules""",
        "labels": ["phase:1", "component:test", "type:task"]
    },
    {
        "title": "Infra: Setup GitHub Actions CI Workflow",
        "body": """Create `.github/workflows/ci.yml` to automate builds and tests.

- Run `make test` on every push and pull request.
- Use the Docker-based build if possible for consistency.

cc @Jules""",
        "labels": ["phase:1", "component:ci", "type:task"]
    },
    {
        "title": "Infra: Add Doxygen Configuration (Doxyfile)",
        "body": """Create and configure `Doxyfile` for API documentation generation.

- Ensure `make docs` correctly generates output in `docs/api/`.

cc @Jules""",
        "labels": ["phase:1", "component:docs", "type:task"]
    }
]

def create_issues_gh(repo):
    for issue in ISSUES:
        cmd = [
            "gh", "issue", "create",
            "-R", repo,
            "-t", issue["title"],
            "-b", issue["body"]
        ]
        for label in issue["labels"]:
            cmd.extend(["-l", label])

        print(f"Creating: {issue['title']}...")
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error creating issue: {result.stderr}")
        else:
            print(f"Created: {result.stdout.strip()}")

def print_issue_commands(repo):
    print(f"# Commands to create issues for {repo}")
    for issue in ISSUES:
        title = issue["title"].replace('"', '\\"')
        body = issue["body"].replace('"', '\\"').replace('\n', '\\n')
        labels = ",".join(issue["labels"])
        print(f'gh issue create -R "{repo}" -t "{title}" -b "{body}" -l "{labels}"')

def main():
    repo = "sholtomaud/Ordagi" # Default from git remote

    # Try to get repo from git
    try:
        remote = subprocess.check_output(["git", "remote", "get-url", "origin"], text=True).strip()
        if "github.com" in remote:
            repo = remote.split("github.com/")[-1].replace(".git", "")
    except:
        pass

    if len(sys.argv) > 1 and sys.argv[1] == "--run":
        create_issues_gh(repo)
    else:
        print_issue_commands(repo)
        print("\n# To run these, make sure 'gh' is installed and authenticated, then run:")
        print(f"# python3 {sys.argv[0]} --run")

if __name__ == "__main__":
    main()
