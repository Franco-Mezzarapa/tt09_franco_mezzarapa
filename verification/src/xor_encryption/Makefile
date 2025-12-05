# Makefile for xor_encrypt testbench (Verilator)

# Paths
TOP         := xor_encrypt
RTL_DIR     := ../../rtl
TB_DIR      := .
BUILD_DIR   := obj_dir_xor
EXE_PREFIX  := sim_xor_encrypt

# Allow JOBS to be passed from parent Makefile, default to 1
JOBS ?= 1

# Source files
RTL_FILES   := $(RTL_DIR)/xor_encrypt.v
TB_SRC      := $(TB_DIR)/xor_encrypt.cpp $(TB_DIR)/xor_encrypt_tests.cpp

# Verilator flags
VERILATOR_FLAGS := --cc --exe --assert --coverage --trace -Wall -O3 -Wno-WIDTHEXPAND --top-module $(TOP) --output-split 20

# C++ Compiler Defines
# These must match the parameters in the Verilog module
CPP_DEFINES := -DDATA_WIDTH=64 -DKEY_WIDTH=8

.PHONY: all run clean coverage build

# Default target
all: build

# Build for MSG_SIZE=64, KEY_SIZE=8
build:
	@echo "Building for DATA_WIDTH=64, KEY_WIDTH=8"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR) -GMSG_SIZE=64 -GKEY_SIZE=8 $(RTL_FILES) $(TB_SRC) -CFLAGS "$(CPP_DEFINES)"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR) -f V$(TOP).mk
	@cp $(BUILD_DIR)/V$(TOP) $(EXE_PREFIX)

run: build
	@echo "Running simulation..."
	@mkdir -p logs
	@./$(EXE_PREFIX) logs/coverage.dat

# Coverage reporting
coverage: run
	@echo "Generating LCOV info file..."
	@verilator_coverage --write-info logs/coverage.info logs/coverage.dat
	@echo "Generating HTML report..."
	@genhtml logs/coverage.info --output-directory coverage_html
	@echo "Coverage report generated in coverage_html/ directory."

clean:
	rm -rf $(BUILD_DIR)* $(EXE_PREFIX)* xor_encrypt.vcd logs coverage_html *.gcda *.gcno
