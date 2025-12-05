#ifndef TESTBENCH_H
#define TESTBENCH_H

#include "Vdeserializer.h"
#include "verilated_vcd_c.h"

template<class T>
class Testbench {
public:
    T* dut;
    VerilatedVcdC* tfp;
    unsigned long m_tickcount;

    Testbench(void) {
        Verilated::traceEverOn(true);
        dut = new T;
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open("waveform.vcd");
        m_tickcount = 0;
    }

    virtual ~Testbench(void) {
        tfp->close();
        delete dut;
        delete tfp;
    }

    virtual void reset(void) {
        dut->rst_n = 0;
        this->tick(5);
        dut->rst_n = 1;
        dut->eval();
    }

    virtual void tick(int count = 1) {
        for (int i = 0; i < count; i++) {
            dut->clk = 0;
            dut->eval();
            tfp->dump(m_tickcount++);
            dut->clk = 1;
            dut->eval();
            tfp->dump(m_tickcount++);
        }
    }
};

#endif // TESTBENCH_H
