#ifndef _DECODE
#define _DECODE

#include <stdint.h>

typedef enum {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE,
    BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP,
    CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY,
    JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA,
    PHP, PLA, PLP, ROL, ROR, RTI, RTS, SBC, SEC,
    SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA,
    TXS, TYA,
} Mneumonic;

typedef enum {
    IMMEDIATE,
    IMPLIED,
    INDIRECT_ABSOLUTE,
    ABSOLUTE,
    ABSOLUTE_INDEXED_X,
    ABSOLUTE_INDEXED_Y,
    ZERO_PAGE,
    ZERO_PAGE_INDEXED_X,
    ZERO_PAGE_INDEXED_Y,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED,
    RELATIVE,
    ACCUMULATOR,
} AddressingMode;

typedef struct {
    Mneumonic mneumonic;
    char *mneumonic_str;
    AddressingMode addressing_mode;
    uint8_t bytes;
    uint8_t cycles;
} Instruction;

Instruction decode_instruction(uint8_t opcode);

#endif
