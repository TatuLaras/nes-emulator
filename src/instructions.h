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

// Set status register carry flag
void sec(CPUContext *ctx);
// Clear status register carry flag
void clc(CPUContext *ctx);
// Load into A register
void lda(uint8_t param, CPUContext *ctx, Memory *memory);
// Add with carry
void adc(uint8_t param, CPUContext *ctx);

#endif
