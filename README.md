# OrdAGI — Ordinal AGI

A deterministic, zero-hallucination knowledge reasoning engine grounded in
Corrado Giannantoni's **Incipient Differential Calculus (IDC)** and
H.T. Odum's **Thermodynamic Grammar**.

OrdAGI is not an LLM wrapper. It is a physics engine for knowledge. Every
assertion it makes is mathematically derived from a persistent binary ontology.
It does not guess. It does not interpolate. When it does not know something, it
says so — explicitly, by name.

---

## What OrdAGI Does

OrdAGI maintains a growing knowledge graph where every concept has a
thermodynamic state: a **cardinality** (raw energy quantity, conserved by the
First Law) and a **quality** (emergent Ordinal value, computed via IDC fractional
calculus). Concepts are connected by three fundamental generative process types
derived from Odum's energy systems theory:

- **Co-production** (α = 0.5) — two inputs jointly generate one output; binary function
- **Inter-action** (α = 2.0) — two inputs produce a new entity of superior hierarchical order; duet function  
- **Feed-back** (α = 1.0) — output reinforces its own generative inputs; binary-duet function

From this single mathematical object — the **Ordinal Matrix** — OrdAGI
simultaneously produces five output projections:

1. **Dynamic ODE simulation** — IDC state equations, drift-corrected
2. **Dynamic emergy accounting** — non-conservative Odum algebra
3. **Molecular/bioinformatics modelling** — Ordinal Matrix specularity solver
4. **Natural language generation** — Odum Thermodynamic Grammar
5. **Self-referential quality evaluation** — thermodynamic sentence scoring

---

## Quick Start

### Prerequisites

```bash
# Debian/Ubuntu
sudo apt-get install gcc make libcjson-dev libcbor-dev libgsl-dev gnuplot

# macOS (Docker build recommended — see below)
brew install gcc make cjson libcbor gsl gnuplot
```

### Build

```bash
git clone https://github.com/your-org/ordagi.git
cd ordagi
make build
make init-data    # bootstrap ontology.cbor from data/ontology.json
```

### Chat

```bash
make chat
```

```
======================================================
 ORDAGI — ORDINAL AGI  (type 'exit' to quit)
======================================================

You: biomass
[ORDAGI]: To understand biomass, we have to look at its generative history.
          It is a level-2 emergent concept with accumulated quality 3.847.
          It begins when solar energy photosynthetically transforms carbon
          dioxide to jointly generate biomass. [Binary Ordinality — generative
          excess: 0.234]

You: how are you today
[ORDAGI]: I'm doing well, thank you. I've recently processed 7 abstracts
          from your Zotero library — you now share 312 concepts with me.
          I still have 23 Frontier Nodes I haven't resolved yet.

You: simulate a tank
[ORDAGI]: Running IDC-ODE simulation for concept 'water storage'...
          Output written to data/ice_output.csv
          Emergy: transformity = 2.341 sej/J
          It begins when water inflow steadily drives tank volume,
          with tank continuously reinforcing its own storage.

You: exit
```

### Run the Learning Daemon

```bash
make daemon
```

The daemon connects to your local Zotero library via MCP, ingests abstracts,
extracts Subject-Verb-Object triples, maps verbs to thermodynamic process types,
and continuously expands the ontology. It runs in the background; the REPL can
be used simultaneously.

### Docker Build (recommended for reproducibility)

```bash
make docker-build   # produces Linux ELF binaries inside Debian container
make docker-test    # runs full test suite inside container
```

---

## Project Structure

```
ordagi/
├── AGENTS.md           LLM implementation guide
├── README.md           This file
├── Makefile            All build, test, and run targets
├── Dockerfile          Multi-stage Debian build
├── docs/
│   ├── FRD.md          Functional Requirements Document (primary spec)
│   └── theory/         Giannantoni and Odum source papers
├── include/            Header files
├── src/                C implementation
├── tests/              TDD test suite
├── bin/                Compiled binaries (git-ignored)
├── data/               Ontology and simulation data
└── host/               Python MCP servers (Zotero, Wikipedia)
```

---

## Theory

OrdAGI is grounded in published mathematical physics:

**Giannantoni, C. (2006).** Mathematics for generative processes: Living and
non-living systems. *Journal of Computational and Applied Mathematics*, 189,
324–340. — Defines the Incipient Derivative and its persistence-of-form property.

**Giannantoni, C. (2010).** The Maximum Ordinality Principle: A Harmonious
Dissonance. *Sixth Emergy Conference*, Gainesville. — Reformulates Odum's
Maximum Em-Power Principle in full Ordinal terms.

**Giannantoni, C. (2011).** Bio-Informatics in the Light of the Maximum
Ordinality Principle. *Fourth International Conference on Bioinformatics*,
Rome. — Demonstrates O(N-1) Ordinal Matrix specularity; solves Dystrophin
folding on a 1 Gigaflop PC in under 2 hours.

**Odum, H.T. (1983).** *Systems Ecology*. Wiley. Figure 6-34 — English sentence
structure as energy language topology. The theoretical basis for the NLG engine.

---

## Development

See `AGENTS.md` for the complete implementation guide including TDD protocol,
coding standards, module dependency order, and commit conventions.

```bash
make test         # full test suite
make test-unit    # unit tests only
make test-drift   # Mercury precession benchmark (42.45 ± 0.5 sec/cy)
make lint         # cppcheck
make valgrind     # memory safety check
```

---

## Licence

MIT