#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <random>
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>

// Test data holding structure
struct test_data_t {
    std::vector<uint64_t> data;
};

// Coverage tracking structure for serializer
struct Coverage {
    // Coverpoints
    int load_hits = 0;
    int reset_during_op_hits = 0;
    int back_to_back_hits = 0;
    int data_pattern_bins[4] = {0}; // 0:all_zeros, 1:all_ones, 2:alt_a, 3:alt_5

    // Test results
    int basic_pass = 0, basic_fail = 0;
    int b2b_pass = 0, b2b_fail = 0;
    int reset_pass = 0, reset_fail = 0;
    int pattern_pass = 0, pattern_fail = 0;
    
    // Sample coverpoints based on test conditions
    void sample(bool load, bool reset, bool back_to_back, int pattern_bin = -1) {
        if (load) load_hits++;
        if (reset) reset_during_op_hits++;
        if (back_to_back) back_to_back_hits++;
        if (pattern_bin >= 0 && pattern_bin < 4) {
            data_pattern_bins[pattern_bin]++;
        }
    }

    void report() {
        printf("\n==== Functional Coverage Report ====\n");
        printf("Load events: %d\n", load_hits);
        printf("Back-to-back transfer events: %d\n", back_to_back_hits);
        printf("Reset during operation events: %d\n", reset_during_op_hits);
        printf("Data Pattern Bins:\n");
        printf("  - All Zeros: %d\n", data_pattern_bins[0]);
        printf("  - All Ones:  %d\n", data_pattern_bins[1]);
        printf("  - Alt 'A':   %d\n", data_pattern_bins[2]);
        printf("  - Alt '5':   %d\n", data_pattern_bins[3]);
        
        auto pct = [](int p, int f){ return (p + f) > 0 ? (100.0 * p / (p + f)) : 0.0; };

        printf("\n==== Test Pass/Fail Report ====\n");
        printf("Basic Serialization:   %d passed, %d failed (%.2f%% pass rate)\n",
               basic_pass, basic_fail, pct(basic_pass, basic_fail));
        printf("Back-to-Back:          %d passed, %d failed (%.2f%% pass rate)\n",
               b2b_pass, b2b_fail, pct(b2b_pass, b2b_fail));
        printf("Reset During Op:       %d passed, %d failed (%.2f%% pass rate)\n",
               reset_pass, reset_fail, pct(reset_pass, reset_fail));
        printf("Edge Patterns:         %d passed, %d failed (%.2f%% pass rate)\n",
               pattern_pass, pattern_fail, pct(pattern_pass, pattern_fail));
        printf("------------------------------------\n");
        int total_pass = basic_pass + b2b_pass + reset_pass + pattern_pass;
        int total_fail = basic_fail + b2b_fail + reset_fail + pattern_fail;
        printf("Total:                 %d passed, %d failed (%.2f%% pass rate)\n",
               total_pass, total_fail, pct(total_pass, total_fail));
        printf("====================================\n");
    }
};

// Generate 64-bit random test data
inline test_data_t generate_test_data(int num) {
    test_data_t d;
    d.data.resize(num);

    std::random_device rd;
    std::mt19937_64 gen(rd()); // Use 64-bit random number generator
    std::uniform_int_distribution<uint64_t> d64(0, 0xFFFFFFFFFFFFFFFFULL);

    for (int i = 0; i < num; i++) {
        d.data[i] = d64(gen);
    }
    return d;
}

#endif // TEST_UTILS_H

