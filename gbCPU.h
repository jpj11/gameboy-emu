#ifndef GB_CPU
#define GB_CPU

typedef unsigned char BYTE;
typedef char S_BYTE;
typedef unsigned short WORD;
typedef short S_WORD;

typedef enum
{
    false,
    true
} bool;

// Graphics constants
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define CHANNELS 3

// CPU Constants
#define REG_P1 0xFF00

// Memory Constants
#define GAME_PAK_SIZE 0x200000
#define MAIN_MEM_SIZE 0x10000

// The memory in a Gameboy cartridge
BYTE gamePakMem[GAME_PAK_SIZE];

// The Gameboy's in unit main memory
BYTE mainMemory[MAIN_MEM_SIZE];

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
union cpuReg SP;  // Stack Pointer

// Buffer that represents the screen's state at any given time
BYTE screenData[SCREEN_HEIGHT][SCREEN_WIDTH][CHANNELS];

// The type of an operand in an instruction
enum operandType
{
    reg,
    immediate,
    memory,
    regOffset,
    immediateOffset,
    memAtImmediate
};

// Flags stored in the most significant bits of register F
enum cpuFlag
{
    zero = 7,
    subtract = 6,
    halfCarry = 5,
    carry = 4
};

// Primary execution functions
BYTE FetchByte(FILE *output);
WORD FetchWord(FILE *output);
short DecodeExecute(BYTE opcode, FILE *output);

// Load instructions
short LoadByte(BYTE *dest, enum operandType destType, BYTE src, enum operandType srcType);
short LoadWord(WORD *dest, WORD src, enum operandType srcType);
short Push(WORD value);
short Pop(WORD *dest);

// Byte arithmetic instructions
short Subtract(BYTE value, enum operandType valueType);
short Xor(BYTE value, enum operandType valueType);
short Compare(BYTE value, enum operandType valueType);
short IncrementByte(BYTE *value, enum operandType valueType);
short DecrementByte(BYTE *value, enum operandType valueType);

// Word arithmetic instructions
short IncrementWord(WORD *value);
short DecrementWord(WORD *value);

// Rotate and shift instructions
short RotateAccuLeftThruCarry();
short RotateAccuRightThruCarry();
short RotateLeft(BYTE *value, enum operandType valueType);

// Bitwise instructions
short Bit(short bit, BYTE *toTest, enum operandType toTestType);

// Jump instructions
short JumpRelative(S_BYTE offset);
short JumpRelativeCond(enum cpuFlag flag, bool condition, S_BYTE offset);
short Call(WORD address);
short Return();

#endif