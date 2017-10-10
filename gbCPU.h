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
#define REG_P1 0xFF00

// Memory Constants
#define GAME_PAK_SIZE 0x200000
#define MAIN_MEM_SIZE 0x10000

// The memory in a Gameboy cartridge
BYTE memGamePak[GAME_PAK_SIZE];

// The Gameboy's in unit main memory
BYTE memMainRAM[MAIN_MEM_SIZE];

// A union that eases access to hi and lo BYTES of a WORD
union cpuReg
{
    WORD word;
    struct byte
    {
        BYTE lo;
        BYTE hi;
    };
};

// Main CPU registers
union cpuReg regAF;
union cpuReg regBC;
union cpuReg regDE;
union cpuReg regHL;

union cpuReg PC;  // Program Counter
union cpuReg SP;  // Stack Pointer. cpuReg allows easier access to hi and lo BYTES

// Buffer that represents the screen's state at any given time
BYTE screenData[SCREEN_HEIGHT][SCREEN_WIDTH][CHANNELS];

void InitSystem();
WORD Fetch();
void DecodeExecute(WORD opcode, FILE *output);

BYTE ReadByte(WORD address, FILE *output);
int WriteByte(WORD address, BYTE data, FILE *output);

WORD ReadWord(WORD address, FILE *output);

void LoadByte(BYTE *dest, BYTE source, unsigned int cycles, FILE *output);
void StoreByte(WORD address, BYTE data, unsigned int cycles, FILE *output);

#endif