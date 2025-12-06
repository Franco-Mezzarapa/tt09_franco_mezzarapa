# Unified Makefile for the entire verification project

# Default number of jobs for parallel execution (can be overridden)
JOBS ?= $(shell nproc 2>/dev/null || echo 1)

.PHONY: all all_tests all_coverage clean_all deserializer serializer xor_encrypt integration_test test_report

# --- High-Level Targets ---

# Default target: build all testbenches in parallel
all:
	@$(MAKE) -j$(JOBS) deserializer serializer xor_encrypt integration_test

# Run all test suites
all_tests:
	@$(MAKE) -C src/deserializer run JOBS=$(JOBS)
	@$(MAKE) -C src/serializer run JOBS=$(JOBS)
	@$(MAKE) -C src/xor_encryption run JOBS=$(JOBS)
	@$(MAKE) -C src/integration_test run JOBS=$(JOBS)

# Create a PASS/FAIL summary for all modules without stopping on failures
test_report:
	@mkdir -p report
	@echo "\n--- Running test suites for PASS/FAIL summary ---"
	@set +e; \
	for m in deserializer serializer xor_encryption integration_test; do \
	  echo "Running $$m..."; \
	  if $(MAKE) -C src/$$m run JOBS=$(JOBS); then \
	    echo "$$m: PASS" | tee report/$$m.summary; \
	  else \
	    echo "$$m: FAIL" | tee report/$$m.summary; \
	  fi; \
	done; \
	set -e; \
	echo "\nSummary files written to verification/report/*.summary";

# Generate a single, merged coverage report for the entire project
all_coverage:
	@echo "\n--- Running tests for coverage (non-fatal on failures) ---"
	@set +e; \
	$(MAKE) -C src/deserializer run JOBS=$(JOBS); \
	$(MAKE) -C src/serializer run JOBS=$(JOBS); \
	$(MAKE) -C src/xor_encryption run JOBS=$(JOBS); \
	$(MAKE) -C src/integration_test run JOBS=$(JOBS); \
	set -e; \
	echo "\n--- Collecting *.dat files and generating merged report ---";
	@mkdir -p coverage
	@files=""; \
	for f in \
	  src/deserializer/logs/coverage_*.dat \
	  src/serializer/logs/coverage_*.dat \
	  src/xor_encryption/logs/coverage.dat \
	  src/integration_test/logs/coverage_top.dat; do \
	  if [ -f "$$f" ]; then files="$$files $$f"; fi; \
	done; \
	if [ -z "$$files" ]; then \
	  echo "No coverage .dat files found. Skipping merge."; \
	else \
	  echo "Merging: $$files"; \
	  verilator_coverage --write-info coverage/full_coverage.info $$files; \
	  echo "\n--- Generating final HTML report ---"; \
	  genhtml coverage/full_coverage.info --output-directory coverage/final_html_report --ignore-errors source --synthesize-missing; \
	  echo "\n\n>>> Unified coverage report generated in: verification/coverage/final_html_report/index.html <<<\n"; \
	fi

# Clean up everything in all sub-directories
clean_all:
	@echo "Cleaning all sub-project directories..."
	@$(MAKE) -C src/deserializer clean
	@$(MAKE) -C src/serializer clean
	@$(MAKE) -C src/xor_encryption clean
	@$(MAKE) -C src/integration_test clean
	@rm -rf coverage

# --- Individual Test Suite Targets ---

# Build the deserializer testbench
deserializer:
	@$(MAKE) -C src/deserializer all JOBS=$(JOBS)

# Build the serializer testbench
serializer:
	@$(MAKE) -C src/serializer all JOBS=$(JOBS)

# Build the xor_encrypt testbench
xor_encrypt:
	@$(MAKE) -C src/xor_encryption build JOBS=$(JOBS)

# Build the top-level integration testbench
integration_test:
	@$(MAKE) -C src/integration_test build JOBS=$(JOBS)
