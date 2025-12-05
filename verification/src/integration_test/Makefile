# Makefile for top-level integration testbench (Verilator)

# Paths
TOP_MODULE  := tt_um_franco_mezzarapa
RTL_DIR     := ../../rtl
TB_DIR      := .
BUILD_DIR   := obj_dir_top
EXE_PREFIX  := sim_top

# Allow JOBS to be passed from parent Makefile, default to 1
JOBS ?= 1

# Source files
# We need to include all verilog files for the top-level build
RTL_FILES   := $(wildcard $(RTL_DIR)/*.v)
TB_SRC      := $(TB_DIR)/integration_test.cpp $(TB_DIR)/integration_test_cases.cpp

# Verilator flags
VERILATOR_FLAGS := --cc --exe --assert --coverage --trace -Wall -O3 -Wno-WIDTHEXPAND --top-module $(TOP_MODULE) --output-split 20

# C++ Compiler Defines
CPP_DEFINES := -DDATA_WIDTH=64 -DKEY_WIDTH=8

.PHONY: all run clean coverage build

# Default target
all: build

build:
	@echo "Building for Top-Level Integration Test"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR) $(RTL_FILES) $(TB_SRC) -CFLAGS "$(CPP_DEFINES)"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR) -f V$(TOP_MODULE).mk
	@cp $(BUILD_DIR)/V$(TOP_MODULE) $(EXE_PREFIX)

run: build
	@echo "Running top-level integration simulation..."
	@mkdir -p logs
	@./$(EXE_PREFIX) logs/coverage_top.dat

coverage: run
	@echo "Generating LCOV info file for top-level..."
	@verilator_coverage --write-info logs/coverage.info logs/coverage_top.dat
	@echo "Generating HTML report..."
	@genhtml logs/coverage.info --output-directory coverage_html
	@echo "Coverage report generated in coverage_html/ directory."

clean:
	rm -rf $(BUILD_DIR)* $(EXE_PREFIX)* top_level.vcd logs coverage_html *.gcda *.gcno
