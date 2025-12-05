#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <verilated.h>
#include "Vxor_encrypt.h"
#include "verilated_vcd_c.h"

// Constants
constexpr vluint64_t MAX_SIM_TIME = 50000000; // Increased for many trials

// Testbench class to manage DUT and simulation
class Testbench {
public:
    Vxor_encrypt* dut;
    VerilatedVcdC* tracer;
    vluint64_t main_time;

    Testbench() : main_time(0) {
        Verilated::traceEverOn(true);
        dut = new Vxor_encrypt;
        tracer = new VerilatedVcdC;
        dut->trace(tracer, 99);
        tracer->open("xor_encrypt.vcd");

        // Initialize DUT signals
        dut->clk = 0;
        dut->rst_n = 0; // Active low reset
        dut->ena = 0;
        dut->iMessage = 0;
        dut->iKey = 0;
        dut->iMessage_bit_counter = 0;
        dut->iKey_bit_counter = 0;
    }

    ~Testbench() {
        tracer->close();
        delete dut;
    }

    // Reset the DUT
    void reset() {
        dut->rst_n = 0; // Assert reset
        dut->ena = 0;
        dut->iMessage_bit_counter = 0;
        dut->iKey_bit_counter = 0;
        tick(5);
        dut->rst_n = 1; // De-assert reset
        tick(5);
    }

    // Advance simulation time
    void tick(int n = 1) {
        for (int i = 0; i < n; ++i) {
            if (main_time > MAX_SIM_TIME) {
                throw std::runtime_error("Simulation time exceeded MAX_SIM_TIME");
            }
            dut->clk = 0;
            dut->eval();
            
            dut->clk = 1;
            dut->eval();

            tracer->dump(main_time++);
        }
    }
};

#endif // TESTBENCH_H
