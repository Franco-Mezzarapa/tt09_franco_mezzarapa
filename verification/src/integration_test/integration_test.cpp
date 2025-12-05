#include "integration_test_cases.h"
#include <verilated_cov.h>

// This is the main entry point for the top-level integration test
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Testbench tb;
    Coverage cov;
    
    const int trials = 512; // Fewer trials for the full simulation

    std::cout << "Starting Top-Level Integration Test Suite..." << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    try {
        test_data_t random_data = generate_test_data(trials);

        for (int i = 0; i < trials; ++i) {
            if (i > 0 && i % 10 == 0) {
                std::cout << "  Progress: " << i << "/" << trials << "\r" << std::flush;
            }
            run_end_to_end_test(tb, cov, random_data.data[i], random_data.keys[i]);
        }
        std::cout << "  Progress: " << trials << "/" << trials << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: A test case threw an exception: " << e.what() << std::endl;
        cov.report();
        return 1;
    }

    std::cout << "\nIntegration Test Suite Finished." << std::endl;
    
    cov.report();

    const char* coverage_file = (argc > 1) ? argv[1] : "logs/coverage_top.dat";
    Verilated::mkdir("logs");
    VerilatedCov::write(coverage_file);

    if (cov.end_to_end_fail > 0) {
        std::cerr << "\n" << cov.end_to_end_fail << " end-to-end test(s) failed." << std::endl;
        return 1;
    }
    
    std::cout << "\nAll end-to-end tests passed!" << std::endl;
    return 0;
}
