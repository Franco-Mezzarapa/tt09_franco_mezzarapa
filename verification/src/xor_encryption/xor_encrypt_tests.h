#ifndef XOR_ENCRYPT_TESTS_H
#define XOR_ENCRYPT_TESTS_H

#include "testbench.h"
#include "test_utils.h"

// Test case declarations
void run_single_test(Testbench& tb, Coverage& cov, uint64_t data, uint8_t key);
void test_edge_cases(Testbench& tb, Coverage& cov);

#endif // XOR_ENCRYPT_TESTS_H
