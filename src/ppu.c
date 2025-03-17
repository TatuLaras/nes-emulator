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

// Returns 4-bit pixel retrieved from `pattern_table`.
static uint8_t get_pattern_table_pixel(uint8_t *pattern_table,
                                       uint8_t tile_number, uint8_t x_offset,
                                       uint8_t y_offset) {
    uint16_t index = tile_number * 16;
    assert(index < PPU_MEMORY_PATTERN_TABLE_SIZE / 16);

    uint8_t plane0_byte = pattern_table[index + y_offset];
    uint8_t plane1_byte = pattern_table[index + y_offset + 8];

    uint8_t plane0_dot = (plane0_byte & (1 << (7 - x_offset))) > 0;
    uint8_t plane1_dot = (plane1_byte & (1 << (7 - x_offset))) > 0;

    // if (y_offset == 3)
    //     asm("int $3");

    return (plane1_dot * 0b10) | (plane0_dot * 0b01);
}

// Returns a 4-bit sprite pixel if there is one at the current dot
static uint8_t evaluate_sprites(PPUContext *ppu_ctx) {
    //  TODO: 8x16 mode
    assert(!ppu_ctx->ppuctrl.sprite_mode_8x16);

    OAMEntry *oam_entries = (OAMEntry *)ppu_ctx->oam;
    uint8_t sprite_height = 8 + (ppu_ctx->ppuctrl.sprite_mode_8x16 * 8);

    for (int i = 0; i < PPU_OAM_ENTRY_COUNT; i++) {
        OAMEntry *sprite_entry = oam_entries + i;

        // Sprites on the first scanline are invisible
        if (sprite_entry->pos_y == 0)
            continue;

        // Sprites are delayed by one scanline, hence the '-1'
        int32_t y_pos_inside_sprite =
            ppu_ctx->current_scanline - 1 - sprite_entry->pos_y;
        int32_t x_pos_inside_sprite =
            ppu_ctx->current_dot - sprite_entry->pos_x;

        if (y_pos_inside_sprite < 0 || y_pos_inside_sprite >= sprite_height)
            continue;

        if (x_pos_inside_sprite < 0 || x_pos_inside_sprite >= 8)
            continue;

        uint8_t *pattern_table = 0;
        if (ppu_ctx->ppuctrl.sprite_pattern_table_select)
            pattern_table = ppu_ctx->memory.pattern_table_1;
        else
            pattern_table = ppu_ctx->memory.pattern_table_0;

        uint8_t pixel =
            get_pattern_table_pixel(pattern_table, sprite_entry->tile_index,
                                    x_pos_inside_sprite, y_pos_inside_sprite);

        return sprite_entry->attributes.palette << 2 | pixel;
    }

    return 0;
}

void ppu_tick(PPUContext *ppu_ctx, uint32_t *framebuffer, int *out_nmi_needed) {
    // Vertical blank triggers on certain dots, also we interrupt the CPU at the
    // start of it
    if (ppu_ctx->current_dot == 1 && ppu_ctx->current_scanline == 241 &&
        out_nmi_needed) {
        *out_nmi_needed = 1;
        ppu_ctx->ppustatus.vblank = 1;
    }

    if (ppu_ctx->current_dot == 1 && ppu_ctx->current_scanline == 261) {
        ppu_ctx->ppustatus.vblank = 0;
    }

    // Sprite evaluation
    if (ppu_ctx->current_dot < PPU_VISIBLE_AREA_WIDTH &&
        ppu_ctx->current_scanline < PPU_VISIBLE_AREA_HEIGTH) {
        uint32_t temp_palette[16] = {0x00000000, 0x00ff0000, 0x0000ff00,
                                     0x000000ff};
        uint8_t sprite_pixel = evaluate_sprites(ppu_ctx);

        framebuffer[ppu_ctx->current_scanline * PPU_VISIBLE_AREA_WIDTH +
                    ppu_ctx->current_dot] = temp_palette[sprite_pixel];
    }

    // Increment current dot and scanline

    ppu_ctx->current_dot++;

    if (ppu_ctx->current_dot == DOTS_PER_SCANLINE) {
        ppu_ctx->current_dot = 0;
        ppu_ctx->current_scanline++;
    }

    if (ppu_ctx->current_scanline == SCANLINES_PER_FRAME)
        ppu_ctx->current_scanline = 0;

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

uint8_t ppu_read_oamdata(PPUContext *ppu_ctx) {
    return ppu_ctx->oam[ppu_ctx->oam_address];
}

void ppu_write_ppuctrl(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppuctrl.value = value;
}

void ppu_write_ppumask(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->ppumask.value = value;
}

void ppu_write_ppuscroll(uint8_t value, PPUContext *ppu_ctx) {
    if (!ppu_ctx->write_latch)
        ppu_ctx->scroll_x = value;
    else
        ppu_ctx->scroll_y = value;

    ppu_ctx->write_latch = !ppu_ctx->write_latch;
}

void ppu_write_ppuaddr(uint8_t value, PPUContext *ppu_ctx) {
    if (!ppu_ctx->write_latch)
        ppu_ctx->address = (ppu_ctx->address & 0x00ff) | value << 8;
    else
        ppu_ctx->address = (ppu_ctx->address & 0xff00) | value;

    ppu_ctx->write_latch = !ppu_ctx->write_latch;
}

void ppu_write_ppudata(uint8_t value, PPUContext *ppu_ctx) {
    ppu_memory_write(ppu_ctx->address, value, &ppu_ctx->memory);
    increment_ppu_address(ppu_ctx);
}

void ppu_write_oamaddr(uint8_t value, PPUContext *ppu_ctx) {
    ppu_ctx->oam_address = value;
}

void ppu_write_oamdata(uint8_t value, PPUContext *ppu_ctx) {
    // Won't need safety checks since oam_address is one byte and the size of
    // oam is 0x100
    ppu_ctx->oam[ppu_ctx->oam_address++] = value;
}
