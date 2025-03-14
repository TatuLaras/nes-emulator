#include "ppu.h"

uint8_t ppu_read_ppustatus(PPUContext *ppu_ctx) {
    ppu_ctx->write_latch = 0;
    return ppu_ctx->ppustatus.value;
}

void ppu_write_ppuctrl(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppuctrl.value = value;
}

void ppu_write_ppumask(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppumask.value = value;
}
