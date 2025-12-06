# Makefile for serializer testbench (Verilator)

# Paths
TOP         := serializer
RTL_DIR     := ../../rtl
TB_DIR      := .
BUILD_DIR   := obj_dir_ser
EXE_PREFIX  := sim_serializer

# Allow JOBS to be passed from parent Makefile, default to 1
JOBS ?= 1

# Source files
RTL_FILES   := $(RTL_DIR)/serializer.v
TB_SRC      := $(TB_DIR)/serializer.cpp $(TB_DIR)/serializer_tests.cpp

# Verilator flags
VERILATOR_FLAGS := --cc --exe --assert --coverage --trace -Wall -O3 -Wno-WIDTHEXPAND --top-module $(TOP) --output-split 20

.PHONY: all run clean coverage build64 run64

# Default target
all: build64

# Build for WIDTH=64
build64:
	@echo "Building for WIDTH=64"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR)64 -GMSG_SIZE=64 $(RTL_FILES) $(TB_SRC) -CFLAGS "-DDATA_WIDTH=64"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR)64 -f V$(TOP).mk
	@cp $(BUILD_DIR)64/V$(TOP) $(EXE_PREFIX)_64

# Run for WIDTH=64
run64: build64
	@echo "Running for WIDTH=64"
	@mkdir -p logs
	@./$(EXE_PREFIX)_64 logs/coverage_64.dat

run: run64

# Coverage reporting
coverage: run
	@echo "Generating LCOV info file..."
	@verilator_coverage --write-info logs/coverage.info logs/coverage_*.dat
	@echo "Generating HTML report..."
	@genhtml logs/coverage.info --output-directory coverage_html --ignore-errors source --synthesize-missing
	@echo "Coverage report generated in coverage_html/ directory."

clean:
	rm -rf $(BUILD_DIR)* $(EXE_PREFIX)_* serializer.vcd logs coverage_html *.gcda *.gcno
