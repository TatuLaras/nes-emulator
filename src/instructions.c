#include "instructions.h"
#include "cpu.h"
#include "memory.h"

// ----- Instructions start -----

void sec(CPUContext *ctx) {
    ctx->status_register.carry = 1;
}

void clc(CPUContext *ctx) {
    ctx->status_register.carry = 0;
}

void lda(uint8_t param, CPUContext *ctx) {
    ctx->a = param;
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

void jmp(uint16_t address, int is_indirect, CPUContext *ctx, Memory *memory) {
    // If addressing mode INDIRECT_ABSOLUTE is used then the address to jump to
    // is looked up from the address given with the instruction
    if (is_indirect) {
        address = memory_read(memory, address + 1) << 8 |
                  memory_read(memory, address);
    }

    // The program counter will get incremented after this in the execution loop
    // so we counteract that by subtracting the amount that it will be
    // incremented by
    const uint16_t JMP_INSTRUCTION_BYTES = 3;
    ctx->program_counter = address - JMP_INSTRUCTION_BYTES;
}

// ----- Instructions end -----

// Gets a 2-byte value following the instruction (an address in most cases).
static inline uint16_t get_address(CPUContext *ctx, Memory *memory) {
    uint8_t low = memory_read(memory, ctx->program_counter + 1);
    uint8_t high = memory_read(memory, ctx->program_counter + 2);
    return high << 8 | low;
}

// Uses the instruction's addressing mode to get the parameter for the
// instruction.
static uint8_t get_parameter(AddressingMode addressing_mode, CPUContext *ctx,
                             Memory *memory) {
    switch (addressing_mode) {
    case IMMEDIATE:
        return memory_read(memory, ctx->program_counter + 1);

    case INDIRECT_ABSOLUTE:
    case IMPLIED:
        return 0;

    //  TODO:
    case ABSOLUTE:
        return 0;

    case ABSOLUTE_INDEXED_X:
    case ABSOLUTE_INDEXED_Y:
    case ZERO_PAGE:
    case ZERO_PAGE_INDEXED_X:
    case ZERO_PAGE_INDEXED_Y:
    case INDEXED_INDIRECT:
    case INDIRECT_INDEXED:
    case RELATIVE:
    case ACCUMULATOR:
        fprintf(stderr, "Unsupported 6502 addressing mode %d\n",
                addressing_mode);
        abort();
    }

    return 0;
}

void instruction_execute(Instruction instruction, CPUContext *ctx,
                         Memory *memory) {
    uint8_t param = get_parameter(instruction.addressing_mode, ctx, memory);
    printf("0x%x\n", param);

    switch (instruction.mneumonic) {
    case SEC:
        sec(ctx);
        break;
    case CLC:
        clc(ctx);
        break;
    case LDA:
        lda(param, ctx);
        break;
    case ADC:
        adc(param, ctx);
        break;
    case SBC:
        sbc(param, ctx);
        break;
    case AND:
        and(param, ctx);
        break;
    case JMP:
        jmp(get_address(ctx, memory),
            instruction.addressing_mode == INDIRECT_ABSOLUTE, ctx, memory);
        break;

    case ASL:
    case BCC:
    case BCS:
    case BEQ:
    case BIT:
    case BMI:
    case BNE:
    case BPL:
    case BRK:
    case BVC:
    case BVS:
    case CLD:
    case CLI:
    case CLV:
    case CMP:
    case CPX:
    case CPY:
    case DEC:
    case DEX:
    case DEY:
    case EOR:
    case INC:
    case INX:
    case INY:
    case JSR:
    case LDX:
    case LDY:
    case LSR:
    case NOP:
    case ORA:
    case PHA:
    case PHP:
    case PLA:
    case PLP:
    case ROL:
    case ROR:
    case RTI:
    case RTS:
    case SED:
    case SEI:
    case STA:
    case STX:
    case STY:
    case TAX:
    case TAY:
    case TSX:
    case TXA:
    case TXS:
    case TYA:
        fprintf(stderr, "Unsupported 6502 instruction \"%s\"\n",
                instruction.mneumonic_str);
        abort();
        break;
    }
}
