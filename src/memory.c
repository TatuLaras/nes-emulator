#include "memory.h"

uint8_t memory_read(Memory *memory, uint16_t address) {
    if (address <= 0x1fff)
        return memory->ram[address % MEMORY_RAM_SIZE];

    if (address >= 0x8000)
        return memory->prg_rom[address - 0x8000];

    return 0;
}

void memory_write(Memory *memory, uint16_t address, char data) {}
