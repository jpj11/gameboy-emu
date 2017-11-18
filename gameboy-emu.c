#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "gbCPU.h"

bool InputIsValid(int argc, char **argv, FILE **output);
bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER);

void *EmulateCPU(void *params);
void *EmulateGraphics(void *params);
void *EmulateClock(void *params);

Uint64 Timer(Uint64 begin, long double threshold, void *(*callMe)(void *), void *params);
void *IncrementCounters(void *params);
void *ExecuteInst(void *params);
void *DrawGraphics(void *params);

bool Quit();

pthread_barrier_t barrier;
pthread_attr_t attribute;
pthread_mutex_t exitLock;

pthread_cond_t CPUCond;
pthread_mutex_t CPULock;
pthread_mutex_t cyclesLock;
volatile short cycles = 0;

pthread_cond_t graphicsCond;
pthread_mutex_t graphicsLock;

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
    //int status = 0;

    pthread_barrier_init(&barrier, NULL, 4);
    pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&exitLock, NULL);
    
    pthread_mutex_init(&CPULock, NULL);
    pthread_mutex_init(&cyclesLock, NULL);
    pthread_cond_init(&CPUCond, NULL);

    pthread_mutex_init(&graphicsLock, NULL);
    pthread_cond_init(&graphicsCond, NULL);

    pthread_mutex_lock(&exitLock);

    pthread_t clockThread;
    pthread_create(&clockThread, &attribute, EmulateClock, NULL);

    //pthread_t CPUThread;
    //pthread_create(&CPUThread, &attribute, EmulateCPU, (void *)output);

    pthread_t graphicsThread;
    pthread_create(&graphicsThread, &attribute, EmulateGraphics, (void *)output);

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
    pthread_mutex_unlock(&exitLock);

    pthread_cond_destroy(&graphicsCond);
    pthread_mutex_destroy(&graphicsLock);
    pthread_cond_destroy(&CPUCond);
    pthread_mutex_destroy(&CPULock);
    pthread_mutex_destroy(&cyclesLock);
    pthread_mutex_destroy(&exitLock);
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

void *EmulateClock(void *params)
{
    pthread_mutex_lock(&CPULock);
    pthread_mutex_lock(&graphicsLock);

    Uint64 clockStart = 0;

    short cycleCounter = 0;
    short divCounter = 0;
    short timaCounter = 0;
    short lineCounter = 0;
    int frameCounter = 0;

    void *args[] = { &cycleCounter, &divCounter, &timaCounter, &lineCounter, &frameCounter };

    pthread_barrier_wait(&barrier);
    do
    {
        clockStart = Timer(clockStart, SEC_PER_CYCLE, IncrementCounters, args);
    } while(!Quit());

    return NULL;
}

// Emulates the cpu by excuting instructions at the appropriate times
void *EmulateCPU(void *params)
{
    // Fetch actual parameters from params
    FILE *output = (FILE *)params;

    //Uint64 instStart = 0;   // Stores the timestamp of when an instruct begins
    // short cycles = 0;       // The number of cycles consumed by an instruction
    // short count = 0;
    BYTE opcode = 0x00;

    pthread_barrier_wait(&barrier);
    do
    {
        opcode = FetchByte(output);
     
        pthread_mutex_lock(&cyclesLock);
        cycles = DecodeExecute(opcode, output);
        fprintf(output, " (%d cycles)\n", cycles);
        pthread_mutex_unlock(&cyclesLock);

        while(pthread_mutex_trylock(&CPULock) != 0)
        {
            pthread_cond_wait(&CPUCond, &CPULock);
        }
        pthread_mutex_unlock(&CPULock);

    } while(!Quit());

    // // Point function pointer to ExecuteInst() and set parameters to pass
    // void *(*executeInstPtr)(void *params) = ExecuteInst;
    // void *executeInstArgs[] = { &cycles, output };

    // // Execute the next instruction at the appropriate time until emulation is ended
    // // Barrier ensures that cpu, graphics, and input threads begin emulation at the same time
    // pthread_barrier_wait(&barrier);
    // while(!(*quit))
    //     instStart = Timer(instStart, cycles * SEC_PER_CYCLE, executeInstPtr, executeInstArgs);
    return NULL;
}

// Emulates graphical output by drawing frames at the appropriate times
void *EmulateGraphics(void *params)
{
    // Fetch actual parameters from params
    FILE *output = (FILE *)params;

    //Uint64 frameStart = 0;  // Stores the timestamp of when a frame begins

    // Point function pointer to DrawGraphics() and set parameters to pass
    //void *(*drawGraphicsPtr)(void *params) = DrawGraphics;
    //void *drawGraphicsArgs[] = { &count, output };

    // Draw the next frame at the appropriate time until emulation is ended
    // Barrier ensures that cpu, graphics, and input threads begin emulation at the same time
    pthread_barrier_wait(&barrier);
    do
    {
        fprintf(output, "\n\n\n\n\nFRAME!\n\n\n\n\n");

        while(pthread_mutex_trylock(&graphicsLock) != 0)
        {
            pthread_cond_wait(&graphicsCond, &graphicsLock);
        }
        pthread_mutex_unlock(&graphicsLock);

    } while(!Quit());
    // while(!(*quit))
    //     frameStart = Timer(frameStart, SEC_PER_FRAME, drawGraphicsPtr, drawGraphicsArgs);
    return NULL;
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

void *IncrementCounters(void *params)
{
    void **paramList = (void **)params;

    short *cycleCounter = (short *)paramList[0];
    short *divCounter = (short *)paramList[1];
    short *timaCounter = (short *)paramList[2];
    short *lineCounter = (short *)paramList[3];
    int *frameCounter = (int *)paramList[4];
    
    short thing = 0;

    (*cycleCounter)++;
    (*divCounter)++;
    (*timaCounter)++;
    (*lineCounter)++;
    (*frameCounter)++;

    //pthread_mutex_lock(&cyclesLock);
    //thing = cycles;
    //pthread_mutex_unlock(&cyclesLock);

    // if(*cycleCounter == thing)
    // {
    //     pthread_mutex_unlock(&CPULock);
    //     pthread_cond_signal(&CPUCond);
    //     pthread_mutex_lock(&CPULock);
    //     *cycleCounter = 0;
    // }

    if(*frameCounter == CYCLES_PER_FRAME)
    {
        pthread_mutex_unlock(&graphicsLock);
        pthread_cond_signal(&graphicsCond);
        pthread_mutex_lock(&graphicsLock);
        *frameCounter = 0;
    }

    return NULL;
}

bool Quit()
{
    if(pthread_mutex_trylock(&exitLock) == 0)
    {
        pthread_mutex_unlock(&exitLock);
        return true;
    }
    else
        return false;
}

// // Execute a single cpu instruction and write out the number of cycles consumed
// void *ExecuteInst(void *params)
// {
//     // Fetch actual parameters from params
//     void **paramList = (void **)params;   
//     short *cycles = (short *)paramList[0];
//     FILE *output = (FILE *)paramList[1];
    
//     BYTE opcode = 0x00;             // Opcode representing the instruction to be executed
//     //static short divCounter = 0;    // Counts the cycles between div register increments
//     //static short timaCounter = 0;   // Counts the cycles between tima register increments

//     // Fetch, decode and execute instruction
//     opcode = FetchByte(output);
//     *cycles = DecodeExecute(opcode, output);
//     fprintf(output, " (%d cycles)\n", *cycles);

//     // // After DIV_SPEED cycles increment the div register
//     // divCounter += *cycles;
//     // if(divCounter >= DIV_SPEED)
//     // {
//     //     mainMemory[REG_DIV] += 1;
//     //     fprintf(output, "REG_DIV++\n");
//     //     divCounter = 0;
//     // }

//     // if(mainMemory[REG_TAC] & 0x04)
//     // {
//     //     timaCounter += *cycles;
//     //     if(timaCounter >= TIMA_SPEED[mainMemory[REG_TAC] & 0x03])
//     //     {
//     //         if(mainMemory[REG_TIMA] == 0xff)
//     //         {
//     //             mainMemory[REG_TIMA] = mainMemory[REG_TMA];
//     //             RequestInterrupt(timer);
//     //         }
//     //         else
//     //             mainMemory[REG_TIMA] += 1;

//     //         fprintf(output, "REG_TIMA++\n");
//     //         timaCounter = 0;
//     //     }
//     // }

//     // // If master interrupt enable flag is true and interrupts are requested
//     // if(IME && mainMemory[REG_IF])
//     // {
//     //     enum interrupt toCheck;
//     //     for(toCheck = vblank; toCheck <= joypad; toCheck++)
//     //     {
//     //         if(IsRequested(toCheck) && IsEnabled(toCheck))
//     //         {
//     //             mainMemory[REG_IF] &= ~(1 << toCheck);
//     //             IME = false;

//     //             Call(INTERRUPT_VECTORS[toCheck]);
//     //         }
//     //     }
//     // }
    
//     return NULL;
// }

// // Draw a single frame of graphics to the window
// void *DrawGraphics(void *params)
// {
//     // Fetch actual parameters from params
//     void **paramList = (void **)params;
//     short *count = (short *)paramList[0];
//     FILE *output = (FILE *)paramList[1];

//     (*count)++;
//     if(*count == 60)
//     {
//         *count = 0;
//         fprintf(output, "60th frame (about one second)\n");
//     }

//     // Do graphics here

//     return NULL;
// }