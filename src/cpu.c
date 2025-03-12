#include "cpu.h"
#include "instructions.h"
#include <stdint.h>
#include <stdio.h>

static void print_cpu_context(CPUContext *ctx) {
    char status[9] = "________\0";
    if (ctx->status_register.negative)
        status[0] = 'N';
    if (ctx->status_register.overflow)
        status[1] = 'V';
    if (ctx->status_register.brk_command)
        status[3] = 'B';
    if (ctx->status_register.decimal_mode)
        status[4] = 'D';
    if (ctx->status_register.irq_disable)
        status[5] = 'I';
    if (ctx->status_register.zero)
        status[6] = 'Z';
    if (ctx->status_register.carry)
        status[7] = 'C';

    printf("SR: %s  SP: 0x%x  PC: 0x%x  X: 0x%x  Y: 0x%x  A: 0x%x\n", status,
           ctx->stack_pointer, ctx->program_counter, ctx->x, ctx->y, ctx->a);
}

int cpu_tick(CPUContext *ctx, Memory *memory) {
    uint8_t opcode = memory_read(memory, ctx->program_counter);

    // We want to exit if it's the BRK instruction / opcode 0
    if (!opcode) {
        printf("BRK instruction, exiting...\n");
        return 1;
    }
    Instruction instruction = decode_instruction(opcode);
    uint16_t instruction_address = ctx->program_counter;

    ctx->program_counter += instruction.bytes;

    printf("\n0x%x %s ", opcode, instruction.mneumonic_str);
    instruction_execute(instruction, instruction_address, ctx, memory);

    print_cpu_context(ctx);

    return 0;
}
