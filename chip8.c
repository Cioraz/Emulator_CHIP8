#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef struct{
    uint32_t window_height;
    uint32_t window_width; 
    uint32_t fg_col; // RGBA8888
    uint32_t bg_col;
    uint32_t scale_factor;
} config_t;

typedef enum{
    QUIT,
    RUNNING,
    PAUSED,
} emulator_state_t;

typedef struct{
    emulator_state_t state;
} chip8_t;

bool init_chip8(chip8_t *chip8){
    chip8->state = RUNNING;
    return true;
}

bool init_SDL(sdl_t *sdl, config_t *config){
    if(SDL_InitSubSystem(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)!=0){
        SDL_Log("Could not initialise SDL subsystems! %s\n",SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("CHIP8 Emulator",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,config->window_width*config->scale_factor,config->window_height*config->scale_factor,0);

    if(!sdl->window){
        SDL_Log("Could not create SDL window %s\n",SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window,-1,SDL_RENDERER_ACCELERATED);
    if(!sdl->renderer){
        SDL_Log("Could not create renderer! %s\n",SDL_GetError());
        return false;
    }
    return true;
}

void init_config(config_t *config){
    config->window_height=32;
    config->window_width=64;
    config->fg_col=0xFFFF00FF;
    config->bg_col=0x00000000;
    config->scale_factor=20;
}


void cleanUP(sdl_t *sdl){
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit(); // Shuts the subsystems 
}

void clear_screen(sdl_t *sdl,config_t *config){
    // RGBA is how it is stored where in each takes 8 bits
    // 0xFF makes the least significant 8 bits
    const uint8_t r = (config->bg_col>>24)&0xFF;
    const uint8_t g = (config->bg_col>>16)&0xFF;
    const uint8_t b = (config->bg_col>> 8)&0xFF;
    const uint8_t a = (config->bg_col>> 0)&0xFF;
    SDL_SetRenderDrawColor(sdl->renderer,r,g,b,a);
    SDL_RenderClear(sdl->renderer);
}

void update_screen(sdl_t *sdl){
    SDL_RenderPresent(sdl->renderer);
}

void handle_inputs(chip8_t *chip8){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                chip8->state = QUIT;
                return;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        chip8->state = QUIT;
                        return;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                break;
            default:
                break;
        }
    }
}

int main(int argc,char **argv){
    (void) argc;
    (void) argv;
    config_t *config = malloc(sizeof(config_t));
    sdl_t *sdl = malloc(sizeof(sdl_t));
    chip8_t *chip8 = malloc(sizeof(chip8_t));
    init_config(config);
    if(!init_SDL(sdl,config)) exit(EXIT_FAILURE);
    if(!init_chip8(chip8)) exit(EXIT_FAILURE);
    clear_screen(sdl,config);
   

    // Main Loop
    while(chip8->state != QUIT){
        handle_inputs(chip8);
        // Delay for 60fps
        SDL_Delay(17);

        update_screen(sdl);
    }

    cleanUP(sdl);
    exit(EXIT_SUCCESS);
}