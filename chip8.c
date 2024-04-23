// Basic imports
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"
#include "structs.h"

// Main chip8 initialiser
bool init_chip8(chip8_t *chip8,char *rom_name){
    uint32_t entry = 0x200;
    // 80 bytes
    uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
        0x20, 0x60, 0x20, 0x20, 0x70,		// 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
        0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
        0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
        0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
        0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
        0xF0, 0x80, 0xF0, 0x80, 0x80		
    };

    memcpy(&chip8->ram,font,sizeof font);
    FILE *rom = fopen(rom_name,"rb");
    if(!rom){
        SDL_Log("ROM %s doesnt exist \n",rom_name);
        return false;
    }
    fseek(rom,0,SEEK_END);
    size_t rom_size = ftell(rom);
    size_t max_size = sizeof chip8->ram - entry;
    rewind(rom);

    if(rom_size > max_size){
        SDL_Log("Rom file %s out of size!\n",SDL_GetError());
        return false;
    }

    if(fread(&chip8->ram[entry],rom_size,1,rom)!=1){
        SDL_Log("Unable to read Rom file %s\n",rom_name);
        return false;
    }

    fclose(rom);

    chip8->state = RUNNING;
    chip8->pc = entry;
    chip8->rom_name = rom_name;

    return true;
}

// Getting SDL ready
bool init_SDL(sdl_t *sdl, config_t *config){
    if(SDL_InitSubSystem(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){
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
                    case SDLK_SPACE:
                        if(chip8->state == RUNNING) chip8->state = PAUSED;
                        else{
                            chip8->state = RUNNING;
                            SDL_Log("--------------------PAUSED--------------------");
                        }
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

//void emulate_inst(chip8_t *chip8){


//}

int main(int argc,char **argv){
    if(argc<2) {
        SDL_Log("Usage: ./chip8 <rom_name>");
        exit(EXIT_FAILURE);
    };
    (void) argc;
    (void) argv;
    config_t *config = malloc(sizeof(config_t));
    sdl_t *sdl = malloc(sizeof(sdl_t));
    chip8_t *chip8 = malloc(sizeof(chip8_t));
    init_config(config);
    if(!init_SDL(sdl,config)) exit(EXIT_FAILURE);
    if(!init_chip8(chip8,argv[1])) exit(EXIT_FAILURE);
    clear_screen(sdl,config);
   
    // Main Loop
    while(chip8->state != QUIT){
        handle_inputs(chip8);
        //emulate_inst(chip8);
        if(chip8->state == PAUSED) continue;
        // Delay for 60fps
        SDL_Delay(17);

        update_screen(sdl);
    }

    cleanUP(sdl);
    exit(EXIT_SUCCESS);
}