#ifndef _ROM_FILE
#define _ROM_FILE

#include "memory.h"
#include <stdint.h>

#define INES_HEADER_SIZE 16

// A header record at the beginning of a ROM file that will tell us some
// information about the cardridge.
typedef struct {
    // Will be "NES", 0x1a
    char magic[4];

    uint8_t prg_rom_size_16k;
    uint8_t chr_rom_size_8k;

    // Flags 6
    uint8_t mapper_number_lower : 4;
    uint8_t using_alternative_nametables : 1;
    uint8_t using_trainer : 1;
    uint8_t using_non_volatile_memory : 1;
    uint8_t mirrored_vertically : 1;

    // Flags 7
    uint8_t mapper_number_higher : 4;
    // If it's 2 then this is an INES 2.0 ROM
    uint8_t ines_2_identifier : 2;
    uint8_t console_type : 2;

    // Rest of the flags (format depends on whether or not this is an INES 2.0
    // ROM)
    uint8_t flags8;
    uint8_t flags9;
    uint8_t flags10;

    // Empty
    uint64_t : 40;
} INESHeader;

// Reads ROM file at `filepath` and populates `memory` with appropriate
// cartridge data from the ROM.
//
// Returns 1 on failure, will also print error messages to stderr.
int rom_file_read(char *filepath, Memory *memory);

#endif
