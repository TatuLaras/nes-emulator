#ifndef _PPU
#define _PPU

#include <stdint.h>

typedef union {
    struct {
        uint8_t : 5;
        // Gets set when there are more than 8 sprites on one line, although
        // this does not always work correctly
        uint8_t sprite_overflow : 1;
        // Gets set when any opaque pixel of sprite zero overlaps with any
        // opaque pixel of the background.
        uint8_t sprite_0_hit : 1;
        // Gets set at the start of vblank (scanline 241, dot 1)
        uint8_t vblank : 1;
    };

    uint8_t value;
} PPUStatus;

typedef union {
    struct {
        // Selects nametable
        //
        // 0 = 0x2000,
        // 1 = 0x2400,
        // 2 = 0x2800,
        // 3 = 0x2C00
        uint8_t nametable_select : 2;
        // If set auto-increment increments by 32 instead of 1.
        uint8_t increment_mode_vertical : 1;
        // If set pattern table at 0x1000 is used for sprites, otherwise 0x0000.
        // Ignored in 8x16 mode.
        uint8_t sprite_pattern_table_select : 1;
        // If set pattern table at 0x1000 is used for backgrounds, otherwise
        // 0x0000.
        uint8_t background_pattern_table_select : 1;
        // If set tall sprites (8x16) are used.
        uint8_t sprite_mode_8x16 : 1;
        // PPU master/slave mode, largely unused
        uint8_t ppu_master_mode : 1;
        // If set, an interrupt is raised on vblank
        uint8_t vblank_nmi_enable : 1;
    };

    uint8_t value;
} PPUCtrl;

typedef union {
    struct {
        // If set, grayscale mode is used.
        uint8_t grayscale_mode : 1;
        // If set, background is shown in the letmost 8 pixels on screen
        uint8_t show_background_left_column : 1;
        // If set, sprites are shown in the letmost 8 pixels on screen
        uint8_t show_sprites_left_column : 1;
        // If set, background rendering is enabled
        uint8_t background_enable : 1;
        // If set, sprite rendering is enabled
        uint8_t sprites_enable : 1;

        // Applies a tint to the image by dimming other colors than red
        uint8_t emphasize_red : 1;
        // Applies a tint to the image by dimming other colors than green
        uint8_t emphasize_green : 1;
        // Applies a tint to the image by dimming other colors than blue
        uint8_t emphasize_blue : 1;
    };

    uint8_t value;
} PPUMask;

#define PPU_MEMORY_PATTERN_TABLE_SIZE 0x1000
#define PPU_MEMORY_NAMETABLE_SIZE 0x400
#define PPU_MEMORY_PALETTE_SIZE 0x20
#define PPU_MEMORY_CARTRIDGE_MAPPED_TOTAL_SIZE                                 \
    (PPU_MEMORY_NAMETABLE_SIZE * 4 + PPU_MEMORY_PATTERN_TABLE_SIZE * 2)

typedef struct {
    uint8_t palette[PPU_MEMORY_PALETTE_SIZE];

    union {
        struct {
            uint8_t nametable_3[PPU_MEMORY_NAMETABLE_SIZE];
            uint8_t nametable_2[PPU_MEMORY_NAMETABLE_SIZE];
            uint8_t nametable_1[PPU_MEMORY_NAMETABLE_SIZE];
            uint8_t nametable_0[PPU_MEMORY_NAMETABLE_SIZE];
            uint8_t pattern_table_1[PPU_MEMORY_PATTERN_TABLE_SIZE];
            uint8_t pattern_table_0[PPU_MEMORY_PATTERN_TABLE_SIZE];
        };

        uint8_t cartridge_mapped_memory[PPU_MEMORY_CARTRIDGE_MAPPED_TOTAL_SIZE];
    };

} PPUMemory;

typedef struct {
    PPUMemory memory;
    PPUStatus ppustatus;
    PPUCtrl ppuctrl;
    PPUMask ppumask;
    uint8_t write_latch;
    uint8_t scroll_x;
    uint8_t scroll_y;
    uint16_t address;

    // Used to delay reads from PPUDATA by one.
    uint8_t read_buffer;
} PPUContext;

// Does one tick of the PPU.
void ppu_tick(PPUContext *ppu_ctx);

// Rendering events
uint8_t ppu_read_ppustatus(PPUContext *ppu_ctx);
// Read data from PPU memory at address `PPUContext.address` (delayed by one).
uint8_t ppu_read_ppudata(PPUContext *ppu_ctx);

// Miscellaneous settings
void ppu_write_ppuctrl(uint8_t value, PPUContext *ppu_ctx);
// Rendering settings
void ppu_write_ppumask(uint8_t value, PPUContext *ppu_ctx);
// Scroll position (takes 2 writes, x and y).
void ppu_write_ppuscroll(uint8_t value, PPUContext *ppu_ctx);
// PPU memory read/write address (takes 2 writes, high and low byte).
void ppu_write_ppuaddr(uint8_t value, PPUContext *ppu_ctx);
// Write data into PPU memory at address `PPUContext.address`.
void ppu_write_ppudata(uint8_t value, PPUContext *ppu_ctx);

#endif
