# Makefile for deserializer testbench (Verilator)

# Paths
TOP         := deserializer
RTL         := ../../rtl/deserializer.v
TB_SRC      := deserializer.cpp deserializer_tests.cpp
BUILD_DIR   := obj_dir_deser
EXE_PREFIX  := sim_deserializer

# Allow JOBS to be passed from parent Makefile, default to 1
JOBS ?= 1

# Verilator flags
VERILATOR_FLAGS := --cc --exe --assert --coverage --trace -Wall -O3 -Wno-WIDTHEXPAND --top-module $(TOP) --output-split 20

.PHONY: all run clean build8 build32 build64 run8 run32 run64 coverage

all: build8 build64

# Build for MSG_SIZE=8
build8:
	@echo "Building for MSG_SIZE=8"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR)8 -GMSG_SIZE=8 $(RTL) $(TB_SRC) -CFLAGS "-DREG_SIZE=8"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR)8 -f V$(TOP).mk
	@cp $(BUILD_DIR)8/V$(TOP) $(EXE_PREFIX)_8

# Build for MSG_SIZE=32
build32:
	@echo "Building for MSG_SIZE=32"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR)32 -GMSG_SIZE=32 $(RTL) $(TB_SRC) -CFLAGS "-DREG_SIZE=32"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR)32 -f V$(TOP).mk
	@cp $(BUILD_DIR)32/V$(TOP) $(EXE_PREFIX)_32

# Build for MSG_SIZE=64
build64:
	@echo "Building for MSG_SIZE=64"
	@verilator $(VERILATOR_FLAGS) --Mdir $(BUILD_DIR)64 -GMSG_SIZE=64 $(RTL) $(TB_SRC) -CFLAGS "-DREG_SIZE=64"
	@$(MAKE) -j$(JOBS) -C $(BUILD_DIR)64 -f V$(TOP).mk
	@cp $(BUILD_DIR)64/V$(TOP) $(EXE_PREFIX)_64


run8: build8
	@echo "Running for MSG_SIZE=8"
	@mkdir -p logs
	@./$(EXE_PREFIX)_8 logs/coverage_8.dat

run32: build32
	@echo "Running for MSG_SIZE=32"
	@mkdir -p logs
	@./$(EXE_PREFIX)_32 logs/coverage_32.dat

run64: build64
	@echo "Running for MSG_SIZE=64"
	@mkdir -p logs
	@./$(EXE_PREFIX)_64 logs/coverage_64.dat

run: run8 run64

# Coverage reporting
coverage: run
	@echo "Generating LCOV info file..."
	@verilator_coverage --write-info logs/coverage.info logs/coverage_*.dat
	@echo "Generating HTML report..."
	@genhtml logs/coverage.info --output-directory coverage_html --ignore-errors source --synthesize-missing

clean:
	rm -rf $(BUILD_DIR)* $(EXE_PREFIX)_* waveform.vcd logs coverage_merged.dat coverage_html
	rm -f *.gcda *.gcno
