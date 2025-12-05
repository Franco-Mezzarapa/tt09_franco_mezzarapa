#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <verilated.h>
#include "Vtt_um_franco_mezzarapa.h"
#include "verilated_vcd_c.h"

// Constants
constexpr vluint64_t MAX_SIM_TIME = 50000000;

// Testbench class to manage top-level DUT and simulation
class Testbench {
public:
    Vtt_um_franco_mezzarapa* dut;
    VerilatedVcdC* tracer;
    vluint64_t main_time;

    Testbench() : main_time(0) {
        Verilated::traceEverOn(true);
        dut = new Vtt_um_franco_mezzarapa;
        tracer = new VerilatedVcdC;
        dut->trace(tracer, 99);
        tracer->open("top_level.vcd");

        // Initialize DUT signals
        dut->clk = 0;
        dut->rst_n = 0; // Active low reset
        dut->ena = 0;
        dut->ui_in = 0;
    }

    ~Testbench() {
        tracer->close();
        delete dut;
    }

    // Reset the DUT
    void reset() {
        dut->rst_n = 0; // Assert reset
        dut->ena = 0;
        dut->ui_in = 0;
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
