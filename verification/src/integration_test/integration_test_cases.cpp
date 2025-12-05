#include "integration_test_cases.h"
#include <iostream>
#include <vector>

/**
 * @brief Drives a serial bitstream into the DUT.
 * 
 * @param tb The testbench object.
 * @param data The data to serialize.
 * @param num_bits The number of bits to send.
 * @param flag_pin The pin index for the data_flag (1 for key, 2 for message).
 */
void drive_serial_input(Testbench& tb, uint64_t data, int num_bits, int flag_pin) {
    for (int i = 0; i < num_bits; ++i) {
        // Set data bit (MSB first)
        uint8_t current_bit = (data >> (num_bits - 1 - i)) & 1;
        
        // Set ui_in with data bit and flag
        uint8_t ui_in_val = (current_bit & 1) | (1 << flag_pin);
        tb.dut->ui_in = ui_in_val;
        
        tb.tick();
    }
    // Clear flags
    tb.dut->ui_in = 0;
}

/**
 * @brief Runs a full, end-to-end test of the top-level module.
 */
void run_end_to_end_test(Testbench& tb, Coverage& cov, uint64_t message, uint8_t key) {
    tb.reset();
    tb.dut->ena = 1; // Keep DUT enabled

    // --- Phase 1: Deserialize Key ---
    drive_serial_input(tb, key, KEY_WIDTH, 1);
    cov.sample_connectivity(true, false, false, false);

    // --- Phase 2: Deserialize Message ---
    drive_serial_input(tb, message, DATA_WIDTH, 2);
    cov.sample_connectivity(false, true, false, false);

    // --- Phase 3: Wait for Encryption ---
    // The xor_encrypt module is triggered by the deserializer counters.
    // We wait for its status flag (on uo_out[2]) to go high, then low.
    int timeout = 200; // Generous timeout
    while (((tb.dut->uo_out >> 2) & 1) == 0 && timeout > 0) {
        tb.tick();
        timeout--;
    }
    if (timeout > 0) { // Found start of encryption
        cov.sample_connectivity(false, false, true, false);
        while (((tb.dut->uo_out >> 2) & 1) == 1 && timeout > 0) {
            tb.tick();
            timeout--;
        }
    }
    if (timeout <= 0) {
        std::cerr << "[FAIL] Timeout waiting for encryption phase." << std::endl;
        cov.end_to_end_fail++;
        return;
    }
    
    // --- Phase 4: Serialize and Capture Output ---
    // The serializer is triggered by the xor_encrypt counter.
    // We wait for its valid flag (on uo_out[1]) to go high.
    std::vector<int> received_bits;
    timeout = 200; 
    while(received_bits.size() < DATA_WIDTH && timeout > 0) {
        if ((tb.dut->uo_out >> 1) & 1) { // Check for valid flag
            if(received_bits.empty()) { // First valid bit
                cov.sample_connectivity(false, false, false, true);
            }
            received_bits.push_back(tb.dut->uo_out & 1);
        }
        tb.tick();
        timeout--;
    }
    if (timeout <= 0) {
        std::cerr << "[FAIL] Timeout waiting for serialized output." << std::endl;
        cov.end_to_end_fail++;
        return;
    }

    // --- Phase 5: Verify ---
    std::vector<int> expected_bits = run_golden_model(message, key);
    
    bool mismatch = false;
    if (received_bits.size() != expected_bits.size()) {
        mismatch = true;
    } else {
        for (size_t i = 0; i < expected_bits.size(); ++i) {
            if (received_bits[i] != expected_bits[i]) {
                mismatch = true;
                break;
            }
        }
    }

    if (mismatch) {
        cov.end_to_end_fail++;
        std::cerr << "[FAIL] End-to-End test failed for MSG: 0x" << std::hex << message 
                  << ", KEY: 0x" << (unsigned int)key << std::dec << std::endl;
    } else {
        cov.end_to_end_pass++;
    }
}