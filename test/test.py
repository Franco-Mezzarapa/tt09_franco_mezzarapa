import cocotb
from cocotb.clock import Clock
from cocotb.triggers import RisingEdge, FallingEdge, Timer

# Constants
MSG_SIZE = 64
KEY_SIZE = 8
DEBUG_SIZE = 24
CLOCK_PERIOD_NS = 100  # Clock period in nanoseconds (10 MHz)

@cocotb.test()
async def test_tt_um_franco_mezzarapa(dut):
    """
    Testbench for tt_um_franco_mezzarapa.
    """

    # Clock generation
    cocotb.start_soon(Clock(dut.clk, CLOCK_PERIOD_NS // 2, units="ns").start())

    # Initialize signals
    dut.rst_n.value = 1
    dut.ena.value = 0
    dut.ui_in.value = 0
    dut.uio_in.value = 0

    # Define key and message
    key = 0xA5  # Example 8-bit key
    message = 0xA3B1F9D2E7C6A594  # Example 64-bit message
    ciphertext = 0
    rebuilt_ciphertext = 0
    rebuilt_debug = 0

    # Perform 101 resets
    for _ in range(101):
        dut.rst_n.value = 0
        await Timer(CLOCK_PERIOD_NS, units="ns")
        dut.rst_n.value = 1
        await Timer(CLOCK_PERIOD_NS, units="ns")

    # Enable the module
    dut.ena.value = 1

    # Load the key
    dut.ui_in.value = 0b10  # Set key loading flag
    for i in range(KEY_SIZE):
        dut.ui_in.value = (key >> (KEY_SIZE - 1 - i)) & 0x1
        await Timer(CLOCK_PERIOD_NS, units="ns")
    dut.ui_in.value = 0

    # Wait one cycle
    await Timer(CLOCK_PERIOD_NS, units="ns")

    # Set control inputs
    dut.ui_in.value = 0b10010  # Set ui_in[4] = 1 for Reset Active
    await Timer(CLOCK_PERIOD_NS, units="ns")

    # Load the message
    dut.ui_in.value = 0b100  # Set message loading flag
    for i in range(MSG_SIZE):
        dut.ui_in.value = (message >> (MSG_SIZE - 1 - i)) & 0x1
        await Timer(CLOCK_PERIOD_NS, units="ns")
    dut.ui_in.value = 0

    # Wait for ciphertext output to be ready
    while not dut.uo_out.value & 0b10:  # Assuming uo_out[1] is the ready flag
        await RisingEdge(dut.clk)

    # Capture the ciphertext serially
    rebuilt_ciphertext = 0
    for i in range(MSG_SIZE):
        await RisingEdge(dut.clk)
        rebuilt_ciphertext = (rebuilt_ciphertext << 1) | int(dut.uo_out.value & 0x1)

    # Capture the debug output serially
    rebuilt_debug = 0
    for i in range(DEBUG_SIZE):
        await RisingEdge(dut.clk)
        rebuilt_debug = (rebuilt_debug << 1) | int(dut.uo_out.value >> 7)

    # Compute expected ciphertext
    for i in range(MSG_SIZE // KEY_SIZE):
        ciphertext |= ((message >> (i * KEY_SIZE)) & 0xFF) ^ key << (i * KEY_SIZE)

    # Print results
    dut._log.info(f"Key: {key:02X}")
    dut._log.info(f"Message: {message:016X}")
    dut._log.info(f"Computed Ciphertext: {ciphertext:016X}")
    dut._log.info(f"Rebuilt Ciphertext: {rebuilt_ciphertext:016X}")
    dut._log.info(f"Debug Output (24 bits): {rebuilt_debug:06X}")

    # Assertions
    assert rebuilt_ciphertext == ciphertext, "Ciphertext does not match expected result."
