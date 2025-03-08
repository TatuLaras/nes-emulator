#include "instructions.h"
#include "cpu.h"

void sec(CPUContext *ctx) {
    ctx->status_register.carry = 1;
}

void clc(CPUContext *ctx) {
    ctx->status_register.carry = 0;
}

void instruction_run(Instruction instruction, CPUContext *ctx, Memory *memory) {
    switch (instruction.mneumonic) {
    case SEC:
        sec(ctx);
        break;
    case CLC:
        clc(ctx);
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
    case LDA:
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
        break;
    }
}
