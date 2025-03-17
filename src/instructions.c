#include "instructions.h"
#include "cpu.h"
#include "decode_instruction.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>

// ----- Helpers -----

// Gets 2-byte address (or value) starting at memory address `address`, low byte
// first.
static inline uint16_t read_two_bytes(uint16_t address, Memory *memory) {
    uint8_t low = memory_read(memory, address);
    uint8_t high = memory_read(memory, address + 1);
    return high << 8 | low;
}

// Gets final `memory` location of instruction parameter depending on the
// `addressing_mode`.
static uint16_t get_effective_address(AddressingMode addressing_mode,
                                      uint16_t instruction_address,
                                      CPUContext *ctx, Memory *memory) {
    switch (addressing_mode) {
    case ACCUMULATOR:
    case IMPLIED:
        return 0;

    case IMMEDIATE:
        return instruction_address + 1;
    case ABSOLUTE:
        return read_two_bytes(instruction_address + 1, memory);
    case INDIRECT_ABSOLUTE:
        return read_two_bytes(read_two_bytes(instruction_address + 1, memory),
                              memory);
    case ZERO_PAGE:
        return memory_read(memory, instruction_address + 1);
    case ZERO_PAGE_INDEXED_X:
        return (memory_read(memory, instruction_address + 1) + ctx->x) % 0x100;
    case ZERO_PAGE_INDEXED_Y:
        return (memory_read(memory, instruction_address + 1) + ctx->y) % 0x100;
    case RELATIVE:
        return ctx->program_counter +
               (int8_t)memory_read(memory, instruction_address + 1);
    case ABSOLUTE_INDEXED_X:
        return read_two_bytes(instruction_address + 1, memory) + ctx->x;
    case ABSOLUTE_INDEXED_Y:
        return read_two_bytes(instruction_address + 1, memory) + ctx->y;

    case INDEXED_INDIRECT:
    case INDIRECT_INDEXED:
        fprintf(stderr, "Unsupported 6502 addressing mode %d\n",
                addressing_mode);
        abort();
    }

    return 0;
}

static inline void branch(uint16_t address, CPUContext *ctx) {
    ctx->program_counter = address;
}

static void push_to_stack(uint8_t value, CPUContext *ctx, Memory *memory) {
    memory_write(memory, 0x0100 | ctx->stack_pointer--, value);
}

static uint8_t pull_from_stack(CPUContext *ctx, Memory *memory) {
    return memory_read(memory, 0x0100 | ++ctx->stack_pointer);
}

// Sets flags as if subtraction was carried out
static void compare(uint8_t a, uint8_t b, CPUContext *ctx) {
    CPUContext temp = {.status_register.carry = 1, .a = a};
    sbc(b, &temp);
    ctx->status_register = temp.status_register;
}

void non_maskable_interrupt(CPUContext *ctx, Memory *memory) {
    // Push program counter to stack, high byte first
    push_to_stack(ctx->program_counter >> 8, ctx, memory);
    push_to_stack(ctx->program_counter & 0xff, ctx, memory);

    // Push status register
    php(ctx, memory);

    // Read nmi handler vector
    uint16_t nmi_handler_address =
        memory_read(memory, 0xfffb) << 8 | memory_read(memory, 0xfffa);

    ctx->program_counter = nmi_handler_address;
}

// ----- Instructions -----

void sed(CPUContext *ctx) {
    ctx->status_register.decimal_mode = 1;
}

void sec(CPUContext *ctx) {
    ctx->status_register.carry = 1;
}

void sei(CPUContext *ctx) {
    ctx->status_register.irq_disable = 1;
}

void cld(CPUContext *ctx) {
    ctx->status_register.decimal_mode = 0;
}

void clc(CPUContext *ctx) {
    ctx->status_register.carry = 0;
}

void cli(CPUContext *ctx) {
    ctx->status_register.irq_disable = 0;
}

void lda(uint8_t param, CPUContext *ctx) {
    ctx->a = param;
}

void ldy(uint8_t param, CPUContext *ctx) {
    ctx->y = param;
}

void ldx(uint8_t param, CPUContext *ctx) {
    ctx->x = param;
}

void adc(uint8_t param, CPUContext *ctx) {
    param += ctx->status_register.carry;
    uint8_t result = param + ctx->a;

    // Check if operations overflow for flag setting purposes
    int unsigned_overflow = result < ctx->a || (result == ctx->a && param > 0);

    int signed_overflow = ((int8_t)result >= 0) != ((int8_t)ctx->a >= 0);

    ctx->a = result;

    // Set flags
    ctx->status_register.carry = unsigned_overflow;
    ctx->status_register.overflow = signed_overflow;
    ctx->status_register.zero = result == 0;
    ctx->status_register.negative = (result & 0b10000000) > 0;
}

void sbc(uint8_t param, CPUContext *ctx) {
    adc(~param, ctx);
}

void and (uint8_t param, CPUContext *ctx) {
    ctx->a &= param;
    ctx->status_register.zero = ctx->a == 0;
    ctx->status_register.negative = (ctx->a & 0b10000000) > 0;
}

void jmp(uint16_t address, CPUContext *ctx) {
    ctx->program_counter = address;
}

void txs(CPUContext *ctx) {
    ctx->stack_pointer = ctx->x;
}

void tsx(CPUContext *ctx) {
    ctx->x = ctx->stack_pointer;
}

void tax(CPUContext *ctx) {
    ctx->x = ctx->a;
}

void tay(CPUContext *ctx) {
    ctx->y = ctx->a;
}

void txa(CPUContext *ctx) {
    ctx->a = ctx->x;
}

void tya(CPUContext *ctx) {
    ctx->a = ctx->y;
}

void pha(CPUContext *ctx, Memory *memory) {
    push_to_stack(ctx->a, ctx, memory);
}

void php(CPUContext *ctx, Memory *memory) {
    push_to_stack(ctx->status_register.value | 0b00110000, ctx, memory);
}

void pla(CPUContext *ctx, Memory *memory) {
    ctx->a = pull_from_stack(ctx, memory);
}

void plp(CPUContext *ctx, Memory *memory) {
    ctx->status_register.value = pull_from_stack(ctx, memory);
}

void sta(uint16_t address, CPUContext *ctx, Memory *memory) {
    memory_write(memory, address, ctx->a);
}

void stx(uint16_t address, CPUContext *ctx, Memory *memory) {
    memory_write(memory, address, ctx->x);
}

void sty(uint16_t address, CPUContext *ctx, Memory *memory) {
    memory_write(memory, address, ctx->y);
}

void dex(CPUContext *ctx) {
    ctx->x--;
}

void dey(CPUContext *ctx) {
    ctx->y--;
}

void inx(CPUContext *ctx) {
    ctx->x++;
}

void iny(CPUContext *ctx) {
    ctx->y++;
}

void dec(uint16_t address, Memory *memory) {
    memory_write(memory, address, memory_read(memory, address) - 1);
}

void inc(uint16_t address, Memory *memory) {
    memory_write(memory, address, memory_read(memory, address) + 1);
}

void cmp(uint8_t param, CPUContext *ctx) {
    compare(ctx->a, param, ctx);
}

void cpx(uint8_t param, CPUContext *ctx) {
    compare(ctx->x, param, ctx);
}

void cpy(uint8_t param, CPUContext *ctx) {
    compare(ctx->y, param, ctx);
}

void bit(uint8_t param, CPUContext *ctx) {
    ctx->status_register.negative = (param & 0b10000000) > 0;
    ctx->status_register.overflow = (param & 0b01000000) > 0;
    ctx->status_register.zero = (param & ctx->a) == 0;
}

void bpl(uint16_t address, CPUContext *ctx) {
    if (!ctx->status_register.negative)
        branch(address, ctx);
}

void bne(uint16_t address, CPUContext *ctx) {
    if (!ctx->status_register.zero)
        branch(address, ctx);
}

void rti(CPUContext *ctx, Memory *memory) {
    plp(ctx, memory);

    uint8_t low = pull_from_stack(ctx, memory);
    uint8_t high = pull_from_stack(ctx, memory);

    ctx->program_counter = high << 8 | low;
}

void instruction_execute(Instruction instruction, uint16_t instruction_address,
                         CPUContext *ctx, Memory *memory) {
    uint16_t effective_address = get_effective_address(
        instruction.addressing_mode, instruction_address, ctx, memory);

    // TODO: do this separately in the instructions
    uint8_t param_value = 0;
    if (effective_address != 0x2007)
        param_value = memory_read(memory, effective_address);

    printf("0x%x at 0x%x\n", param_value, effective_address);

    switch (instruction.mneumonic) {
    case SEC:
        sec(ctx);
        break;
    case CLC:
        clc(ctx);
        break;
    case LDA:
        lda(param_value, ctx);
        break;
    case LDX:
        ldx(param_value, ctx);
        break;
    case LDY:
        ldy(param_value, ctx);
        break;
    case ADC:
        adc(param_value, ctx);
        break;
    case SBC:
        sbc(param_value, ctx);
        break;
    case AND:
        and(param_value, ctx);
        break;
    case JMP:
        jmp(effective_address, ctx);
        break;
    case TXS:
        txs(ctx);
        break;
    case PHA:
        pha(ctx, memory);
        break;
    case PLA:
        pla(ctx, memory);
        break;
    case PHP:
        pla(ctx, memory);
        break;
    case CLI:
        cli(ctx);
        break;
    case PLP:
        plp(ctx, memory);
        break;
    case TSX:
        cli(ctx);
        break;
    case SEI:
        sei(ctx);
        break;
    case SED:
        sed(ctx);
        break;
    case CLD:
        cld(ctx);
        break;
    case STA:
        sta(effective_address, ctx, memory);
        break;
    case STX:
        stx(effective_address, ctx, memory);
        break;
    case STY:
        sty(effective_address, ctx, memory);
        break;
    case DEX:
        dex(ctx);
        break;
    case DEY:
        dey(ctx);
        break;
    case INX:
        inx(ctx);
        break;
    case INY:
        iny(ctx);
        break;
    case DEC:
        dec(effective_address, memory);
        break;
    case INC:
        inc(effective_address, memory);
        break;
    case BIT:
        bit(param_value, ctx);
        break;
    case BPL:
        bpl(effective_address, ctx);
        break;
    case BNE:
        bne(effective_address, ctx);
        break;
    case CMP:
        cmp(param_value, ctx);
        break;
    case CPX:
        cpx(param_value, ctx);
        break;
    case CPY:
        cpy(param_value, ctx);
        break;
    case RTI:
        rti(ctx, memory);
        break;

    case BRK:
    case BVC:
    case BVS:
    case CLV:
    case EOR:
    case JSR:
    case LSR:
    case NOP:
    case ORA:
    case ROL:
    case ROR:
    case RTS:
    case TAX:
    case TAY:
    case TXA:
    case TYA:
    case ASL:
    case BCC:
    case BCS:
    case BEQ:
    case BMI:
        fprintf(stderr, "Unsupported 6502 instruction \"%s\"\n",
                instruction.mneumonic_str);
        abort();
        break;
    }
}
