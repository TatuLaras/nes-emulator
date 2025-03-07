// Handles read/write into RAM and read from ROM
// Also redirects memory-mapped I/O

#ifndef _MEMORY
#define _MEMORY

#include <stdint.h>
#define RAM_SIZE 0x0800

typedef struct {
    uint8_t ram[RAM_SIZE];
    // Contains game code, no fixed size
    uint8_t *prg_rom;
} Memory;

uint8_t read(uint16_t address);
void write(uint16_t address, char data);

#endif
