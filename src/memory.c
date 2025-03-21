#include "memory.h"
#include "ppu.h"
#include <stdio.h>
#include <stdlib.h>

// To abort with an error message if a non-implemented memory address is read or
// written:
// #define _STRICT_READ
#define _STRICT_WRITE

uint8_t memory_read(Memory *memory, uint16_t address) {
    if (address <= 0x1fff)
        return memory->ram[address % MEMORY_RAM_SIZE];

    if (address == 0x2002)
        return ppu_read_ppustatus(&memory->ppu_ctx);
    if (address == 0x2004)
        return ppu_read_oamdata(&memory->ppu_ctx);
    if (address == 0x2007)
        return ppu_read_ppudata(&memory->ppu_ctx);

    if (address >= 0x8000 && address < 0x8000 + memory->prg_rom_size)
        return memory->prg_rom[address - 0x8000];

#ifdef _STRICT_READ
    fprintf(stderr, "Out of bounds memory read 0x%x\n", address);
    abort();
#endif
    return 0;
}

void memory_write(Memory *memory, uint16_t address, uint8_t data) {
    if (address >= 0x0200 && address <= 0x02ff)
        asm("int $3");

    if (address <= 0x1fff) {
        memory->ram[address % MEMORY_RAM_SIZE] = data;
        return;
    }

    if (address == 0x2000) {
        ppu_write_ppuctrl(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2001) {
        ppu_write_ppumask(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2003) {
        ppu_write_oamaddr(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2004) {
        ppu_write_oamdata(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2005) {
        ppu_write_ppuscroll(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2006) {
        ppu_write_ppuaddr(data, &memory->ppu_ctx);
        return;
    }
    if (address == 0x2007) {
        ppu_write_ppudata(data, &memory->ppu_ctx);
        return;
    }

    // OAMDMA
    if (address == 0x4014) {
        for (uint16_t i = 0; i < 0x100; i++) {
            ppu_write_oamdata(memory_read(memory, data << 8 | i),
                              &memory->ppu_ctx);
        }
        return;
    }

    if (address == 0x4017) {
        printf("TODO: register 0x4017\n");
        return;
    }

    if (address == 0x4010) {
        printf("TODO: register 0x4017\n");
        return;
    }

#ifdef _STRICT_WRITE
    fprintf(stderr, "Out of bounds memory write 0x%x\n", address);
    abort();
#endif
}
