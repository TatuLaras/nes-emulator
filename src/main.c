#include "cpu.h"
#include "memory.h"
#include "rom_file.h"
#include <stdio.h>

static CPUContext ctx = {0, .program_counter = 0x8000};
static Memory memory = {0};

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage:\nnes [file path of ROM]");
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
