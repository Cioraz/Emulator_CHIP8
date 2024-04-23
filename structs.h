#include <stdint.h>
#include <stdbool.h>
#include "SDL.h"

// SDL type
typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

// Config type
typedef struct{
    uint32_t window_height;
    uint32_t window_width; 
    uint32_t fg_col; // RGBA8888
    uint32_t bg_col;
    uint32_t scale_factor;
} config_t;

// Emulator state type
typedef enum{
    QUIT,
    RUNNING,
    PAUSED,
} emulator_state_t;

// CHIP8 machine instance
typedef struct{
    emulator_state_t state;
    uint8_t ram[0x1000];
    bool screen[64*32];
    uint16_t stack[12];
    uint8_t V[0x10];
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool keypad[16];
    char *rom_name;
    uint16_t pc;
} chip8_t;