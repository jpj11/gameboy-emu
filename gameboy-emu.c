#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include "gbCPU.h"

bool InputIsValid(int argc, char **argv, FILE **output);
bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER);

void *EmulateCPU(void *args);
void *EmulateGraphics(void *args);

Uint64 Timer(Uint64 begin, long double threshold, void (*callMe)(void **), void **params);
void ExecuteInst(void **params);
void DrawGraphics(void **params);
void IncrementDiv(void **params);

pthread_barrier_t barrier;

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

    pthread_t cpuThread, graphicsThread;
    pthread_barrier_init(&barrier, NULL, 3);

    void *emulateCPUArgs[] = { &quit, output };
    pthread_create(&cpuThread, NULL, EmulateCPU, emulateCPUArgs);

    void *emulateGraphicsArgs[] = { &quit, output };
    pthread_create(&graphicsThread, NULL, EmulateGraphics, emulateGraphicsArgs);

    // User input loop
    pthread_barrier_wait(&barrier);
    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // Do input here

            if(event.type == SDL_QUIT)
                quit = true;
        }
    }

    pthread_join(cpuThread, NULL);
    pthread_join(graphicsThread, NULL);
    pthread_barrier_destroy(&barrier);

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

void *EmulateCPU(void *args)
{
    void **params = (void **)args;
    bool *quit = (bool *)params[0];
    FILE *output = (FILE *)params[1];

    Uint64 cycleStart = 0;
    short cycles = 0;

    // Point function pointer to ExecuteInst() and set parameters to pass
    const void (*executeInstPtr)(void **params) = ExecuteInst;
    void *executeInstParams[] = { &cycles, output };

    pthread_barrier_wait(&barrier);
    while(!(*quit))
        cycleStart = Timer(cycleStart, cycles * SEC_PER_CYCLE, executeInstPtr, executeInstParams);
    return NULL;
}

void *EmulateGraphics(void *args)
{
    void **params = (void **)args;
    bool *quit = (bool *)params[0];
    FILE *output = (FILE *)params[1];

    Uint64 frameStart = 0;
    short count = 0;

    // Point function pointer to DrawGraphics() and set parameters to pass
    const void (*drawGraphicsPtr)(void **params) = DrawGraphics;
    void *drawGraphicsParams[] = { &count, output };

    pthread_barrier_wait(&barrier);
    while(!(*quit))
        frameStart = Timer(frameStart, SEC_PER_FRAME, drawGraphicsPtr, drawGraphicsParams);
    return NULL;
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

// Increment the DIV register at the appropriate time
void IncrementDiv(void **params)
{
    FILE *output = (FILE *)params[1];

    mainMemory[REG_DIV] += 1;
    fprintf(output, "DIV!\n");
}