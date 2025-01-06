# defaults
SIM ?= icarus
TOPLEVEL_LANG ?= verilog
SRC_DIR := $(PWD)/../src

# Corrected paths for the Verilog sources using SRC_DIR
VERILOG_SOURCES = $(SRC_DIR)/xor_encrypt.v $(SRC_DIR)/tt_um_franco_mezzarapa.v $(SRC_DIR)/serializer.v $(SRC_DIR)/deserializer.v $(SRC_DIR)/shift_register.v

# use VHDL_SOURCES for VHDL files

# TOPLEVEL is the name of the toplevel module in your Verilog or VHDL file
TOPLEVEL = tt_um_franco_mezzarapa

# MODULE is the basename of the Python test file
MODULE = test

# include cocotb's make rules to take care of the simulator setup
include $(shell cocotb-config --makefiles)/Makefile.sim
