#ifndef SERIALIZER_TESTS_H
#define SERIALIZER_TESTS_H

#include "testbench.h"
#include "test_utils.h"

// Test case declarations
void test_basic_serialization(Testbench& tb, Coverage& cov, uint64_t test_data);
void test_back_to_back(Testbench& tb, Coverage& cov, uint64_t data1, uint64_t data2);
void test_reset_during_operation(Testbench& tb, Coverage& cov, uint64_t test_data);
void test_edge_patterns(Testbench& tb, Coverage& cov);

#endif // SERIALIZER_TESTS_H
