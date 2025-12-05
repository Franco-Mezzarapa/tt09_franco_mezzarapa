#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <random>
#include <vector>
#include <cstdint>
#include <iostream>

// Test data holding structure
struct test_data_t {
    std::vector<uint8_t>  short_data;
    std::vector<uint32_t> medium_data;
    std::vector<uint64_t> long_data;
};

// Coverage tracking structure
struct Coverage {
    int width_bins[3] = {0};  
    int reset_hits = 0;
    int ena_hits[2] = {0}; // 0 for disabled, 1 for enabled
    int flag_active_hits = 0;
    int overflow_hits = 0;
    int underflow_hits = 0;
    int partial_flag_hits = 0;
    std::vector<uint64_t> data_samples;

    int normal_pass = 0, normal_fail = 0;
    int overflow_pass = 0, overflow_fail = 0;
    int underflow_pass = 0, underflow_fail = 0;
    int ena_pass = 0, ena_fail = 0;

    void sample(uint64_t value, size_t width, bool flag, bool reset, bool ena,
                bool overflow=false, bool underflow=false, bool partial=false)
    {
        if(width == 8) width_bins[0]++;
        else if(width == 32) width_bins[1]++;
        else width_bins[2]++;

        if(flag) flag_active_hits++;
        if(reset) reset_hits++;
        if(overflow) overflow_hits++;
        if(underflow) underflow_hits++;
        if(partial) partial_flag_hits++;
        ena_hits[ena ? 1:0]++;

        data_samples.push_back(value);
    }

    void report() {
        printf("\n==== Functional Coverage Report ====\n");
        printf("Width coverage:  8-bit=%d, 32-bit=%d, 64-bit=%d\n",
               width_bins[0], width_bins[1], width_bins[2]);
        printf("Enable (ena) coverage: disabled=%d, enabled=%d\n", ena_hits[0], ena_hits[1]);
        printf("Flag active hits: %d\n", flag_active_hits);
        printf("Partial flag hits: %d\n", partial_flag_hits);
        printf("Reset hits: %d\n", reset_hits);
        printf("Overflow events: %d\n", overflow_hits);
        printf("Underflow events: %d\n", underflow_hits);
        printf("Total samples: %zu\n", data_samples.size());

        auto pct = [](int p, int f){ return (p+f) ? (100.0*p/(p+f)) : 0.0; };

        printf("\n==== Test Pass Rates ====\n");
        printf("Normal:         %d/%d (%.2f%%)\n", normal_pass,
               normal_pass + normal_fail, pct(normal_pass, normal_fail));
        printf("Overflow:       %d/%d (%.2f%%)\n", overflow_pass,
               overflow_pass + overflow_fail, pct(overflow_pass, overflow_fail));
        printf("Underflow:      %d/%d (%.2f%%)\n", underflow_pass,
               underflow_pass + underflow_fail, pct(underflow_pass, underflow_fail));
        printf("Enable toggle:  %d/%d (%.2f%%)\n", ena_pass,
               ena_pass + ena_fail, pct(ena_pass, ena_fail));
        printf("====================================\n");
    }
};

// Generate 8, 32, and 64-bit random test data
inline test_data_t generate_test_data(int num)
{
    test_data_t d;
    d.short_data.resize(num);
    d.medium_data.resize(num);
    d.long_data.resize(num);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint8_t>  d8(0,255);
    std::uniform_int_distribution<uint32_t> d32(0,0xFFFFFFFF);
    std::uniform_int_distribution<uint64_t> d64(0,0xFFFFFFFFFFFFFFFFULL);

    for(int i=0;i<num;i++)
    {
        d.short_data[i] = d8(gen);
        d.medium_data[i] = d32(gen);
        d.long_data[i]  = d64(gen);
    }
    return d;
}

#endif // TEST_UTILS_H
