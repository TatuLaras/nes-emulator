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
    jmp(0x8014, &ctx);
    TEST_ASSERT_EQUAL(0x8014 - 3, ctx.program_counter);

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

void test_store_registers() {
    lda(14, &ctx);
    ldx(17, &ctx);
    ldy(23, &ctx);

    sta(0x01, &ctx, &memory);
    stx(0x06, &ctx, &memory);
    sty(0x0a, &ctx, &memory);

    TEST_ASSERT_EQUAL(14, memory_read(&memory, 0x01));
    TEST_ASSERT_EQUAL(17, memory_read(&memory, 0x06));
    TEST_ASSERT_EQUAL(23, memory_read(&memory, 0x0a));
}

void test_increment_decrement() {
    ldx(17, &ctx);
    ldy(23, &ctx);
    memory_write(&memory, 0x6b, 12);

    dex(&ctx);
    TEST_ASSERT_EQUAL(16, ctx.x);
    dey(&ctx);
    TEST_ASSERT_EQUAL(22, ctx.y);
    dec(0x6b, &memory);
    TEST_ASSERT_EQUAL(11, memory_read(&memory, 0x6b));

    inx(&ctx);
    TEST_ASSERT_EQUAL(17, ctx.x);
    iny(&ctx);
    TEST_ASSERT_EQUAL(23, ctx.y);
    inc(0x6b, &memory);
    TEST_ASSERT_EQUAL(12, memory_read(&memory, 0x6b));
}

void test_bit() {
    bit(0b10000000, &ctx);
    TEST_ASSERT(ctx.status_register.negative);
    TEST_ASSERT_FALSE(ctx.status_register.overflow);

    bit(0b01000000, &ctx);
    TEST_ASSERT_FALSE(ctx.status_register.negative);
    TEST_ASSERT(ctx.status_register.overflow);

    lda(0b00101010, &ctx);
    bit(0b11010101, &ctx);
    TEST_ASSERT(ctx.status_register.zero);
    bit(0b11110101, &ctx);
    TEST_ASSERT_FALSE(ctx.status_register.zero);
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
    RUN_TEST(test_store_registers);
    RUN_TEST(test_increment_decrement);
    RUN_TEST(test_bit);

    return UNITY_END();
}
