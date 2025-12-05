#include "xor_encrypt.h"
#include <verilated_cov.h>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Testbench tb;
    Coverage cov;
    
    // Number of randomized trials to run
    const int trials = 2048;

    std::cout << "Starting XOR Encrypt Test Suite..." << std::endl;
    std::cout << "------------------------------------" << std::endl;

    try {
        // --- Fixed Edge-Case Tests ---
        std::cout << "Running Edge-Case Tests..." << std::endl;
        test_edge_cases(tb, cov);

        // --- Randomized Trials ---
        std::cout << "\nRunning " << trials << " Randomized Trials..." << std::endl;
        test_data_t random_data = generate_test_data(trials);

        for (int i = 0; i < trials; ++i) {
            if (i > 0 && i % 100 == 0) {
                std::cout << "  Progress: " << i << "/" << trials << "\r" << std::flush;
            }
            run_single_test(tb, cov, random_data.data[i], random_data.keys[i]);
        }
        std::cout << "  Progress: " << trials << "/" << trials << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: A test case threw an exception: " << e.what() << std::endl;
        cov.report(); // Report what we have so far
        return 1;
    }

    std::cout << "\nTest Suite Finished." << std::endl;
    
    // Final reports
    cov.report();

    // Finalize Verilator coverage
    const char* coverage_file = (argc > 1) ? argv[1] : "logs/coverage.dat";
    Verilated::mkdir("logs");
    VerilatedCov::write(coverage_file);

    // Determine exit code based on failures
    if (cov.tests_failed > 0) {
        std::cerr << "\n" << cov.tests_failed << " test(s) failed." << std::endl;
        return 1;
    }
    
    std::cout << "\nAll " << cov.tests_passed << " tests passed!" << std::endl;
    return 0;
}
