#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "rom_file.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define CLOCK_CYCLES_PER_FRAME 29780

static CPUContext ctx = {0, .program_counter = 0x8000};
static Memory memory = {0};
int step = 0;
int headless = 0;

SDL_Texture *framebuffer_texture = 0;

static SDL_Window *window = 0;

static void parse_flag(char *argument) {
    if (!strcmp("-step", argument)) {
        step = 1;
        return;
    }
    if (!strcmp("-headless", argument)) {
        headless = 1;
        return;
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // Read ROM file

    char *rom_filepath = 0;

    for (int i = 1; i < argc; i++) {
        if (*argv[i] == '-')
            parse_flag(argv[i]);
        else
            rom_filepath = argv[i];
    }

    if (!rom_filepath) {
        printf("Usage:\n%s [file path of ROM]\n", argv[0]);
        return 1;
    }

    if (rom_file_read(rom_filepath, &memory))
        return 1;

    printf("\n\n\n\n");

    if (step)
        printf("Press enter to run one CPU tick... (q+enter to quit)");

    if (headless)
        return SDL_APP_CONTINUE;

    // Setup graphics window

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    window = SDL_CreateWindow("NES emulator", PPU_VISIBLE_AREA_WIDTH,
                              PPU_VISIBLE_AREA_HEIGTH, 0);
    if (!window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_Surface dummy_surface = {0};
    SDL_Surface *surface = &dummy_surface;

    if (!headless) {
        surface = SDL_GetWindowSurface(window);
        assert(surface);
        assert(surface->pixels);
        assert(surface->w == PPU_VISIBLE_AREA_WIDTH);
        assert(surface->h == PPU_VISIBLE_AREA_HEIGTH);
    }

    for (int i = 0; i < CLOCK_CYCLES_PER_FRAME; i++) {
        if (step) {
            int character = getchar();
            if (character == 'q')
                return SDL_APP_SUCCESS;
        }

        int nmi_needed = 0;
        ppu_tick(&memory.ppu_ctx, (uint32_t *)surface->pixels, &nmi_needed);
        ppu_tick(&memory.ppu_ctx, (uint32_t *)surface->pixels, &nmi_needed);
        ppu_tick(&memory.ppu_ctx, (uint32_t *)surface->pixels, &nmi_needed);

        cpu_tick(&ctx, &memory, nmi_needed);
    }

    if (!headless)
        SDL_UpdateWindowSurface(window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
