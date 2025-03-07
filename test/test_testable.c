#include "testable.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void test_should_be_correct1() { TEST_ASSERT_EQUAL(3, add_numbers(1, 2)); }
void test_should_be_correct2() { TEST_ASSERT_EQUAL(6, add_numbers(4, 2)); }
void test_should_be_correct3() { TEST_ASSERT_EQUAL(3, add_numbers(1, 2)); }
void test_should_be_correct4() { TEST_ASSERT_EQUAL(2, add_numbers(1, 2)); }

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_should_be_correct1);
    RUN_TEST(test_should_be_correct2);
    RUN_TEST(test_should_be_correct3);
    RUN_TEST(test_should_be_correct4);
    return UNITY_END();
}
