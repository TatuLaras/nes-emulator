#ifndef _INSTRUCTIONS
#define _INSTRUCTIONS

#include "cpu.h"
#include "decode_instruction.h"
#include "memory.h"

void instruction_run(Instruction instruction, CPUContext *ctx, Memory *memory);

#endif
