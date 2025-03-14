#ifndef _CPU
#define _CPU

#include "decode_instruction.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>

typedef union {
    struct {
        // Explanations from:
        // https://dwheeler.com/6502/oneelkruns/asm1step.html

        // Set if the add produced a carry, or if the subtraction
        //  produced a borrow. Also holds bits after a logical shift.
        uint8_t carry : 1;
        // Set if the result of the last operation (load/inc/dec/
        //  add/sub) was zero.
        uint8_t zero : 1;
        // Set if maskable interrupts are disabled.
        uint8_t irq_disable : 1;
        // Set if decimal mode active.
        uint8_t decimal_mode : 1;
        // Set if an interrupt caused by a BRK, reset if
        //  caused by an external interrupt.
        uint8_t brk_command : 1;
        // Unused
        uint8_t : 1;
        // (V flag) Set if the addition of two like-signed numbers or the
        //  subtraction of two unlike-signed numbers produces a result
        //  greater than +127 or less than -128.
        uint8_t overflow : 1;
        // Set if bit 7 of the accumulator is set.
        uint8_t negative : 1;
    };

    uint8_t value;

} CPUStatusRegister;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t a;
    uint8_t stack_pointer;
    uint16_t program_counter;
    CPUStatusRegister status_register;
} CPUContext;

// Performs one CPU cycle
int cpu_tick(CPUContext *ctx, Memory *memory);

#endif
