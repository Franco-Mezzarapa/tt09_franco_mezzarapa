#include "serializer_tests.h"
#include <iostream>
#include <vector>

// Helper function to convert a vector of bits (MSB first) to a uint64_t
uint64_t bits_to_uint64(const std::vector<int>& bits) {
    uint64_t num = 0;
    for(int bit : bits) {
        num = (num << 1) | (static_cast<uint64_t>(bit) & 1);
    }
    return num;
}

void test_basic_serialization(Testbench& tb, Coverage& cov, uint64_t test_data) {
    tb.reset();
    
    // Set data and trigger signals
    tb.dut->iData_in = test_data;
    tb.dut->ena = 1;
    tb.dut->iCounter = DATA_WIDTH;
    cov.sample(true, false, false);

    std::vector<int> received_bits;
    // Tick first, then sample
    for (int i = 0; i < DATA_WIDTH; ++i) {
        tb.tick();
        if (tb.dut->oData_flag) {
            received_bits.push_back(tb.dut->oData_out);
        }
    }
    uint64_t received_data = bits_to_uint64(received_bits);

    // De-assert trigger signals
    tb.dut->ena = 0;
    tb.dut->iCounter = 0;
    tb.tick(); // Allow signals to settle

    if (received_data == test_data) {
        cov.basic_pass++;
    } else {
        cov.basic_fail++;
        std::cerr << "[FAIL] Basic Serialization: exp=0x" << std::hex << test_data
                  << " got=0x" << received_data << std::dec << std::endl;
    }
}

void test_back_to_back(Testbench& tb, Coverage& cov, uint64_t data1, uint64_t data2) {
    // --- First Word ---
    tb.reset();
    tb.dut->iData_in = data1;
    tb.dut->ena = 1;
    tb.dut->iCounter = DATA_WIDTH;
    cov.sample(true, false, true); // Hit back-to-back coverpoint

    std::vector<int> bits1;
    for (int i = 0; i < DATA_WIDTH; ++i) {
        tb.tick();
        if (tb.dut->oData_flag) bits1.push_back(tb.dut->oData_out);
    }
    uint64_t received_data1 = bits_to_uint64(bits1);

    // --- Second Word ---
    tb.reset(); // This DUT requires a reset between transfers
    tb.dut->iData_in = data2;
    tb.dut->ena = 1;
    tb.dut->iCounter = DATA_WIDTH;
    cov.sample(true, false, false);

    std::vector<int> bits2;
    for (int i = 0; i < DATA_WIDTH; ++i) {
        tb.tick();
        if (tb.dut->oData_flag) bits2.push_back(tb.dut->oData_out);
    }
    uint64_t received_data2 = bits_to_uint64(bits2);

    // --- Verification ---
    if (received_data1 == data1 && received_data2 == data2) {
        cov.b2b_pass++;
    } else {
        cov.b2b_fail++;
        if (received_data1 != data1) std::cerr << "[FAIL] B2B word1: exp=0x" << std::hex << data1 << " got=0x" << received_data1 << std::dec << std::endl;
        if (received_data2 != data2) std::cerr << "[FAIL] B2B word2: exp=0x" << std::hex << data2 << " got=0x" << received_data2 << std::dec << std::endl;
    }
}

void test_reset_during_operation(Testbench& tb, Coverage& cov, uint64_t test_data) {
    tb.reset();
    tb.dut->iData_in = test_data;
    tb.dut->ena = 1;
    tb.dut->iCounter = DATA_WIDTH;
    
    tb.tick(DATA_WIDTH / 2); // Run for half the time

    // Assert reset in the middle of operation
    tb.reset();
    cov.sample(false, true, false);

    if (tb.dut->oData_flag != 0) {
        cov.reset_fail++;
        std::cerr << "[FAIL] Reset During Op: oData_flag not low after reset." << std::endl;
        return;
    }

    // Ensure it can still serialize correctly after reset
    uint64_t post_reset_data = 0xFFFFFFFFFFFFFFFFULL;
    tb.dut->iData_in = post_reset_data;
    tb.dut->ena = 1;
    tb.dut->iCounter = DATA_WIDTH;

    std::vector<int> received_bits;
    for (int i = 0; i < DATA_WIDTH; ++i) {
        tb.tick();
        if (tb.dut->oData_flag) received_bits.push_back(tb.dut->oData_out);
    }
    uint64_t received_data = bits_to_uint64(received_bits);

    if (received_data == post_reset_data) {
        cov.reset_pass++;
    } else {
        cov.reset_fail++;
        std::cerr << "[FAIL] Reset Post-Op: exp=0x" << std::hex << post_reset_data << " got=0x" << received_data << std::dec << std::endl;
    }
}

void test_edge_patterns(Testbench& tb, Coverage& cov) {
    uint64_t patterns[] = {0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL};
    bool all_pass = true;

    for(int i = 0; i < 4; ++i) {
        tb.reset();
        tb.dut->iData_in = patterns[i];
        tb.dut->ena = 1;
        tb.dut->iCounter = DATA_WIDTH;
        cov.sample(true, false, false, i);

        std::vector<int> received_bits;
        for (int j = 0; j < DATA_WIDTH; ++j) {
            tb.tick();
            if (tb.dut->oData_flag) received_bits.push_back(tb.dut->oData_out);
        }
        uint64_t received_data = bits_to_uint64(received_bits);
        
        if (received_data != patterns[i]) {
            all_pass = false;
            std::cerr << "[FAIL] Edge Pattern: exp=0x" << std::hex << patterns[i]
                      << " got=0x" << received_data << std::dec << std::endl;
        }
    }

    if (all_pass) {
        cov.pattern_pass++;
    } else {
        cov.pattern_fail++;
    }
}