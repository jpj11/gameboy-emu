#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

bool InputIsValid(int argc, char **argv, FILE **output);
bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER);

Uint64 Timer(Uint64 begin, long double threshold, void *(*callMe)(void *), void *params);
void *EmulateFrame(void *params);
void *EmulateLine(void *params);

void ExecuteInst(short *cycles, FILE *output);
void DrawScanLine(FILE *output);

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
    
    Uint64 frameStart = 0;
    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // Do input here

            if(event.type == SDL_QUIT)
                quit = true;
        }
        
        frameStart = Timer(frameStart, SEC_PER_FRAME, EmulateFrame, output);
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

// Timer takes a timestamp (parameter begin) and calls arbitrary function callMe when a particular
// amount of time has passed
Uint64 Timer(Uint64 begin, long double threshold, void *(*callMe)(void *), void *params)
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

void *EmulateFrame(void *params)
{
    fprintf((FILE *)params, "\n\nFRAME!\n\n\n");

    mainMemory[REG_LY] = 0x00;
    Uint64 lineStart = 0;
    short cycles = 0, i, j;

    // If the display is disabled, calculate cycles and return
    if((mainMemory[REG_LCDC] & 0x80) == 0)
    {
        for(i = 0; i < SCREEN_HEIGHT; i++)
        {
            for(j = 0; j < CYCLES_PER_LINE; j += cycles)
                ExecuteInst(&cycles, (FILE *)params);

            DrawScanLine((FILE *)params);
            mainMemory[REG_LY] += 1;
        }

        // Mode 1 - Vertical Blank
        SetLCDMode(vblank);
        if(mainMemory[REG_STAT] & 0x10)
            RequestInterrupt(vrefresh);

        for(i = 0; i < SCAN_LINES - SCREEN_HEIGHT; i++)
        {
            for(j = 0; j < CYCLES_PER_LINE; j += cycles)
                ExecuteInst(&cycles, (FILE *)params);
            mainMemory[REG_LY] += 1;
        }

        return NULL;
    }

    // Draw visible scanlines
    while(mainMemory[REG_LY] < SCREEN_HEIGHT)
        lineStart = Timer(lineStart, SEC_PER_LINE, EmulateLine, params);
    
    // Mode 1 - Vertical Blank
    SetLCDMode(vblank);
    if(mainMemory[REG_STAT] & 0x10)
        RequestInterrupt(vrefresh);

    // Execute instructions during vblank
    for(i = 0; i < SCAN_LINES - SCREEN_HEIGHT; i++)
    {
        for(j = 0; j < CYCLES_PER_LINE; j += cycles)
            ExecuteInst(&cycles, (FILE *)params);
        mainMemory[REG_LY] += 1;
    }

    return NULL;
}

void *EmulateLine(void *params)
{   
    FILE *output = (FILE *)params;
    short cyclesThisLine = 0;
    short cycles = 0, i;

    // If REG_LY == REG_LYC, set coincidence bit otherwise unset
    if(mainMemory[REG_LY] == mainMemory[REG_LYC])
        mainMemory[REG_STAT] |= 0x04;
    else
        mainMemory[REG_STAT] &= 0xfb;


    // Mode 2 - OAM Search
    SetLCDMode(oam);
    if(mainMemory[REG_STAT] & 0x20)
        RequestInterrupt(lcd_stat);

    for(i = 0; i < CYCLES_PER_OAM; i += cycles)
        ExecuteInst(&cycles, output);
    cyclesThisLine += i;


    // Mode 3 - Transfer
    SetLCDMode(transfer);
    if((mainMemory[REG_STAT] & 0x40) && (mainMemory[REG_STAT] & 0x04))
        RequestInterrupt(lcd_stat);

    for(i = 0; i < CYCLES_PER_TRANSFER; i += cycles)
        ExecuteInst(&cycles, output);
    cyclesThisLine += i;


    // Mode 0 - Horizontal Blank
    SetLCDMode(hblank);
    if(mainMemory[REG_STAT] & 0x08)
        RequestInterrupt(lcd_stat);

    for(i = 0; i < CYCLES_PER_LINE - cyclesThisLine; i += cycles)
        ExecuteInst(&cycles, output);

    DrawScanLine(output);

    mainMemory[REG_LY] += 0x01;

    return NULL;
}

// Execute a single cpu instruction and write out the number of cycles consumed
void ExecuteInst(short *cycles, FILE *output)
{   
    BYTE opcode = 0x00;             // Opcode representing the instruction to be executed
    static short divCounter = 0;    // Counts the cycles between div register increments
    static short timaCounter = 0;   // Counts the cycles between tima register increments

    // Fetch, decode and execute instruction
    opcode = FetchByte(output);
    *cycles = DecodeExecute(opcode, output);
    fprintf(output, " (%d cycles)\n", *cycles);

    // After DIV_SPEED cycles increment the div register
    divCounter += *cycles;
    if(divCounter >= DIV_SPEED)
    {
        mainMemory[REG_DIV] += 1;
        fprintf(output, "REG_DIV++\n");
        divCounter = 0;
    }

    // If TIMA counter is enabled, count cycles for TIMA
    if(mainMemory[REG_TAC] & 0x04)
    {
        timaCounter += *cycles;

        // If counter surpasses the number of cycles specified by REG_TAC, increment
        if(timaCounter >= TIMA_SPEED[mainMemory[REG_TAC] & 0x03])
        {
            // If REG_TIMA will overflow this increment, request interrupt
            if(mainMemory[REG_TIMA] == 0xff)
            {
                // Reset REG_TIMA to value specified by REG_TMA
                mainMemory[REG_TIMA] = mainMemory[REG_TMA];
                RequestInterrupt(timer);
            }
            // Otherwise just increment REG_TIMA
            else
                mainMemory[REG_TIMA] += 1;

            fprintf(output, "REG_TIMA++\n");
            timaCounter = 0;
        }
    }

    // If master interrupt enable flag is true and interrupts are requested
    if(IME && mainMemory[REG_IF])
    {
        // Check all possible interrupts
        enum interrupt toCheck;
        for(toCheck = vrefresh; toCheck <= joypad; toCheck++)
        {
            // If an interrupt is requested and enabled, call interrupt
            // Note that interrupts are checked in priority order
            if(IsRequested(toCheck) && IsEnabled(toCheck))
            {
                mainMemory[REG_IF] &= ~(1 << toCheck);
                IME = false;

                Call(INTERRUPT_VECTORS[toCheck]);
            }
        }
    }
}

// Draw a single frame of graphics to the window
void DrawScanLine(FILE *output)
{
    fprintf(output, "\n\nLINE!\n\n\n");

    // Do graphics here
}