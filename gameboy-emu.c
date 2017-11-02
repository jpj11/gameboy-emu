#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

bool CheckValidInput(int argc, char **argv);
void InitSystem();

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
    strcpy((char *)mainMemory, BOOTSTRAP_ROM);

    // FILE *dmg = NULL;
    // if((dmg = fopen("DMG_ROM.bin", "rb")) == NULL)
    // {
    //     fprintf(stderr, "INITIALIZATION ERROR!\nCould not open DMG_ROM.bin");
    //     return -1;
    // }
    // fread(&mainMemory[0x0000], 0x0100, 1, dmg);

    for(int i = 0; i < 48; i++)
    {
        mainMemory[0x0104+i] = mainMemory[0x00A8+i];
    }

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

    short cycles = -1;
    BYTE opcode = 0x00;
    PC.word = 0x0000;
    while(PC.word < 0x0100)
    {
        opcode = FetchByte(stdout);

        cycles = DecodeExecute(opcode, stdout);

        fprintf(stdout, " <> %d cycles\n", cycles);
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

        // opcode = FetchByte();
        // Decode
        // Execute

        // Draw Graphics
    }

    return 0;
}

// (http://bgb.bircd.org/pandocs.htm#powerupsequence)
void InitSystem()
{
    // Initialize registers
    regAF.word = 0x01B0;
    regBC.word = 0x0013;
    regDE.word = 0x00D8;
    regHL.word = 0x014D;

    // Initialize program counter and stack pointer
    PC.word = 0x0100;
    SP.word = 0xFFFE;

    // Initialize RAM (I/0 Special Registers)
    mainMemory[0xFF05] = 0x00;  // TIMA
    mainMemory[0xFF06] = 0x00;  // TMA
    mainMemory[0xFF07] = 0x00;  // TAC
    mainMemory[0xFF10] = 0x80;  // NR10
    mainMemory[0xFF11] = 0xBF;  // NR11
    mainMemory[0xFF12] = 0xF3;  // NR12
    mainMemory[0xFF14] = 0xBF;  // NR14
    mainMemory[0xFF16] = 0x3F;  // NR21
    mainMemory[0xFF17] = 0x00;  // NR22
    mainMemory[0xFF19] = 0xBF;  // NR24
    mainMemory[0xFF1A] = 0x7F;  // NR30
    mainMemory[0xFF1B] = 0xFF;  // NR31
    mainMemory[0xFF1C] = 0x9F;  // NR32
    mainMemory[0xFF1E] = 0xBF;  // NR33
    mainMemory[0xFF20] = 0xFF;  // NR41
    mainMemory[0xFF21] = 0x00;  // NR42
    mainMemory[0xFF22] = 0x00;  // NR43
    mainMemory[0xFF23] = 0xBF;  // NR30
    mainMemory[0xFF24] = 0x77;  // NR50
    mainMemory[0xFF25] = 0xF3;  // NR51
    mainMemory[0xFF26] = 0xF1;  // NR52
    mainMemory[0xFF40] = 0x91;  // LCDC
    mainMemory[0xFF42] = 0x00;  // SCY
    mainMemory[0xFF43] = 0x00;  // SCX
    mainMemory[0xFF45] = 0x00;  // LYC
    mainMemory[0xFF47] = 0xFC;  // BGP
    mainMemory[0xFF48] = 0xFF;  // OBP0
    mainMemory[0xFF49] = 0xFF;  // OBP1
    mainMemory[0xFF4A] = 0x00;  // WY
    mainMemory[0xFF4B] = 0x00;  // WX
    mainMemory[0xFFFF] = 0x00;  // IE
}