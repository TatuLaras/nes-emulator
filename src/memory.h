// Handles read/write into RAM and read from ROM
// Also redirects memory-mapped I/O

#ifndef _MEMORY
#define _MEMORY

#include "ppu.h"
#include <stdint.h>
#define MEMORY_RAM_SIZE 0x800
#define MEMORY_TRAINER_SIZE 0x200

typedef struct {
    uint8_t ram[MEMORY_RAM_SIZE];
    uint8_t trainer[MEMORY_TRAINER_SIZE];
    // Contains game code, no fixed size
    uint8_t *prg_rom;
    int prg_rom_size;
    // Contains sprites, no fixed size
    uint8_t *chr_rom;
    int chr_rom_size;

    //  NOTE: Makes the most sense to have this here since PPU is only
    //  controlled through memory-mapped I/O. This prevents us from having to
    //  pass PPUContext as a parameter everywhere.
    PPUContext ppu_ctx;
} Memory;

uint8_t memory_read(Memory *memory, uint16_t address);
void memory_write(Memory *memory, uint16_t address, uint8_t data);

#endif
