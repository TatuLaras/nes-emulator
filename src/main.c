#include "cpu.h"
#include "memory.h"
#include "rom_file.h"
#include <stdio.h>

static CPUContext ctx = {0, .program_counter = 0x8000};
static Memory memory = {0};

int main(int argc, char *argv[]) {
    memory.ppu_ctx.ppustatus.vblank = 1;

    if (argc < 1)
        return 1;

    if (argc != 2) {
        printf("Usage:\n%s [file path of ROM]\n", argv[0]);
        return 1;
    }

    char *rom_filepath = argv[1];
    if (rom_file_read(rom_filepath, &memory))
        return 1;

    printf("\n\n\n\n");

    while (!cpu_tick(&ctx, &memory)) {
    }

    return 0;
}
