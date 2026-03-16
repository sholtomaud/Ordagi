# =============================================================================
# OrdAGI Makefile
# Deterministic Ordinal Reasoning Engine
#
# Usage:
#   make              — build all binaries
#   make test         — build and run full test suite
#   make chat         — launch interactive REPL
#   make daemon       — launch autonomous learning daemon
#   make clean        — remove build artefacts
#   make help         — print this summary
#
# See AGENTS.md for full build system documentation.
# =============================================================================

# -----------------------------------------------------------------------------
# Compiler & Flags
# -----------------------------------------------------------------------------

CC        = gcc
CSTD      = -std=c11
WARNINGS  = -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter
INCLUDES  = -I./include
LIBS      = -lcjson -lcbor -lgsl -lgslcblas -lm

CFLAGS    = $(CSTD) $(WARNINGS) $(INCLUDES)
LDFLAGS   = $(LIBS)

# Debug build: address + UB sanitisers, debug symbols
DBGFLAGS  = -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer

# Release build: optimised, assertions disabled
RELFLAGS  = -O2 -DNDEBUG

# Default: debug symbols, no sanitisers (fast iteration)
CFLAGS   += -g

# -----------------------------------------------------------------------------
# Directories
# -----------------------------------------------------------------------------

SRCDIR    = src
INCDIR    = include
TESTDIR   = tests
BINDIR    = bin
DATADIR   = data
DOCDIR    = docs

# -----------------------------------------------------------------------------
# Sources & Objects
# -----------------------------------------------------------------------------

# All application source files (main excluded — linked separately)
LIB_SRCS  = $(SRCDIR)/memory.c       \
             $(SRCDIR)/parser.c       \
             $(SRCDIR)/nlg.c          \
             $(SRCDIR)/nlg_quality.c  \
             $(SRCDIR)/ordinal_matrix.c \
             $(SRCDIR)/emergy.c       \
             $(SRCDIR)/ode_solver.c   \
             $(SRCDIR)/mcp_client.c   \
             $(SRCDIR)/ordagi_daemon.c

LIB_OBJS  = $(LIB_SRCS:.c=.o)

MAIN_SRC  = $(SRCDIR)/main.c
MAIN_OBJ  = $(SRCDIR)/main.o

DAEMON_SRC = $(SRCDIR)/ordagi_daemon.c

# Test sources
TEST_SRCS = $(TESTDIR)/test_runner.c       \
            $(TESTDIR)/test_ice_math.c     \
            $(TESTDIR)/test_memory.c       \
            $(TESTDIR)/test_parser.c       \
            $(TESTDIR)/test_nlg.c          \
            $(TESTDIR)/test_nlg_quality.c  \
            $(TESTDIR)/test_ordinal_matrix.c \
            $(TESTDIR)/test_mcp_client.c

DRIFT_SRC = $(TESTDIR)/test_drift.c

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------

.PHONY: all build debug release clean test test-unit test-drift \
        chat daemon simulate simulate-bio emergy \
        init-data docs lint valgrind help

# Default target
all: build

# -----------------------------------------------------------------------------
# Build
# -----------------------------------------------------------------------------

## build: compile all binaries into bin/
build: $(BINDIR)/ordagi $(BINDIR)/ordagi-daemon

$(BINDIR):
	@mkdir -p $(BINDIR)

## Compile library objects
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

## Main REPL binary
$(BINDIR)/ordagi: $(LIB_OBJS) $(MAIN_OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[build] bin/ordagi ready"

## Daemon binary (main entry point in ordagi_daemon.c with daemon_main())
$(BINDIR)/ordagi-daemon: $(LIB_OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -DDAEMON_MAIN $^ -o $@ $(LDFLAGS)
	@echo "[build] bin/ordagi-daemon ready"

## debug: build with address + UB sanitisers
debug: CFLAGS += $(DBGFLAGS)
debug: build

## release: build with optimisations, no debug symbols
release: CFLAGS += $(RELFLAGS)
release: build

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

## test: build and run full test suite (unit + drift benchmark)
test: test-unit test-drift

## test-unit: run all unit tests (fast — no benchmarks)
test-unit: $(BINDIR)/test_runner | $(BINDIR)
	@echo "[test] running unit tests..."
	@$(BINDIR)/test_runner
	@echo "[test] unit tests complete"

$(BINDIR)/test_runner: $(LIB_OBJS) $(TEST_SRCS) | $(BINDIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(TEST_SRCS) -o $@ $(LDFLAGS)

## test-drift: Mercury precession benchmark — target 42.45 ± 0.5 sec/cy
## This validates Tier 2 IDC drift correction. Must pass before any
## Tier 2 code is considered correct.
test-drift: $(BINDIR)/test_drift | $(BINDIR)
	@echo "[test] running Mercury precession drift benchmark..."
	@$(BINDIR)/test_drift
	@echo "[test] drift benchmark complete"

$(BINDIR)/test_drift: $(LIB_OBJS) $(DRIFT_SRC) | $(BINDIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(DRIFT_SRC) -o $@ $(LDFLAGS)

# -----------------------------------------------------------------------------
# Run Targets
# -----------------------------------------------------------------------------

## chat: build then launch interactive REPL
chat: build init-data
	@echo "[ordagi] starting REPL..."
	@$(BINDIR)/ordagi

## daemon: build then launch autonomous learning daemon (foreground)
daemon: build init-data
	@echo "[ordagi] starting daemon..."
	@$(BINDIR)/ordagi-daemon

## simulate: run ICE benchmark; output to data/ice_output.csv
simulate: build
	@echo "[simulate] running ICE benchmark simulation..."
	@$(BINDIR)/ordagi --simulate
	@echo "[simulate] output written to data/ice_output.csv"

## simulate-bio: run Ordinal Matrix solver on test molecular system
simulate-bio: build
	@echo "[simulate-bio] running Ordinal Matrix molecular solver..."
	@$(BINDIR)/ordagi --simulate-bio
	@echo "[simulate-bio] complete"

## emergy: output dynamic emergy accounting from current ontology
emergy: build init-data
	@echo "[emergy] computing dynamic emergy matrix..."
	@$(BINDIR)/ordagi --emergy
	@echo "[emergy] output written to data/emergy_output.csv and data/emergy_output.tex"

# -----------------------------------------------------------------------------
# Data Initialisation
# -----------------------------------------------------------------------------

## init-data: bootstrap ontology.cbor from ontology.json if not present
init-data:
	@mkdir -p $(DATADIR)
	@if [ ! -f $(DATADIR)/ontology.cbor ]; then \
		echo "[init-data] ontology.cbor not found — bootstrapping from ontology.json..."; \
		$(BINDIR)/ordagi --init; \
		echo "[init-data] ontology.cbor created"; \
	else \
		echo "[init-data] ontology.cbor already exists — skipping"; \
	fi

# -----------------------------------------------------------------------------
# Code Quality
# -----------------------------------------------------------------------------

## lint: run cppcheck over src/ and include/
lint:
	@echo "[lint] running cppcheck..."
	@cppcheck --enable=all --error-exitcode=1 \
	          --suppress=missingIncludeSystem \
	          -I$(INCDIR) $(SRCDIR)/
	@echo "[lint] clean"

## valgrind: run REPL test scenario under valgrind; zero errors required
valgrind: build init-data
	@echo "[valgrind] running memory check..."
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=1 \
	          $(BINDIR)/ordagi --test-scenario
	@echo "[valgrind] clean"

## docs: generate API documentation with doxygen
docs:
	@echo "[docs] generating API documentation..."
	@doxygen Doxyfile
	@echo "[docs] output written to docs/api/"

# -----------------------------------------------------------------------------
# Docker
# -----------------------------------------------------------------------------

## docker-build: build Linux ELF binaries inside Debian container
docker-build:
	@echo "[docker] building container..."
	docker build -t ordagi-builder .
	docker run --rm -v $(PWD):/workspace -w /workspace ordagi-builder make build
	@echo "[docker] build complete — binaries in bin/"

## docker-test: run full test suite inside container
docker-test:
	docker run --rm -v $(PWD):/workspace -w /workspace ordagi-builder make test

# -----------------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------------

## clean: remove all build artefacts
clean:
	@echo "[clean] removing build artefacts..."
	@rm -rf $(BINDIR)
	@rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o
	@echo "[clean] done"

## clean-data: remove generated data files (does NOT remove ontology.json)
clean-data:
	@echo "[clean-data] removing generated data files..."
	@rm -f $(DATADIR)/ontology.cbor
	@rm -f $(DATADIR)/learned_verbs.json
	@rm -f $(DATADIR)/ice_output.csv
	@rm -f $(DATADIR)/emergy_output.csv
	@rm -f $(DATADIR)/emergy_output.tex
	@echo "[clean-data] done — ontology.json preserved"

# -----------------------------------------------------------------------------
# Help
# -----------------------------------------------------------------------------

## help: print all available targets with descriptions
help:
	@echo ""
	@echo "OrdAGI — Ordinal AGI Build System"
	@echo "=================================="
	@echo ""
	@grep -E '^## ' Makefile | sed 's/## /  make /'
	@echo ""