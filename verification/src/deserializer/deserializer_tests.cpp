#include "deserializer_tests.h"
#include <iostream>

// Helper to get the mask for the current register size
uint64_t get_mask(size_t reg_size) {
    return (reg_size == 64) ? 0xFFFFFFFFFFFFFFFFULL : ((1ULL << reg_size) - 1);
}

void drive_data(Testbench<Vdeserializer>* tb, uint64_t value, size_t reg_size, bool flag, bool ena) {
    tb->dut->iData_flag = flag;
    tb->dut->ena = ena;
    for (size_t b = 0; b < reg_size; b++) {
        tb->dut->iData_in = (value >> (reg_size - 1 - b)) & 0x1;
        tb->tick();
    }
    tb->dut->iData_flag = 0;
    tb->dut->eval();
}

void test_normal_deserialization(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    drive_data(tb, value, reg_size, true, true);
    
    uint64_t dut_val = tb->dut->oData_out & get_mask(reg_size);
    if (tb->dut->oBit_counter == reg_size && dut_val == value) {
        cov.normal_pass++;
    } else {
        cov.normal_fail++;
        std::cerr << "[Normal FAIL] exp=0x" << std::hex << value
                  << " got=0x" << dut_val 
                  << " bits=" << std::dec << tb->dut->oBit_counter << std::endl;
    }
    cov.sample(value, reg_size, true, false, true);
}

void test_all_zeros(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size) {
    std::cout << "Running All Zeros Test..." << std::endl;
    test_normal_deserialization(tb, cov, 0x0, reg_size);
}

void test_all_ones(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size) {
    std::cout << "Running All Ones Test..." << std::endl;
    uint64_t all_ones = get_mask(reg_size);
    test_normal_deserialization(tb, cov, all_ones, reg_size);
}

void test_alternating_pattern(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size) {
    std::cout << "Running Alternating Pattern Test..." << std::endl;
    uint64_t pattern = 0xAAAAAAAAAAAAAAAAULL & get_mask(reg_size);
    test_normal_deserialization(tb, cov, pattern, reg_size);
    pattern = 0x5555555555555555ULL & get_mask(reg_size);
    test_normal_deserialization(tb, cov, pattern, reg_size);
}

void test_overflow(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    drive_data(tb, value, reg_size, true, true);

    // Drive one extra bit
    tb->dut->iData_flag = 1;
    tb->dut->iData_in = 1;
    tb->tick();
    tb->dut->iData_flag = 0;
    tb->dut->eval();

    uint64_t dut_val = tb->dut->oData_out & get_mask(reg_size);
    if (dut_val == value) {
        cov.overflow_pass++;
    } else {
        cov.overflow_fail++;
        std::cerr << "[Overflow FAIL] exp=0x" << std::hex << value
                  << " got=0x" << dut_val << std::dec << std::endl;
    }
    cov.sample(value, reg_size, true, false, true, true);
}

void test_underflow(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    drive_data(tb, value, reg_size, false, true); // Flag is always false

    if (tb->dut->oBit_counter == 0 && (tb->dut->oData_out & get_mask(reg_size)) == 0) {
        cov.underflow_pass++;
    } else {
        cov.underflow_fail++;
        std::cerr << "[Underflow FAIL] bits=" << tb->dut->oBit_counter
                  << " data=0x" << std::hex << (tb->dut->oData_out & get_mask(reg_size)) 
                  << std::dec << std::endl;
    }
    cov.sample(value, reg_size, false, false, true, false, true);
}

void test_partial_transfer(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    tb->dut->iData_flag = 1;
    tb->dut->ena = 1;
    for (size_t b = 0; b < reg_size / 2; b++) {
        tb->dut->iData_in = (value >> (reg_size - 1 - b)) & 0x1;
        tb->tick();
    }
    // Flag drops early
    tb->dut->iData_flag = 0;
    tb->dut->eval();
    
    // Check that the counter stopped
    size_t bits_shifted = tb->dut->oBit_counter;
    tb->tick(); // Let one more cycle pass
    if (tb->dut->oBit_counter != bits_shifted) {
         std::cerr << "[Partial Transfer FAIL] Counter did not stop after flag drop." << std::endl;
    } else {
         std::cout << "[Partial Transfer PASS]" << std::endl;
    }

    cov.sample(value, reg_size, true, false, true, false, false, true);
}


void test_mid_transfer_reset(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    tb->dut->iData_flag = 1;
    tb->dut->ena = 1;

    for (size_t b = 0; b < reg_size / 2; b++) {
        tb->dut->iData_in = (value >> (reg_size - 1 - b)) & 1;
        tb->tick();
    }

    tb->reset();
    cov.reset_hits++;

    if (tb->dut->oBit_counter != 0 || tb->dut->oData_out != 0) {
        std::cerr << "[Mid Reset FAIL] State not cleared properly." << std::endl;
    } else {
        std::cout << "[Mid Reset PASS]" << std::endl;
    }
    cov.sample(value, reg_size, false, true, true);
}

void test_enable_toggle(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size) {
    tb->reset();
    tb->dut->iData_flag = 1;

    // Shift first half with ena=1
    tb->dut->ena = 1;
    for (size_t b = 0; b < reg_size / 2; b++) {
        tb->dut->iData_in = (value >> (reg_size - 1 - b)) & 1;
        tb->tick();
    }
    
    // Shift second half with ena=0 (should be ignored)
    tb->dut->ena = 0;
    for (size_t b = reg_size / 2; b < reg_size; b++) {
        tb->dut->iData_in = (value >> (reg_size - 1 - b)) & 1;
        tb->tick();
    }
    
    if (tb->dut->oBit_counter != reg_size / 2) {
        cov.ena_fail++;
        std::cerr << "[Enable Toggle FAIL] Shifting occurred while ena=0" << std::endl;
    } else {
        cov.ena_pass++;
        std::cout << "[Enable Toggle PASS]" << std::endl;
    }
    cov.sample(value, reg_size, true, false, false); // ena=0 part
    cov.sample(value, reg_size, true, false, true); // ena=1 part
}


void test_back_to_back_frames(Testbench<Vdeserializer>* tb, Coverage& cov, const test_data_t& data, size_t reg_size, int trials) {
    tb->reset();
    for (int i = 0; i < trials; i++) {
        uint64_t value = (reg_size==8) ? data.short_data[i] :
                         (reg_size==32) ? data.medium_data[i] :
                                          data.long_data[i];

        drive_data(tb, value, reg_size, true, true);

        if ((tb->dut->oData_out & get_mask(reg_size)) != value || tb->dut->oBit_counter != reg_size) {
            std::cerr << "[Back-to-Back FAIL] exp=" << std::hex << value
                      << " got=" << (tb->dut->oData_out & get_mask(reg_size))
                      << " bits=" << std::dec << tb->dut->oBit_counter << std::endl;
        }
        tb->reset(); // Reset between frames
    }
     std::cout << "[Back-to-Back PASS]" << std::endl;
}
