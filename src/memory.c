#include "memory.h"
#include "ppu.h"

uint8_t memory_read(Memory *memory, uint16_t address) {
    if (address <= 0x1fff)
        return memory->ram[address % MEMORY_RAM_SIZE];

    if (address == 0x2002)
        return ppu_read_ppustatus(&memory->ppu_ctx);

    if (address >= 0x8000 && address < 0x8000 + memory->prg_rom_size)
        return memory->prg_rom[address - 0x8000];

    return 0;
}

void memory_write(Memory *memory, uint16_t address, uint8_t data) {
    if (address <= 0x1fff) {
        memory->ram[address % MEMORY_RAM_SIZE] = data;
        return;
    }

    if (address == 0x2000) {
        ppu_write_ppuctrl(data, &memory->ppu_ctx);
        return;
    }
}
