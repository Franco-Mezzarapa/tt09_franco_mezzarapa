#ifndef DESERIALIZER_TESTS_H
#define DESERIALIZER_TESTS_H

#include "testbench.h"
#include "test_utils.h"

void test_normal_deserialization(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_all_zeros(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size);
void test_all_ones(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size);
void test_alternating_pattern(Testbench<Vdeserializer>* tb, Coverage& cov, size_t reg_size);
void test_overflow(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_underflow(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_partial_transfer(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_mid_transfer_reset(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_enable_toggle(Testbench<Vdeserializer>* tb, Coverage& cov, uint64_t value, size_t reg_size);
void test_back_to_back_frames(Testbench<Vdeserializer>* tb, Coverage& cov, const test_data_t& data, size_t reg_size, int trials);

#endif // DESERIALIZER_TESTS_H
