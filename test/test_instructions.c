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

static void assert_flags_equal(CPUStatusRegister a, CPUStatusRegister b) {
    TEST_ASSERT_EQUAL(a.carry, b.carry);
    TEST_ASSERT_EQUAL(a.negative, b.negative);
    TEST_ASSERT_EQUAL(a.overflow, b.overflow);
    TEST_ASSERT_EQUAL(a.brk_command, b.brk_command);
    TEST_ASSERT_EQUAL(a.decimal_mode, b.decimal_mode);
    TEST_ASSERT_EQUAL(a.irq_disable, b.irq_disable);
    TEST_ASSERT_EQUAL(a.zero, b.zero);
}

void test_sec() {
    sec(&ctx);
    CPUStatusRegister expected = {.carry = 1};
    assert_flags_equal(expected, ctx.status_register);
}

void test_clc() {
    sec(&ctx);
    clc(&ctx);

    CPUStatusRegister expected = {0};
    assert_flags_equal(expected, ctx.status_register);
}

void test_lda() {
    lda(123, &ctx);
    TEST_ASSERT_EQUAL(123, ctx.a);
}

void test_adc() {
    lda(0x12, &ctx);
    adc(0xee, &ctx);

    TEST_ASSERT_EQUAL(0, ctx.a);

    CPUStatusRegister expected = {.carry = 1, .zero = 1};
    assert_flags_equal(expected, ctx.status_register);

    adc(0x43, &ctx);

    TEST_ASSERT_EQUAL(0x44, ctx.a);

    expected.carry = 0;
    expected.zero = 0;
    assert_flags_equal(expected, ctx.status_register);
}

void test_adc_overflow() {
    lda(0x9c, &ctx);
    adc(0x9c, &ctx);

    CPUStatusRegister expected = {.carry = 1, .overflow = 1};
    assert_flags_equal(expected, ctx.status_register);
}

void test_sbc() {
    sec(&ctx);
    lda(0x08, &ctx);
    sbc(0x80, &ctx);

    TEST_ASSERT_EQUAL(0x88, ctx.a);

    CPUStatusRegister expected = {.negative = 1, .overflow = 1};
    assert_flags_equal(expected, ctx.status_register);
}

void test_and() {
    lda(0xb5, &ctx);
    and(0xf0, &ctx);

    TEST_ASSERT_EQUAL(0xb0, ctx.a);

    CPUStatusRegister expected = {.negative = 1};
    assert_flags_equal(expected, ctx.status_register);

    and(0x00, &ctx);

    expected.negative = 0;
    expected.zero = 1;
    assert_flags_equal(expected, ctx.status_register);
}

void test_jmp() {
    // Direct
    jmp(0x8014, 0, &ctx, &memory);
    TEST_ASSERT_EQUAL(0x8014 - 3, ctx.program_counter);

    // Indirect
    memory.ram[0x32] = 0x54;
    memory.ram[0x32 + 1] = 0x80;
    jmp(0x32, 1, &ctx, &memory);
    TEST_ASSERT_EQUAL(0x8054 - 3, ctx.program_counter);

    CPUStatusRegister expected = {0};
    assert_flags_equal(expected, ctx.status_register);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_sec);
    RUN_TEST(test_clc);
    RUN_TEST(test_lda);
    RUN_TEST(test_adc);
    RUN_TEST(test_adc_overflow);
    RUN_TEST(test_sbc);
    RUN_TEST(test_and);
    RUN_TEST(test_jmp);

    return UNITY_END();
}
