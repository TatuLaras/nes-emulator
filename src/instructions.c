#include "instructions.h"

// ----- Instructions start -----

void sec(CPUContext *ctx) {
    ctx->status_register.carry = 1;
}

void clc(CPUContext *ctx) {
    ctx->status_register.carry = 0;
}

void lda(uint8_t param, CPUContext *ctx, Memory *memory) {
    ctx->a = param;
}

void adc(uint8_t param, CPUContext *ctx) {
    param += ctx->status_register.carry;
    uint8_t unsigned_result = param + ctx->a;
    int8_t result = (int8_t)param + (int8_t)ctx->a;

    // Check if operations overflow for flag setting purposes
    int unsigned_overflow =
        unsigned_result < ctx->a || (unsigned_result == ctx->a && param > 0);

    int signed_overflow =
        (param > 0 && result < ctx->a) || (param < 0 && result > ctx->a);

    ctx->a = result;

    // Set flags
    ctx->status_register.carry = unsigned_overflow;
    ctx->status_register.overflow = signed_overflow;
    ctx->status_register.zero = result == 0;
    ctx->status_register.negative = (result & 0b10000000) > 0;
}

// ----- Instructions end -----

// Uses the instruction's addressing mode to get the parameter for the
// instruction.
static uint8_t get_parameter(AddressingMode addressing_mode, CPUContext *ctx,
                             Memory *memory) {
    switch (addressing_mode) {
    case IMMEDIATE:
        return memory_read(memory, ctx->program_counter + 1);

    case IMPLIED:
        return 0;

    case INDIRECT_ABSOLUTE:
    case ABSOLUTE:
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
        lda(param, ctx, memory);
        break;

    case ADC:
    case AND:
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
    case JMP:
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
    case SBC:
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
        fprintf(stderr, "Unsupported 6502 instruction\n");
        abort();
        break;
    }
}
