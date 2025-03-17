#include "rom_file.h"
#include "memory.h"
#include "ppu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copies data from the `file_buffer` into the appropriate locations in the
// `Memory` struct
static int populate_cpu_memory_with_file_data(uint8_t *file_buffer,
                                              Memory *memory, int limit) {
    INESHeader *header = (INESHeader *)file_buffer;
    memory->prg_rom_size = header->prg_rom_size_16k * 16 * 1024;
    memory->chr_rom_size = header->chr_rom_size_8k * 8 * 1024;

    // Safety check
    int usage = INES_HEADER_SIZE +
                (header->using_trainer * MEMORY_TRAINER_SIZE) +
                memory->prg_rom_size + memory->chr_rom_size;
    if (usage > limit)
        return 1;

    int cursor = INES_HEADER_SIZE;

    // Trainer is a 512 byte chunk of extra stuff usable to the CPU at 0x7000.
    // Unused by most cartridges.
    if (header->using_trainer) {
        memcpy(memory->trainer, file_buffer + cursor, MEMORY_TRAINER_SIZE);
        cursor += MEMORY_TRAINER_SIZE;
    }

    // Program ROM
    memory->prg_rom = (uint8_t *)malloc(memory->prg_rom_size);
    memcpy(memory->prg_rom, file_buffer + cursor, memory->prg_rom_size);
    cursor += memory->prg_rom_size;

    // Character / Sprite ROM
    if (memory->chr_rom_size &&
        memory->chr_rom_size < PPU_MEMORY_CARTRIDGE_MAPPED_TOTAL_SIZE) {
        memcpy(memory->ppu_ctx.memory.cartridge_mapped_memory,
               file_buffer + cursor, memory->chr_rom_size);
        cursor += memory->chr_rom_size;
    }

    return 0;
}

int rom_file_read(char *filepath, Memory *memory) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        perror("Could not open ROM file");
        return 1;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read into memory
    uint8_t *file_buffer = (uint8_t *)malloc(filesize + 1);
    fread(file_buffer, filesize, 1, fp);
    fclose(fp);

    INESHeader *header = (INESHeader *)file_buffer;
    if (filesize < 16 || strncmp(header->magic, "NES\032", 4)) {
        fprintf(stderr, "Invalid ROM file\n");
        return 1;
    }

    if (populate_cpu_memory_with_file_data(file_buffer, memory, filesize)) {
        fprintf(stderr, "Invalid ROM file\n");
        return 1;
    }

    free(file_buffer);

    return 0;
}
