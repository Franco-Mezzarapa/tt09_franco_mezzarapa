#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <random>
#include <vector>
#include <cstdint>
#include <iostream>

#ifndef DATA_WIDTH
#define DATA_WIDTH 64
#endif
#ifndef KEY_WIDTH
#define KEY_WIDTH 8
#endif

// Test data holding structure
struct test_data_t {
    std::vector<uint64_t> data;
    std::vector<uint8_t>  keys;
};

// Coverage tracking structure for xor_encrypt
struct Coverage {
    // Coverpoints
    int zero_key_hits = 0;
    int ff_key_hits = 0;
    int zero_data_hits = 0;
    int ff_data_hits = 0;
    int random_hits = 0;

    // Test results
    int tests_passed = 0;
    int tests_failed = 0;

    // Sample coverpoints
    void sample(uint64_t data, uint8_t key) {
        if (key == 0x00) zero_key_hits++;
        if (key == 0xFF) ff_key_hits++;
        if (data == 0x00) zero_data_hits++;
        if (data == 0xFFFFFFFFFFFFFFFFULL) ff_data_hits++;
        random_hits++; // Count every call as a random trial
    }

    void report() {
        printf("\n==== Functional Coverage Report ====\n");
        printf("Zero Key (0x00) tested:    %d times\n", zero_key_hits);
        printf("All-Ones Key (0xFF) tested: %d times\n", ff_key_hits);
        printf("Zero Data tested:          %d times\n", zero_data_hits);
        printf("All-Ones Data tested:      %d times\n", ff_data_hits);
        printf("Total random trials:       %d\n", random_hits);
        
        auto pct = [](int p, int f){ return (p + f) > 0 ? (100.0 * p / (p + f)) : 0.0; };

        printf("\n==== Test Pass/Fail Report ====\n");
        printf("Tests Passed: %d\n", tests_passed);
        printf("Tests Failed: %d\n", tests_failed);
        printf("Pass Rate:    %.2f%%\n", pct(tests_passed, tests_failed));
        printf("=================================\n");
    }
};

// Generate random test data
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

// C++ "Golden Model" to calculate the expected XOR result
inline uint64_t calculate_expected_xor(uint64_t data, uint8_t key) {
    uint64_t expected_output = 0;
    for (int i = 0; i < (DATA_WIDTH / KEY_WIDTH); ++i) {
        uint8_t data_chunk = (data >> (i * KEY_WIDTH)) & 0xFF;
        uint8_t encrypted_chunk = data_chunk ^ key;
        expected_output |= static_cast<uint64_t>(encrypted_chunk) << (i * KEY_WIDTH);
    }
    return expected_output;
}

#endif // TEST_UTILS_H
