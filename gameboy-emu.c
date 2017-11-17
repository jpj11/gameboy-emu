#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include "gbCPU.h"

bool InputIsValid(int argc, char **argv, FILE **output);
bool InitializeSDL(SDL_Window **window, SDL_Renderer **renderer, const unsigned short MULTIPLIER);

void *EmulateCPU(void *params);
void *EmulateGraphics(void *params);
int EmulateClock(void *params);

Uint64 Timer(Uint64 begin, long double threshold, void *(*callMe)(void *), void *params);
void *IncrementCounters(void *params);
void *ExecuteInst(void *params);
void *DrawGraphics(void *params);

int frameCounter = 0;
short lineCounter = 0;
short cycleCounter = 0;
short divCounter = 0;
short timaCounter = 0;

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
    int status = 0;

    // Create and use thread to emulate the cpu clock in parallel
    void *emulateClockArgs[] = { &quit };
    SDL_Thread *clockThread = SDL_CreateThread(EmulateClock, "EmulateClock", emulateClockArgs);

    // // Create and use thread to emulate cpu in parallel
    // void *emulateCPUArgs[] = { &quit, output };
    // pthread_create(&cpuThread, NULL, EmulateCPU, emulateCPUArgs);

    // // Create and use thread to emulate graphics in parallel
    // void *emulateGraphicsArgs[] = { &quit, output };
    // pthread_create(&graphicsThread, NULL, EmulateGraphics, emulateGraphicsArgs);

    //Uint64 clockStart = 0;

    //void *(*incrementCountersPtr)(void *params) = IncrementCounters;
    // void *incrementCountersArgs[] = { &frameCounter, &lineCounter, &cycleCounter, &divCounter, &timaCounter };

    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // Do input here

            if(event.type == SDL_QUIT)
                quit = true;
        }
    }

    // When user quits join separate threads and destroy barrier
    SDL_WaitThread(clockThread, &status);
    // pthread_join(cpuThread, NULL);
    // pthread_join(graphicsThread, NULL);
    // pthread_join(clockThread, NULL);
    // pthread_barrier_destroy(&barrier);

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

// // Emulates the cpu by excuting instructions at the appropriate times
// void *EmulateCPU(void *params)
// {
//     // Fetch actual parameters from params
//     void **paramList = (void **)params;
//     bool *quit = (bool *)paramList[0];
//     FILE *output = (FILE *)paramList[1];

//     //Uint64 instStart = 0;   // Stores the timestamp of when an instruct begins
//     short cycles = 0;       // The number of cycles consumed by an instruction
//     BYTE opcode = 0x00;  

//     pthread_barrier_wait(&barrier);
//     do
//     {
//         opcode = FetchByte(output);
//         cycles = DecodeExecute(opcode, output);
//         fprintf(output, " (%d cycles)\n", cycles);

//         while(cycleCounter < cycles);
//         cycleCounter = 0;

//     } while(!(*quit));

//     // // Point function pointer to ExecuteInst() and set parameters to pass
//     // void *(*executeInstPtr)(void *params) = ExecuteInst;
//     // void *executeInstArgs[] = { &cycles, output };

//     // // Execute the next instruction at the appropriate time until emulation is ended
//     // // Barrier ensures that cpu, graphics, and input threads begin emulation at the same time
//     // pthread_barrier_wait(&barrier);
//     // while(!(*quit))
//     //     instStart = Timer(instStart, cycles * SEC_PER_CYCLE, executeInstPtr, executeInstArgs);
//     return NULL;
// }

// // Emulates graphical output by drawing frames at the appropriate times
// void *EmulateGraphics(void *params)
// {
//     // Fetch actual parameters from params
//     void **paramList = (void **)params;
//     bool *quit = (bool *)paramList[0];
//     FILE *output = (FILE *)paramList[1];

//     //Uint64 frameStart = 0;  // Stores the timestamp of when a frame begins
//     short count = 0;

//     // Point function pointer to DrawGraphics() and set parameters to pass
//     //void *(*drawGraphicsPtr)(void *params) = DrawGraphics;
//     //void *drawGraphicsArgs[] = { &count, output };

//     // Draw the next frame at the appropriate time until emulation is ended
//     // Barrier ensures that cpu, graphics, and input threads begin emulation at the same time
//     pthread_barrier_wait(&barrier);
//     do
//     {
//         count++;
//         if(count == 60)
//         {
//             count = 0;
//             fprintf(output, "60th frame (about one second)\n");
//         }

//         pthread_mutex_lock(&mutex);
//         while(frameCounter < CYCLES_PER_FRAME)
//         {
//             pthread_cond_wait(&condition, &mutex);
//         }
//         frameCounter = 0;
//         pthread_mutex_unlock(&mutex);

//     } while(!(*quit));
//     // while(!(*quit))
//     //     frameStart = Timer(frameStart, SEC_PER_FRAME, drawGraphicsPtr, drawGraphicsArgs);
//     return NULL;
// }

int EmulateClock(void *params)
{
    void **paramList = (void **)params;
    bool *quit = (bool *)paramList[0];
    
    Uint64 clockStart = 0;

    // Point function pointer to DrawGraphics() and set parameters to pass
    void *(*incrementCountersPtr)(void *params) = IncrementCounters;

    while(!(*quit))
        clockStart = Timer(clockStart, SEC_PER_CYCLE, incrementCountersPtr, NULL);
    
    return 0;
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
    frameCounter++;
    lineCounter++;
    cycleCounter++;
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