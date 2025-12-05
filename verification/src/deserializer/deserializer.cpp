#include "deserializer.h"
#include <verilated_cov.h>

#ifndef REG_SIZE
#define REG_SIZE 64
#endif

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Testbench<Vdeserializer>* tb = new Testbench<Vdeserializer>();
    
    Coverage cov;
    
    int trials = 2048; 
    test_data_t data = generate_test_data(trials);

    std::cout << "Running tests for REG_SIZE = " << REG_SIZE << " with " << trials << " random values." << std::endl;

    for (int i = 0; i < trials; ++i) {
        uint64_t test_val = (REG_SIZE == 8)  ? data.short_data[i] :
                            (REG_SIZE == 32) ? data.medium_data[i] :
                                               data.long_data[i];

        // Run single-value tests for each random value
        test_normal_deserialization(tb, cov, test_val, REG_SIZE);
        test_overflow(tb, cov, test_val, REG_SIZE);
        test_underflow(tb, cov, test_val, REG_SIZE);
        test_partial_transfer(tb, cov, test_val, REG_SIZE);
        test_mid_transfer_reset(tb, cov, test_val, REG_SIZE);
        test_enable_toggle(tb, cov, test_val, REG_SIZE);
    }

    // These tests use specific patterns or multiple values
    test_all_zeros(tb, cov, REG_SIZE);
    test_all_ones(tb, cov, REG_SIZE);
    test_alternating_pattern(tb, cov, REG_SIZE);
    test_back_to_back_frames(tb, cov, data, REG_SIZE, trials);
    
    // Final coverage report
    cov.report();

    // Finalize Verilator coverage
    const char* coverage_file = (argc > 1) ? argv[1] : "logs/coverage.dat";
    Verilated::mkdir("logs");
    VerilatedCov::write(coverage_file);

    delete tb;
    return 0;
}
