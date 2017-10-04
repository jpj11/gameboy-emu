typedef unsigned char BYTE;
typedef char S_BYTE;
typedef unsigned short WORD;
typedef short S_WORD;

// Graphics constants
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define CHANNELS 3

// CPU Constants
#define CPU_FLAG_ZERO 7 // Zero flag
#define CPU_FLAG_N 6    // Add/Sub flag (BCD)
#define CPU_FLAG_H 5    // Half carry flag (BCD)
#define CPU_FLAG_CY 4   // Carry flag

// Memory Constants
#define GAME_PAK_SIZE 0x200000
#define MAIN_MEM_SIZE 0x10000

// The memory in a Gameboy cartridge
BYTE memGamePak[GAME_PAK_SIZE];

// The Gameboy's in unit main memory
BYTE memMainRAM[MAIN_MEM_SIZE];

union cpuReg
{
    WORD val;
    struct
    {
        BYTE lo;
        BYTE hi;
    };
};

cpuReg regAF;
cpuReg regBC;
cpuReg regDE;
cpuReg regHL;

WORD PC;
cpuReg SP;

BYTE screenData[SCREEN_HEIGHT][SCREEN_WIDTH][CHANNELS];