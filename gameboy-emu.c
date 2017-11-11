#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

bool InputIsValid(int argc, char **argv, FILE **output);
bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER);
void InitSystem();

Uint64 Timer(Uint64 begin, long double threshold, void (*callMe)(void **), void **params);
void ExecuteInst(void **params);
void DrawGraphics(void **params);

int main(int argc, char **argv)
{
    // Where diagnostic output is printed
    FILE *output = NULL;

    // Check for valid files and usage
    if(!InputIsValid(argc, argv, &output))
        return -1;

    // Integer multiple for graphics
    const unsigned short MULTIPLIER = atoi(argv[2]);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize SDL
    if(!InitializeSDL(&window, &renderer, MULTIPLIER))
        return -1;

    // Initialize Gameboy cpu and memory
    InitSystem();

    bool quit = false;      // Controls main emulation loop
    SDL_Event event;        // Captures user input
    short cycles = -1;      // Number of cycles consumed by a given instruction
    
    // Used to calculate timings accurate to the original hardware
    Uint64 frameStart = 0;  // Time at beginning of frame
    Uint64 cycleStart = 0;  // Time at beginning of cycle

    short count = 0;

    // Point function pointer to ExecuteInst() and set parameters to pass
    const void (*executeInstPtr)(void **params) = ExecuteInst;
    void *executeInstParams[] = { &cycles, output };
    
    // Point function pointer to DrawGraphics() and set parameters to pass
    const void (*drawGraphicsPtr)(void **params) = DrawGraphics;
    void *drawGraphicsParams[] = { &count, output };

    // Main emulation loop
    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // Do input here

            if(event.type == SDL_QUIT)
                quit = true;
        }

        // Call ExecuteInst when the threshold of cycles * SEC_PER_CYCLE has been passed
        cycleStart = Timer(cycleStart, cycles * SEC_PER_CYCLE, executeInstPtr, executeInstParams);
        
        // Call DrawGraphics when the threshold of SEC_PER_FRAME has been passed
        frameStart = Timer(frameStart, SEC_PER_FRAME, drawGraphicsPtr, drawGraphicsParams);
    }

    // Close the output file if one was specified
    if(output != stdout && output != NULL)
        fclose(output);

    // Free dynamically allocated game pak
    free(gamePakMem);

    return 0;
}

bool InputIsValid(int argc, char **argv, FILE **output)
{
    FILE *input = NULL;
    long inputROMSize = 0l;

    // Check for valid usage
    if(argc < 3 || argc > 4)
    {
        fprintf(stderr, "USAGE ERROR!\nCorrect Usage: gameboy-emu <rom-file> <graphics-multiple> <optional-output>");
        return false;
    }
    
    // Open ROM file
    if((input = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "FILE I/O ERROR!\nCould not open file \"%s\"", argv[1]);
        return false;
    }
    
    // Calculate the size of the input ROM
    fseek(input, 0l, SEEK_END);
    inputROMSize = ftell(input);
    rewind(input);

    // Load input ROM dynamically
    gamePakMem = malloc(inputROMSize * sizeof(BYTE));
    if (fread(gamePakMem, 1, inputROMSize, input) != inputROMSize)
    {
        fprintf(stderr, "FILE I/O ERROR!\nRead error occurred while processing input ROM");
        return false;
    }
    fclose(input);

    // Check for valid multiplier
    if(atoi(argv[2]) <= 0)
    {
        fprintf(stderr, "GRAPHICS ERROR!\n<graphics-multiple> must be a positive integer");
        return false;
    }

    // Check for output file
    if(argc == 4)
    {
        if(strcmp(argv[3], "console") == 0)
            *output = stdout;
        else if((*output = fopen(argv[3], "w")) == NULL)
        {
            fprintf(stderr, "FILE I/O ERROR!\nCould not open file \"%s\"", argv[3]);
            return false;
        }
    }

    return true;
}

bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER)
{
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL ERROR!\nCould not initialize: %s", SDL_GetError());
        return false;
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
            return false;
        }
        else
        {
            // Create renderer
            *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
            if(*renderer == NULL)
            {
                fprintf(stderr, "SDL ERROR!\nRenderer could not be created: %s", SDL_GetError());
                return false;
            }
            SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    
    return true;
}

// (http://bgb.bircd.org/pandocs.htm#powerupsequence)
void InitSystem()
{
    // Initialize registers
    // regAF.word = 0x01B0;
    // regBC.word = 0x0013;
    // regDE.word = 0x00D8;
    // regHL.word = 0x014D;

    // Initialize program counter and stack pointer
    PC.word = 0x0000;
    // SP.word = 0xFFFE;

    // Initialize RAM (I/0 Special Registers)
    mainMemory[REG_DIV] = 0x00;

    // mainMemory[0xFF05] = 0x00;  // TIMA
    // mainMemory[0xFF06] = 0x00;  // TMA
    // mainMemory[0xFF07] = 0x00;  // TAC
    // mainMemory[0xFF10] = 0x80;  // NR10
    // mainMemory[0xFF11] = 0xBF;  // NR11
    // mainMemory[0xFF12] = 0xF3;  // NR12
    // mainMemory[0xFF14] = 0xBF;  // NR14
    // mainMemory[0xFF16] = 0x3F;  // NR21
    // mainMemory[0xFF17] = 0x00;  // NR22
    // mainMemory[0xFF19] = 0xBF;  // NR24
    // mainMemory[0xFF1A] = 0x7F;  // NR30
    // mainMemory[0xFF1B] = 0xFF;  // NR31
    // mainMemory[0xFF1C] = 0x9F;  // NR32
    // mainMemory[0xFF1E] = 0xBF;  // NR33
    // mainMemory[0xFF20] = 0xFF;  // NR41
    // mainMemory[0xFF21] = 0x00;  // NR42
    // mainMemory[0xFF22] = 0x00;  // NR43
    // mainMemory[0xFF23] = 0xBF;  // NR30
    // mainMemory[0xFF24] = 0x77;  // NR50
    // mainMemory[0xFF25] = 0xF3;  // NR51
    // mainMemory[0xFF26] = 0xF1;  // NR52
    // mainMemory[0xFF40] = 0x91;  // LCDC
    // mainMemory[0xFF42] = 0x00;  // SCY
    // mainMemory[0xFF43] = 0x00;  // SCX
    // mainMemory[0xFF45] = 0x00;  // LYC
    // mainMemory[0xFF47] = 0xFC;  // BGP
    // mainMemory[0xFF48] = 0xFF;  // OBP0
    // mainMemory[0xFF49] = 0xFF;  // OBP1
    // mainMemory[0xFF4A] = 0x00;  // WY
    // mainMemory[0xFF4B] = 0x00;  // WX
    // mainMemory[0xFFFF] = 0x00;  // IE
}

// Timer takes a timestamp (parameter begin) and calls arbitrary function callMe when a particular
// amount of time has passed
Uint64 Timer(Uint64 begin, long double threshold, void (*callMe)(void **), void **params)
{
    // Set delta to the amount of time since begin
    long double delta = ((SDL_GetPerformanceCounter() - begin) / (long double)SDL_GetPerformanceFrequency());
    
    // If this amount of time passes the threshold, call the function callMe
    if(delta >= threshold)
    {
        // This is a new event; record the beginning of this event
        begin = SDL_GetPerformanceCounter();
        (*callMe)(params);
    }

    return begin;
}

// Execute a single cpu instruction and write out the number of cycles consumed
void ExecuteInst(void **params)
{
    short *cycles = (short *)params[0];
    FILE *output = (FILE *)params[1];
    BYTE opcode = 0x00;

    opcode = FetchByte(output);
    *cycles = DecodeExecute(opcode, output);
    fprintf(output, " (%d cycles)\n", *cycles);
}

// Draw a single frame of graphics to the window
void DrawGraphics(void **params)
{
    short *count = (short *)params[0];
    FILE *output = (FILE *)params[1];

    (*count)++;
    if(*count == 60)
    {
        *count = 0;
        fprintf(output, "60th frame (about one second)\n");
    }
    // Do graphics here
}