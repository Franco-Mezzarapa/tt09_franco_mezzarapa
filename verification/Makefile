# Unified Makefile for the entire verification project

# Default number of jobs for parallel execution (can be overridden)
JOBS ?= $(shell nproc 2>/dev/null || echo 1)

.PHONY: all all_tests all_coverage clean_all deserializer serializer xor_encrypt integration_test

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

# Generate a single, merged coverage report for the entire project
all_coverage: all_tests
	@echo "\n--- Merging coverage data from all test suites ---"
	@mkdir -p coverage
	@verilator_coverage --write-info coverage/full_coverage.info \
		src/deserializer/logs/coverage*.dat \
		src/serializer/logs/coverage*.dat \
		src/xor_encryption/logs/coverage.dat \
		src/integration_test/logs/coverage_top.dat
	@echo "\n--- Generating final HTML report ---"
	@genhtml coverage/full_coverage.info --output-directory coverage/final_html_report
	@echo "\n\n>>> Unified coverage report generated in: verification/coverage/final_html_report/index.html <<<\n"

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
