#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

int main(int argc, char **argv)
{
    WORD opcode = 0x0000;

    // Check for valid usage
    if(argc < 3 || argc > 4)
    {
        fprintf(stderr,
            "USAGE ERROR!\nCorrect Usage: gameboy-emu <rom-file> <graphics-multiple> <optional-output-file>");
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

    FILE *dmg = NULL;
    if((dmg = fopen("DMG_ROM.bin", "rb")) == NULL)
    {
        fprintf(stderr, "INITIALIZATION ERROR!\nCould not open DMG_ROM.bin");
        return -1;
    }
    fread(&memMainRAM[0x0000], 0x0100, 1, dmg);

    PC.word = 0x0000;
    while(PC.word < 0x0100)
    {
        opcode = Fetch(output);

        DecodeExecute(opcode, output);

        fprintf(output, "\n");
    }

    if(output)
        fclose(output);

    InitSystem();

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
    memMainRAM[0xFF05] = 0x00;  // TIMA
    memMainRAM[0xFF06] = 0x00;  // TMA
    memMainRAM[0xFF07] = 0x00;  // TAC
    memMainRAM[0xFF10] = 0x80;  // NR10
    memMainRAM[0xFF11] = 0xBF;  // NR11
    memMainRAM[0xFF12] = 0xF3;  // NR12
    memMainRAM[0xFF14] = 0xBF;  // NR14
    memMainRAM[0xFF16] = 0x3F;  // NR21
    memMainRAM[0xFF17] = 0x00;  // NR22
    memMainRAM[0xFF19] = 0xBF;  // NR24
    memMainRAM[0xFF1A] = 0x7F;  // NR30
    memMainRAM[0xFF1B] = 0xFF;  // NR31
    memMainRAM[0xFF1C] = 0x9F;  // NR32
    memMainRAM[0xFF1E] = 0xBF;  // NR33
    memMainRAM[0xFF20] = 0xFF;  // NR41
    memMainRAM[0xFF21] = 0x00;  // NR42
    memMainRAM[0xFF22] = 0x00;  // NR43
    memMainRAM[0xFF23] = 0xBF;  // NR30
    memMainRAM[0xFF24] = 0x77;  // NR50
    memMainRAM[0xFF25] = 0xF3;  // NR51
    memMainRAM[0xFF26] = 0xF1;  // NR52
    memMainRAM[0xFF40] = 0x91;  // LCDC
    memMainRAM[0xFF42] = 0x00;  // SCY
    memMainRAM[0xFF43] = 0x00;  // SCX
    memMainRAM[0xFF45] = 0x00;  // LYC
    memMainRAM[0xFF47] = 0xFC;  // BGP
    memMainRAM[0xFF48] = 0xFF;  // OBP0
    memMainRAM[0xFF49] = 0xFF;  // OBP1
    memMainRAM[0xFF4A] = 0x00;  // WY
    memMainRAM[0xFF4B] = 0x00;  // WX
    memMainRAM[0xFFFF] = 0x00;  // IE
}

// Fetch the next opcode to be executed
BYTE Fetch(FILE *output)
{
    BYTE val = memMainRAM[PC.word++];
    fprintf(output, "%0#4x --> ", val);
    return val;
}

WORD GetImmediateWord(FILE *output)
{
    WORD val = Fetch(output);
    val |= Fetch(output) << 8;
    return val;
}

BYTE ReadByte(WORD address, FILE *output)
{
    fprintf(output, " --> ReadByte(%0#6x)", address);
    return 0x00;
}

int WriteByte(WORD address, BYTE data, FILE *output)
{
    fprintf(output, " --> WriteByte(%0#6x, %0#4x)", address, data);
    return 1;
}

void LoadByte(BYTE *dest, BYTE source, unsigned int cycles, FILE *output)
{
    fprintf(output, " --> LoadByte(%p, %0#4x, %d)", dest, source, cycles);
}

void StoreByte(WORD address, BYTE data, unsigned int cycles, FILE *output)
{
    fprintf(output, " --> StoreByte(%0#6x, %0#4x, %d)", address, data, cycles);
    WriteByte(address, data, output);
}