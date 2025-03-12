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

void test_flag_setting() {
    sed(&ctx);
    sec(&ctx);
    sei(&ctx);

    CPUStatusRegister expected = {
        .carry = 1, .irq_disable = 1, .decimal_mode = 1};
    assert_flags_equal(expected, ctx.status_register);

    cld(&ctx);
    clc(&ctx);
    cli(&ctx);

    expected.value = 0;
    assert_flags_equal(expected, ctx.status_register);
}

void test_load_register() {
    lda(123, &ctx);
    ldx(174, &ctx);
    ldy(28, &ctx);

    TEST_ASSERT_EQUAL(123, ctx.a);
    TEST_ASSERT_EQUAL(174, ctx.x);
    TEST_ASSERT_EQUAL(28, ctx.y);
}

void test_register_transfers() {
    lda(143, &ctx);

    tax(&ctx);
    TEST_ASSERT_EQUAL(143, ctx.x);
    tay(&ctx);
    TEST_ASSERT_EQUAL(143, ctx.y);

    ldx(21, &ctx);
    txa(&ctx);
    TEST_ASSERT_EQUAL(21, ctx.a);
    txs(&ctx);
    TEST_ASSERT_EQUAL(21, ctx.stack_pointer);

    ldy(53, &ctx);
    tya(&ctx);
    TEST_ASSERT_EQUAL(53, ctx.a);

    ctx.stack_pointer = 83;
    tsx(&ctx);
    TEST_ASSERT_EQUAL(83, ctx.x);
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

void test_stack_instructions() {
    // void php(CPUContext *ctx, Memory *memory);
    // void plp(CPUContext *ctx, Memory *memory);

    // A register

    // Push
    lda(4, &ctx);
    pha(&ctx, &memory);
    lda(5, &ctx);
    pha(&ctx, &memory);
    lda(6, &ctx);
    pha(&ctx, &memory);
    lda(7, &ctx);
    pha(&ctx, &memory);

    lda(0, &ctx);

    // Pull

    pla(&ctx, &memory);
    TEST_ASSERT_EQUAL(ctx.a, 7);
    pla(&ctx, &memory);
    TEST_ASSERT_EQUAL(ctx.a, 6);
    pla(&ctx, &memory);
    TEST_ASSERT_EQUAL(ctx.a, 5);
    pla(&ctx, &memory);
    TEST_ASSERT_EQUAL(ctx.a, 4);

    // Status register
    ctx.status_register.value = 0b10001010;
    php(&ctx, &memory);
    ctx.status_register.value = 0b00000101;
    php(&ctx, &memory);

    ctx.status_register.value = 0;

    // Bits 4 and 5 get set by the php instruction
    plp(&ctx, &memory);
    TEST_ASSERT_EQUAL(0b00110101, ctx.status_register.value);
    plp(&ctx, &memory);
    TEST_ASSERT_EQUAL(0b10111010, ctx.status_register.value);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_flag_setting);
    RUN_TEST(test_load_register);
    RUN_TEST(test_adc);
    RUN_TEST(test_adc_overflow);
    RUN_TEST(test_sbc);
    RUN_TEST(test_and);
    RUN_TEST(test_jmp);
    RUN_TEST(test_register_transfers);
    RUN_TEST(test_stack_instructions);

    return UNITY_END();
}
