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
void instruction_execute(Instruction instruction, CPUContext *ctx,
                         Memory *memory);

// Set status register carry flag.
void sec(CPUContext *ctx);
// Clear status register carry flag.
void clc(CPUContext *ctx);
// Load into A register.
void lda(uint8_t param, CPUContext *ctx);
// Add with carry.
void adc(uint8_t param, CPUContext *ctx);
// Subtract with carry.
void sbc(uint8_t param, CPUContext *ctx);
// AND memory with A register.
void and (uint8_t param, CPUContext *ctx);
// Jump (set program counter) to an `address` either directly given in the
// instruction or the address in a `memory` location pointed to by that
// `address`.
void jmp(uint16_t address, int is_indirect, CPUContext *ctx, Memory *memory);

#endif
