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

#define VERTICAL_SYNC 59.73
extern const long double SEC_PER_FRAME;

// CPU Constants
#define CLOCK_SPEED 4194304
extern const long double SEC_PER_CYCLE;
extern const short TIMA_SPEED[];
extern bool IME;
extern const WORD INTERRUPT_VECTORS[];

#define DIV_SPEED 256
#define TAC_ZERO  1024
#define TAC_ONE   16
#define TAC_TWO   64
#define TAC_THREE 256

#define REG_P1   0xff00

#define REG_DIV  0xff04
#define REG_TIMA 0xff05
#define REG_TMA  0xff06
#define REG_TAC  0xff07

#define REG_IE   0xffff
#define REG_IF   0xff0f

#define VBLANK_VECT   0x0040
#define LCD_STAT_VECT 0x0048
#define TIMER_VECT    0x0050
#define SERIAL_VECT   0x0058
#define JOYPAD_VECT   0x0060

// The memory in a Gameboy cartridge
BYTE *gamePakMem;

// Memory Constants
#define MAIN_MEM_SIZE 0x10000

// The Gameboy's in unit main memory
extern BYTE mainMemory[MAIN_MEM_SIZE];

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

// Possible interrupts that can be raised
enum interrupt
{
    vblank,
    lcd_stat,
    timer,
    serial,
    joypad
};

// Primary execution functions
void InitSystem();
BYTE FetchByte(FILE *output);
WORD FetchWord(FILE *output);
short DecodeExecute(BYTE opcode, FILE *output);

void Write(BYTE *dest, BYTE src);
void RequestInterrupt(enum interrupt requested);
bool IsRequested(enum interrupt toCheck);
bool IsEnabled(enum interrupt toCheck);

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