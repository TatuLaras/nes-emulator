#include "memory.h"

uint8_t *memory_get_pointer(Memory *memory, uint16_t address) {
    if (address <= 0x1fff)
        return memory->ram + (address % MEMORY_RAM_SIZE);

    if (address >= 0x8000 && address < 0x8000 + memory->prg_rom_size)
        return memory->prg_rom + (address - 0x8000);

    return 0;
}

uint8_t memory_read(Memory *memory, uint16_t address) {
    uint8_t *pointer = memory_get_pointer(memory, address);

    if (!pointer)
        return 0;

    return *pointer;
}

void memory_write(Memory *memory, uint16_t address, char data) {
    if (address <= 0x1fff)
        memory->ram[address % MEMORY_RAM_SIZE] = data;
}
