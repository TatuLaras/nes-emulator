#ifndef _INSTRUCTIONS
#define _INSTRUCTIONS

#include "cpu.h"
#include "decode_instruction.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Executes an `Instruction` updating the `CPUContext` and `Memory`
// appropriately.
void instruction_execute(Instruction instruction, uint16_t instruction_address,
                         CPUContext *ctx, Memory *memory);

// 6502 Instruction set:

// Set status register decimal mode flag.
void sed(CPUContext *ctx);
// Set status register carry flag.
void sec(CPUContext *ctx);
// Set status register interrupt disable flag.
void sei(CPUContext *ctx);
// Clear status register decimal mode flag.
void cld(CPUContext *ctx);
// Clear status register carry flag.
void clc(CPUContext *ctx);
// Clear interrupt disable flag.
void cli(CPUContext *ctx);
// Load into A register.
void lda(uint8_t param, CPUContext *ctx);
// Load into X register.
void ldx(uint8_t param, CPUContext *ctx);
// Load into Y register.
void ldy(uint8_t param, CPUContext *ctx);
// Add with carry.
void adc(uint8_t param, CPUContext *ctx);
// Subtract with carry.
void sbc(uint8_t param, CPUContext *ctx);
// AND memory with A register.
void and (uint8_t param, CPUContext *ctx);
// Set program counter to `address`.
void jmp(uint16_t address, CPUContext *ctx);
// Transfer X -> stack pointer
void txs(CPUContext *ctx);
// Transfer stack pointer -> X
void tsx(CPUContext *ctx);
// Transfer accumulator -> X
void tax(CPUContext *ctx);
// Transfer accumulator -> Y
void tay(CPUContext *ctx);
// Transfer X -> accumulator
void txa(CPUContext *ctx);
// Transfer Y -> accumulator
void tya(CPUContext *ctx);
// Push A register to stack
void pha(CPUContext *ctx, Memory *memory);
// Push status register to stack with bit 5 and break flag set.
void php(CPUContext *ctx, Memory *memory);
// Pull A register from stack
void pla(CPUContext *ctx, Memory *memory);
// Pull status register from stack.
void plp(CPUContext *ctx, Memory *memory);
// Store A register in memory
void sta(uint16_t address, CPUContext *ctx, Memory *memory);
// Store X register in memory
void stx(uint16_t address, CPUContext *ctx, Memory *memory);
// Store Y register in memory
void sty(uint16_t address, CPUContext *ctx, Memory *memory);
// Decrement X register
void dex(CPUContext *ctx);
// Decrement Y register
void dey(CPUContext *ctx);
// Increment X register
void inx(CPUContext *ctx);
// Increment Y register
void iny(CPUContext *ctx);
// Decrement memory
void dec(uint16_t address, Memory *memory);
// Increment memory
void inc(uint16_t address, Memory *memory);
// Test bits
void bit(uint8_t param, CPUContext *ctx);
// Branch on status register flag negative == 0
void bpl(uint16_t address, CPUContext *ctx);
// Branch on status register flag zero == 0
void bne(uint16_t address, CPUContext *ctx);
// Compare memory with A register
void cmp(uint8_t param, CPUContext *ctx);
// Compare memory with X register
void cpx(uint8_t param, CPUContext *ctx);
// Compare memory with Y register
void cpy(uint8_t param, CPUContext *ctx);

#endif
