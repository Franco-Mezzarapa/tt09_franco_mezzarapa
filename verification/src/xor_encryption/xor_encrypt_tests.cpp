#include "xor_encrypt_tests.h"
#include <iostream>

/**
 * @brief Runs a single encryption test on the DUT.
 * 
 * @param tb The testbench object.
 * @param cov The coverage object.
 * @param data The 64-bit data to encrypt.
 * @param key The 8-bit key to use for encryption.
 */
void run_single_test(Testbench& tb, Coverage& cov, uint64_t data, uint8_t key) {
    tb.reset();
    
    // Load inputs
    tb.dut->iMessage = data;
    tb.dut->iKey = key;
    
    // Set trigger conditions
    tb.dut->ena = 1;
    tb.dut->iMessage_bit_counter = DATA_WIDTH;
    tb.dut->iKey_bit_counter = KEY_WIDTH;
    
    // Sample coverage
    cov.sample(data, key);

    // The encryption process takes MSG_SIZE / KEY_SIZE cycles for this DUT.
    // Let's wait for the encryption_status flag to go low, with a timeout.
    int timeout = (DATA_WIDTH / KEY_WIDTH) + 10; 
    while (tb.dut->encryption_status == 0 && timeout > 0) {
        tb.tick();
        timeout--;
    }
    
    while (tb.dut->encryption_status == 1 && timeout > 0) {
        tb.tick();
        timeout--;
    }

    if (timeout <= 0) {
        std::cerr << "[FAIL] Timeout waiting for encryption to complete." << std::endl;
        cov.tests_failed++;
        return;
    }
    
    // Get DUT output and expected output
    uint64_t dut_output = tb.dut->oCiphertext;
    uint64_t expected_output = calculate_expected_xor(data, key);
    
    // Compare and report
    if (dut_output == expected_output) {
        cov.tests_passed++;
    } else {
        cov.tests_failed++;
        std::cerr << "[FAIL] Data: 0x" << std::hex << data 
                  << ", Key: 0x" << (unsigned int)key
                  << " | Expected: 0x" << expected_output
                  << ", Got: 0x" << dut_output << std::dec << std::endl;
    }
}

/**
 * @brief Runs a series of specific edge-case tests.
 */
void test_edge_cases(Testbench& tb, Coverage& cov) {
    std::cout << "  Running test with zero key..." << std::endl;
    run_single_test(tb, cov, 0xAAAAAAAAAAAAAAAAULL, 0x00);

    std::cout << "  Running test with 0xFF key..." << std::endl;
    run_single_test(tb, cov, 0x5555555555555555ULL, 0xFF);

    std::cout << "  Running test with zero data..." << std::endl;
    run_single_test(tb, cov, 0x0000000000000000ULL, 0xA5);

    std::cout << "  Running test with 0xFF data..." << std::endl;
    run_single_test(tb, cov, 0xFFFFFFFFFFFFFFFFULL, 0x5A);
}