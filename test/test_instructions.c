#include "cpu.h"
#include "instructions.h"
#include "memory.h"
#include "unity.h"
#include <string.h>

CPUContext ctx;
Memory memory;

void setUp() {
    memset(&ctx, 0, sizeof(CPUContext));
    memset(&memory, 0, sizeof(Memory));
}

void tearDown() {}

void test_sec() {
    sec(&ctx);
    TEST_ASSERT(ctx.status_register.carry);
    TEST_ASSERT_FALSE(ctx.status_register.overflow);
    TEST_ASSERT_FALSE(ctx.status_register.decimal_mode);
    TEST_ASSERT_FALSE(ctx.status_register.irq_disable);
    TEST_ASSERT_FALSE(ctx.status_register.brk_command);
    TEST_ASSERT_FALSE(ctx.status_register.negative);
    TEST_ASSERT_FALSE(ctx.status_register.zero);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_sec);

    return UNITY_END();
}
