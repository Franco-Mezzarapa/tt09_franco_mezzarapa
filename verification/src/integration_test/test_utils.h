#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <random>

#ifndef DATA_WIDTH
#define DATA_WIDTH 64
#endif
#ifndef KEY_WIDTH
#define KEY_WIDTH 8
#endif

// --- Test Data Generation ---

struct test_data_t {
    std::vector<uint64_t> data;
    std::vector<uint8_t>  keys;
};

inline test_data_t generate_test_data(int num) {
    test_data_t d;
    d.data.resize(num);
    d.keys.resize(num);

    std::random_device rd;
    std::mt19937_64 gen64(rd());
    std::mt19937 gen8(rd());
    
    std::uniform_int_distribution<uint64_t> dist64;
    std::uniform_int_distribution<unsigned int> dist8(0, 255);

    for (int i = 0; i < num; i++) {
        d.data[i] = dist64(gen64);
        d.keys[i] = static_cast<uint8_t>(dist8(gen8));
    }
    return d;
}


// --- Golden Model ---
// This C++ model simulates the entire hardware data path.

// 1. Golden Model for XOR Encryption
inline uint64_t golden_xor_encrypt(uint64_t data, uint8_t key) {
    uint64_t expected_output = 0;
    for (int i = 0; i < (DATA_WIDTH / KEY_WIDTH); ++i) {
        uint8_t data_chunk = (data >> (i * KEY_WIDTH)) & 0xFF;
        uint8_t encrypted_chunk = data_chunk ^ key;
        expected_output |= static_cast<uint64_t>(encrypted_chunk) << (i * KEY_WIDTH);
    }
    return expected_output;
}

// 2. Golden Model for Serialization
inline std::vector<int> golden_serialize(uint64_t data) {
    std::vector<int> serial_bits;
    for (int i = 0; i < DATA_WIDTH; ++i) {
        serial_bits.push_back((data >> (DATA_WIDTH - 1 - i)) & 1);
    }
    return serial_bits;
}

// 3. Top-Level Golden Model
inline std::vector<int> run_golden_model(uint64_t message, uint8_t key) {
    // Step 1: Encrypt the message
    uint64_t encrypted_message = golden_xor_encrypt(message, key);
    // Step 2: Serialize the encrypted message
    std::vector<int> final_serial_stream = golden_serialize(encrypted_message);
    return final_serial_stream;
}


// --- Functional Coverage ---

struct Coverage {
    int end_to_end_pass = 0;
    int end_to_end_fail = 0;
    int key_load_events = 0;
    int msg_load_events = 0;
    int encryption_events = 0;
    int serialization_events = 0;

    void sample_connectivity(bool key, bool msg, bool encrypt, bool serialize) {
        if(key) key_load_events++;
        if(msg) msg_load_events++;
        if(encrypt) encryption_events++;
        if(serialize) serialization_events++;
    }

    void report() {
        printf("\n==== Connectivity Coverage Report ====\n");
        printf("  - Key Deserializer Triggered:      %d times\n", key_load_events);
        printf("  - Message Deserializer Triggered:  %d times\n", msg_load_events);
        printf("  - XOR Encrypt Module Triggered:    %d times\n", encryption_events);
        printf("  - Serializer Module Triggered:     %d times\n", serialization_events);
        
        auto pct = [](int p, int f){ return (p + f) > 0 ? (100.0 * p / (p + f)) : 0.0; };

        printf("\n==== Test Pass/Fail Report ====\n");
        printf("End-to-End Tests Passed: %d\n", end_to_end_pass);
        printf("End-to-End Tests Failed: %d\n", end_to_end_fail);
        printf("Pass Rate:               %.2f%%\n", pct(end_to_end_pass, end_to_end_fail));
        printf("=================================\n");
    }
};

#endif // TEST_UTILS_H