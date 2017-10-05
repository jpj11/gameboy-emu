#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "gameboyCPU.h"

int main(int argc, char **argv)
{
    // Check for valid usage
    if(argc != 3)
    {
        fprintf(stderr, "USAGE ERROR!\nCorrect Usage: gameboy-emu <rom-file> <graphics-multiple>.");
        return -1;
    }
    
    // Open ROM file
    FILE *input;
    if((input = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "FILE I/O ERROR!\nCould not open file \"%s\".", argv[1]);
        return -1;
    }

    // Check for valid multiplier
    if(atoi(argv[2]) <= 0)
    {
        fprintf(stderr, "GRAPHICS ERROR!\n<graphics-multiple> must be a positive integer");
        return -1;
    }
    const unsigned int MULTIPLIER = atoi(argv[2]);


    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL ERROR!\nCould not initialize: %s", SDL_GetError());
        return -1;
    }
    else
    {
        // Create window
        *window = SDL_CreateWindow("gameboy-emu", SDL_WINDOWPOS_UNDEFINED,
                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * MULTIPLIER,
                  SCREEN_HEIGHT * MULTIPLIER, SDL_WINDOW_SHOWN);
        if(*window == NULL)
        {
            fprintf(stderr, "SDL ERROR!\nWindow could not be created: %s", SDL_GetError());
            return -1;
        }
        // else
        // {
        //     // Create renderer
        //     *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
        //     if(*renderer == NULL)
        //     {
        //         fprintf(stderr, "SDL ERROR!\nRenderer could not be created: %s", SDL_GetError());
        //         return -1;
        //     }
        //     SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        // }
    }

    int quit = 0;
    SDL_Event event;

    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // Do input here

            if(event.type == SDL_QUIT)
                quit = 1;
        }

        // Fetch
        // Decode
        // Execute

        // Draw Graphics
    }
}