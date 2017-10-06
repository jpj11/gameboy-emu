#ifndef GB_CPU
#define GB_CPU

typedef unsigned char BYTE;
typedef char S_BYTE;
typedef unsigned short WORD;
typedef short S_WORD;

// Graphics constants
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define CHANNELS 3

// CPU Constants
#define CPU_FLAG_Z 7    // Zero flag
#define CPU_FLAG_N 6    // Add/Sub flag (BCD)
#define CPU_FLAG_H 5    // Half carry flag (BCD)
#define CPU_FLAG_C 4    // Carry flag

// Memory Constants
#define GAME_PAK_SIZE 0x200000
#define MAIN_MEM_SIZE 0x10000

// The memory in a Gameboy cartridge
BYTE memGamePak[GAME_PAK_SIZE];

// The Gameboy's in unit main memory
BYTE memMainRAM[MAIN_MEM_SIZE];

// A union that creates the cpuReg type. Eases access to hi and lo BYTES of a WORD
typedef union
{
    WORD word;
    struct
    {
        BYTE lo;
        BYTE hi;
    };
} cpuRegVal;

typedef struct
{
    char *name;
    cpuRegVal val;
} cpuReg;

// Main CPU registers
cpuReg regAF; //regAF.name = "AF";
cpuReg regBC; //regBC.name = "BC";
cpuReg regDE; //regDE.name = "DE";
cpuReg regHL; //regHL.name = "HL";

cpuReg PC;  // Program Counter
cpuReg SP;  // Stack Pointer. cpuReg allows easier access to hi and lo BYTES

// Buffer that represents the screen's state at any given time
BYTE screenData[SCREEN_HEIGHT][SCREEN_WIDTH][CHANNELS];

void InitSystem();
WORD Fetch();
void DecodeExecute(WORD opcode);

void LoadByteImmediate(cpuReg dest, BYTE source);

#endif