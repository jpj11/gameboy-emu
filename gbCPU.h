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

static const long double SEC_PER_FRAME = 0.01674200569228193537585802779173;

// CPU Constants
#define REG_P1 0xFF00

// Memory Constants
#define MAIN_MEM_SIZE 0x10000

// The memory in a Gameboy cartridge
BYTE *gamePakMem;

// The Gameboy's in unit main memory
static BYTE mainMemory[MAIN_MEM_SIZE] = 
    "\x31\xFE\xFF\xAF\x21\xFF\x9F\x32\xCB\x7C\x20\xFB\x21\x26\xFF\x0E"
    "\x11\x3E\x80\x32\xE2\x0C\x3E\xF3\xE2\x32\x3E\x77\x77\x3E\xFC\xE0"
    "\x47\x11\x04\x01\x21\x10\x80\x1A\xCD\x95\x00\xCD\x96\x00\x13\x7B"
    "\xFE\x34\x20\xF3\x11\xD8\x00\x06\x08\x1A\x13\x22\x23\x05\x20\xF9"
    "\x3E\x19\xEA\x10\x99\x21\x2F\x99\x0E\x0C\x3D\x28\x08\x32\x0D\x20"
    "\xF9\x2E\x0F\x18\xF3\x67\x3E\x64\x57\xE0\x42\x3E\x91\xE0\x40\x04"
    "\x1E\x02\x0E\x0C\xF0\x44\xFE\x90\x20\xFA\x0D\x20\xF7\x1D\x20\xF2"
    "\x0E\x13\x24\x7C\x1E\x83\xFE\x62\x28\x06\x1E\xC1\xFE\x64\x20\x06"
    "\x7B\xE2\x0C\x3E\x87\xE2\xF0\x42\x90\xE0\x42\x15\x20\xD2\x05\x20"
    "\x4F\x16\x20\x18\xCB\x4F\x06\x04\xC5\xCB\x11\x17\xC1\xCB\x11\x17"
    "\x05\x20\xF5\x22\x23\x22\x23\xC9\xCE\xED\x66\x66\xCC\x0D\x00\x0B"
    "\x03\x73\x00\x83\x00\x0C\x00\x0D\x00\x08\x11\x1F\x88\x89\x00\x0E"
    "\xDC\xCC\x6E\xE6\xDD\xDD\xD9\x99\xBB\xBB\x67\x63\x6E\x0E\xEC\xCC"
    "\xDD\xDC\x99\x9F\xBB\xB9\x33\x3E\x3C\x42\xB9\xA5\xB9\xA5\x42\x3C"
    "\x21\x04\x01\x11\xA8\x00\x1A\x13\xBE\x20\xFE\x23\x7D\xFE\x34\x20"
    "\xF5\x06\x19\x78\x86\x23\x05\x20\xFB\x86\x20\xFE\x3E\x01\xE0\x50";

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
short AddByte(BYTE value, enum operandType valueType);
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
short RotateLeftThruCarry(BYTE *value, enum operandType valueType);

// Bitwise instructions
short Bit(short bit, BYTE *toTest, enum operandType toTestType);

// Jump instructions
short JumpRelative(S_BYTE offset);
short JumpRelativeCond(enum cpuFlag flag, bool condition, S_BYTE offset);
short Call(WORD address);
short Return();

#endif