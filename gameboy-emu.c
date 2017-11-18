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

volatile int frameCounter = 0;
volatile short lineCounter = 0;
volatile short cycleCounter = 0;
volatile short divCounter = 0;
volatile short timaCounter = 0;

pthread_barrier_t barrier;

volatile bool requestExit = false;
pthread_mutex_t exitLock;

pthread_cond_t CPUCond;
pthread_mutex_t CPULock;
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
    pthread_mutex_init(&exitLock, NULL);
    
    pthread_mutex_init(&CPULock, NULL);
    pthread_cond_init(&CPUCond, NULL);
    pthread_mutex_init(&graphicsLock, NULL);
    pthread_cond_init(&graphicsCond, NULL);

    pthread_t clockThread;
    pthread_create(&clockThread, NULL, EmulateClock, NULL);

    // Create and use thread to emulate the cpu clock in parallel
    //void *emulateClockArgs[] = { &quit };
    
    pthread_t CPUThread;
    void *emulateCPUArgs = (void *)output;
    pthread_create(&CPUThread, NULL, EmulateCPU, emulateCPUArgs);

    // // Create and use thread to emulate cpu in parallel
    // void *emulateCPUArgs[] = { &quit, output };
    // pthread_create(&cpuThread, NULL, EmulateCPU, emulateCPUArgs);

    pthread_t graphicsThread;
    void *emulateGraphicsArgs = (void *)output;
    pthread_create(&graphicsThread, NULL, EmulateGraphics, emulateGraphicsArgs);

    // // Create and use thread to emulate graphics in parallel
    // void *emulateGraphicsArgs[] = { &quit, output };
    // pthread_create(&graphicsThread, NULL, EmulateGraphics, emulateGraphicsArgs);

    //Uint64 clockStart = 0;

    //void *(*incrementCountersPtr)(void *params) = IncrementCounters;
    // void *incrementCountersArgs[] = { &frameCounter, &lineCounter, &cycleCounter, &divCounter, &timaCounter };

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
    pthread_mutex_lock(&exitLock);
    requestExit = true;
    pthread_mutex_unlock(&exitLock);

    // When user quits join separate threads and destroy barrier
    // pthread_join(clockThread, NULL);
    // pthread_join(CPUThread, NULL);
    // pthread_join(graphicsThread, NULL);
    // pthread_join(cpuThread, NULL);
    // pthread_join(graphicsThread, NULL);
    // pthread_join(clockThread, NULL);

    pthread_cond_destroy(&graphicsCond);
    pthread_mutex_destroy(&graphicsLock);
    pthread_cond_destroy(&CPUCond);
    pthread_mutex_destroy(&CPULock);
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
    pthread_detach(pthread_self());
    bool quit = false;
    Uint64 clockStart = 0;

    // Point function pointer to DrawGraphics() and set parameters to pass
    void *(*incrementCountersPtr)(void *params) = IncrementCounters;

    pthread_barrier_wait(&barrier);
    while(!quit)
    {
        clockStart = Timer(clockStart, SEC_PER_CYCLE, incrementCountersPtr, NULL);

        pthread_mutex_lock(&exitLock);
        quit = requestExit;
        pthread_mutex_unlock(&exitLock);
    }
    return NULL;
}

// Emulates the cpu by excuting instructions at the appropriate times
void *EmulateCPU(void *params)
{
    pthread_detach(pthread_self());
    // Fetch actual parameters from params
    FILE *output = (FILE *)params;

    //Uint64 instStart = 0;   // Stores the timestamp of when an instruct begins
    // short cycles = 0;       // The number of cycles consumed by an instruction
    // short count = 0;
    BYTE opcode = 0x00;
    bool quit = false;

    pthread_barrier_wait(&barrier);
    while(!quit)
    {
        opcode = FetchByte(output);

        pthread_mutex_lock(&CPULock);
        cycles = DecodeExecute(opcode, output);
        fprintf(output, " (%d cycles)\n", cycles);
        pthread_mutex_unlock(&CPULock);

        // do
        // {
        //     pthread_mutex_lock(&CPULock);
        //     count = cycleCounter;
        //     pthread_mutex_unlock(&CPULock);
        // } while(count < cycles);
        // cycleCounter = 0;

        pthread_mutex_lock(&CPULock);
        while(cycleCounter < cycles)
        {
            pthread_cond_wait(&CPUCond, &CPULock);
        }
        cycleCounter = 0;
        pthread_mutex_unlock(&CPULock);

        pthread_mutex_lock(&exitLock);
        quit = requestExit;
        pthread_mutex_unlock(&exitLock);
    }

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
    pthread_detach(pthread_self());
    // Fetch actual parameters from params
    FILE *output = (FILE *)params;

    //Uint64 frameStart = 0;  // Stores the timestamp of when a frame begins
    bool quit = false;

    // Point function pointer to DrawGraphics() and set parameters to pass
    //void *(*drawGraphicsPtr)(void *params) = DrawGraphics;
    //void *drawGraphicsArgs[] = { &count, output };

    // Draw the next frame at the appropriate time until emulation is ended
    // Barrier ensures that cpu, graphics, and input threads begin emulation at the same time
    pthread_barrier_wait(&barrier);
    while(!quit)
    {
        fprintf(output, "\n\n\n\n\n60th frame (about one second)\n\n\n\n\n");

        pthread_mutex_lock(&graphicsLock);
        while(frameCounter < CYCLES_PER_FRAME)
        {
            pthread_cond_wait(&graphicsCond, &graphicsLock);
        }
        frameCounter = 0;
        pthread_mutex_unlock(&graphicsLock);

        pthread_mutex_lock(&exitLock);
        quit = requestExit;
        pthread_mutex_unlock(&exitLock);
    }
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
    pthread_mutex_lock(&CPULock);
    cycleCounter++;
    if (cycleCounter >= cycles) pthread_cond_broadcast(&CPUCond);
    pthread_mutex_unlock(&CPULock);

    pthread_mutex_lock(&graphicsLock);
    frameCounter++;
    if (frameCounter >= CYCLES_PER_FRAME) pthread_cond_broadcast(&graphicsCond);
    pthread_mutex_unlock(&graphicsLock);

    lineCounter++;
    divCounter++;
    timaCounter++;

    return NULL;
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