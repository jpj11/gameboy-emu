#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

int main(int argc, char **argv)
{
    // Check for valid usage
    if(argc < 3 || argc > 4)
    {
        fprintf(stderr, "USAGE ERROR!\nCorrect Usage: gameboy-emu <rom-file> <graphics-multiple>"
                "<optional-output-file>");
        return -1;
    }
    
    // Open ROM file
    FILE *input = NULL;
    if((input = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "FILE I/O ERROR!\nCould not open file \"%s\"", argv[1]);
        return -1;
    }
    fclose(input);

    // Check for valid multiplier
    if(atoi(argv[2]) <= 0)
    {
        fprintf(stderr, "GRAPHICS ERROR!\n<graphics-multiple> must be a positive integer");
        return -1;
    }
    const unsigned int MULTIPLIER = atoi(argv[2]);

    // Check for output file
    FILE *output = NULL;
    if((argc == 4) && ((output = fopen(argv[3], "w")) == NULL))
    {
        fprintf(stderr, "FILE I/O ERROR!\nCould not open file \"%s\"", argv[3]);
        return -1;
    }

    // Check for bootstrap ROM
    FILE *dmg = NULL;
    if((dmg = fopen("DMG_ROM.bin", "rb")) == NULL)
    {
        fprintf(stderr, "INITIALIZATION ERROR!\nCould not open DMG_ROM.bin");
        return -1;
    }
    fread(&memMainRAM[0x0000], 0x0100, 1, dmg);

    SDL_Window *window = NULL;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL ERROR!\nCould not initialize: %s", SDL_GetError());
        return -1;
    }
    else
    {
        // Create window
        window = SDL_CreateWindow("gameboy-emu", SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * MULTIPLIER,
                 SCREEN_HEIGHT * MULTIPLIER, SDL_WINDOW_SHOWN);
        if(window == NULL)
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

    BYTE opcode = 0x00;
    PC.word = 0x0000;
    while(PC.word < 0x0100)
    {
        opcode = Fetch(output);

        DecodeExecute(opcode, output);

        fprintf(output, "\n");
    }

    if(output)
        fclose(output);

    //InitSystem();

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

        // opcode = Fetch();
        // Decode
        // Execute

        // Draw Graphics
    }

    return 0;
}