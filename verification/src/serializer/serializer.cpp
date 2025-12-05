#include "serializer.h"
#include <verilated_cov.h>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Testbench tb;
    Coverage cov;
    
    // Number of randomized trials to run
    const int trials = 2048;

    std::cout << "Starting Serializer Test Suite (64-bit, " << trials << " trials)..." << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    try {
        // Generate random data for the trials
        test_data_t random_data = generate_test_data(trials);

        // --- Randomized Trials ---
        std::cout << "Running Randomized Trials..." << std::endl;
        for (int i = 0; i < trials; ++i) {
            if (i > 0 && i % 100 == 0) {
                std::cout << "  Progress: " << i << "/" << trials << "\r" << std::flush;
            }

            uint64_t val1 = random_data.data[i];
            uint64_t val2 = random_data.data[(i + 1) % trials]; // Wrap around for the last element

            // Run tests for each random value
            test_basic_serialization(tb, cov, val1);
            test_reset_during_operation(tb, cov, val1);
            test_back_to_back(tb, cov, val1, val2);
        }
        std::cout << "  Progress: " << trials << "/" << trials << std::endl;
        
        // --- Fixed Pattern Tests ---
        std::cout << "Running Fixed Pattern Tests..." << std::endl;
        test_edge_patterns(tb, cov);
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: A test case threw an exception: " << e.what() << std::endl;
        cov.report(); // Report what we have so far
        return 1;
    }

    std::cout << "\nTest Suite Finished." << std::endl;
    
    // Final functional coverage report
    cov.report();

    // Finalize Verilator coverage
    const char* coverage_file = (argc > 1) ? argv[1] : "logs/coverage.dat";
    Verilated::mkdir("logs");
    VerilatedCov::write(coverage_file);

    // Determine exit code based on failures
    if (cov.basic_fail > 0 || cov.b2b_fail > 0 || cov.reset_fail > 0 || cov.pattern_fail > 0) {
        std::cerr << "One or more tests failed." << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}