#include "ppu.h"
#include <stdint.h>

static uint8_t ppu_memory_read(uint16_t address, PPUMemory *ppu_memory) {
    if (address < PPU_MEMORY_CARTRIDGE_MAPPED_TOTAL_SIZE)
        return ppu_memory->cartridge_mapped_memory[address];

    // Palettes
    if (address >= 0x3f00 && address <= 0x3f1f)
        return ppu_memory->palette[address % PPU_MEMORY_PALETTE_SIZE];

    return 0;
}

static void ppu_memory_write(uint16_t address, uint8_t value,
                             PPUMemory *ppu_memory) {
    if (address < PPU_MEMORY_CARTRIDGE_MAPPED_TOTAL_SIZE)
        ppu_memory->palette[address] = value;

    // Palettes
    if (address >= 0x3f00 && address <= 0x3f1f)
        ppu_memory->palette[address % PPU_MEMORY_PALETTE_SIZE] = value;
}

// Increment PPU address by 1 or 32 depending on
// `PPUCtrl.increment_mode_vertical`
static inline void increment_ppu_address(PPUContext *ppu_ctx) {
    ppu_ctx->address += (ppu_ctx->ppuctrl.increment_mode_vertical * 0x20) +
                        (!ppu_ctx->ppuctrl.increment_mode_vertical * 1);
}

void ppu_tick(PPUContext *ppu_ctx) {
    return;
}

uint8_t ppu_read_ppustatus(PPUContext *ppu_ctx) {
    ppu_ctx->write_latch = 0;
    return ppu_ctx->ppustatus.value;
}

uint8_t ppu_read_ppudata(PPUContext *ppu_ctx) {
    // Reading data from the PPU is delayed by one read
    uint8_t return_value = ppu_ctx->read_buffer;
    ppu_ctx->read_buffer = ppu_memory_read(ppu_ctx->address, &ppu_ctx->memory);

    increment_ppu_address(ppu_ctx);

    return return_value;
}

void ppu_write_ppuctrl(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppuctrl.value = value;
}

void ppu_write_ppumask(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppumask.value = value;
}

void ppu_write_ppuscroll(uint8_t value, PPUContext *ppu_ctx) {
    if (ppu_ctx->write_latch)
        ppu_ctx->scroll_y = value;
    else
        ppu_ctx->scroll_x = value;

    ppu_ctx->write_latch = !ppu_ctx->write_latch;
}

void ppu_write_ppuaddr(uint8_t value, PPUContext *ppu_ctx) {
    if (ppu_ctx->write_latch)
        ppu_ctx->address = (ppu_ctx->address & 0x00ff) | value << 8;
    else
        ppu_ctx->address = (ppu_ctx->address & 0xff00) | value;

    ppu_ctx->write_latch = !ppu_ctx->write_latch;
}

void ppu_write_ppudata(uint8_t value, PPUContext *ppu_ctx) {
    ppu_memory_write(ppu_ctx->address, value, &ppu_ctx->memory);
    increment_ppu_address(ppu_ctx);
}
