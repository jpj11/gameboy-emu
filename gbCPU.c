#include <stdio.h>
#include <string.h>
#include "gbCPU.h"

// int IsRegister(WORD *address)
// {
//     if( (&(*address) == &regAF.word) ||
//         (&(*address) == &regBC.word) ||
//         (&(*address) == &regDE.word) ||
//         (&(*address) == &regHL.word) ||
//         (&(*address) ==    &SP.word) ||
//         (&(*address) ==    &PC.word) )
//     {
//         return 1;
//     }
//     else
//     {
//         return -1;
//     }
// }

BYTE *Write(BYTE *address)
{
    return NULL;
}

void SetFlag(enum cpuFlag flag)
{
    regAF.lo |= 1 << flag;
}

void UnsetFlag(enum cpuFlag flag)
{
    regAF.lo &= ~(1 << flag);
}

void ModifyFlag(enum cpuFlag flag, BYTE value)
{
    switch (flag)
    {
        case zero:
            if(value == 0x00)
                SetFlag(zero);
            else
                UnsetFlag(zero);
            break;

        case subtract: break;

        case halfCarry:
            if(value > 0xF)
                SetFlag(halfCarry);
            else
                UnsetFlag(halfCarry);
            break;

        case carry:
            if(value > 0xFF)
                SetFlag(carry);
            else
                UnsetFlag(carry);
            break;

        default: break;
    }
}

// (http://bgb.bircd.org/pandocs.htm#powerupsequence)
void InitSystem()
{
    // Initialize registers
    regAF.word = 0x01B0;
    regBC.word = 0x0013;
    regDE.word = 0x00D8;
    regHL.word = 0x014D;

    // Initialize program counter and stack pointer
    PC.word = 0x0100;
    SP.word = 0xFFFE;

    // Initialize RAM (I/0 Special Registers)
    mainMemory[0xFF05] = 0x00;  // TIMA
    mainMemory[0xFF06] = 0x00;  // TMA
    mainMemory[0xFF07] = 0x00;  // TAC
    mainMemory[0xFF10] = 0x80;  // NR10
    mainMemory[0xFF11] = 0xBF;  // NR11
    mainMemory[0xFF12] = 0xF3;  // NR12
    mainMemory[0xFF14] = 0xBF;  // NR14
    mainMemory[0xFF16] = 0x3F;  // NR21
    mainMemory[0xFF17] = 0x00;  // NR22
    mainMemory[0xFF19] = 0xBF;  // NR24
    mainMemory[0xFF1A] = 0x7F;  // NR30
    mainMemory[0xFF1B] = 0xFF;  // NR31
    mainMemory[0xFF1C] = 0x9F;  // NR32
    mainMemory[0xFF1E] = 0xBF;  // NR33
    mainMemory[0xFF20] = 0xFF;  // NR41
    mainMemory[0xFF21] = 0x00;  // NR42
    mainMemory[0xFF22] = 0x00;  // NR43
    mainMemory[0xFF23] = 0xBF;  // NR30
    mainMemory[0xFF24] = 0x77;  // NR50
    mainMemory[0xFF25] = 0xF3;  // NR51
    mainMemory[0xFF26] = 0xF1;  // NR52
    mainMemory[0xFF40] = 0x91;  // LCDC
    mainMemory[0xFF42] = 0x00;  // SCY
    mainMemory[0xFF43] = 0x00;  // SCX
    mainMemory[0xFF45] = 0x00;  // LYC
    mainMemory[0xFF47] = 0xFC;  // BGP
    mainMemory[0xFF48] = 0xFF;  // OBP0
    mainMemory[0xFF49] = 0xFF;  // OBP1
    mainMemory[0xFF4A] = 0x00;  // WY
    mainMemory[0xFF4B] = 0x00;  // WX
    mainMemory[0xFFFF] = 0x00;  // IE
}

// Fetch the next opcode to be executed
BYTE Fetch(FILE *output)
{
    BYTE val = mainMemory[PC.word++];
    fprintf(output, "%0#4x --> ", val);
    return val;
}

WORD GetImmediateWord(FILE *output)
{
    WORD val = Fetch(output);
    val |= Fetch(output) << 8;
    return val;
}

short LoadWord(WORD *dest, WORD src, enum operandType srcType)
{
    // Load source into destination
    *dest = src;

    // Return the appropriate number of cycles
    if(srcType == reg)
        return 8;
    else
        return 12;
}

short LoadByte(BYTE *dest, enum operandType destType, BYTE src, enum operandType srcType)
{
    // Load source into destination
    *dest = src;

    // Return the appropriate number of cycles
    if(destType == reg && srcType == reg)
    {
        return 4;
    }
    else if( (destType == memory && srcType == reg) ||
             (destType == reg && srcType == memory) ||
             (destType == reg && srcType == immediate) )
    {
        return 8;
    }
    else
    {
        return 12;
    }
}

short JumpRelativeCond(char *cond, S_BYTE offset)
{
    short zFlag = (regAF.lo >> zero) & 1;
    short cFlag = (regAF.lo >> carry) & 1;

    if( ((strcmp(cond, "Z" ) == 0) &&  zFlag) ||
        ((strcmp(cond, "NZ") == 0) && !zFlag) ||
        ((strcmp(cond, "C" ) == 0) &&  cFlag) ||
        ((strcmp(cond, "NC") == 0) && !cFlag) )
    {
        PC.word += offset;
    }

    return 8;
}

short IncrementWord(WORD *value)
{
    // Increment word by 1
    *value = *value + 1;
    return 8;
}

short IncrementByte(BYTE *value, enum operandType valueType)
{
    // Increment byte
    *value = *value + 1;

    // Set zero flag if the operation resulted in zero, otherwise unset
    ModifyFlag(zero, *value);

    // Unset subtract flag
    UnsetFlag(subtract);

    // Set half carry flag if the operation resulted in a half carry
    ModifyFlag(halfCarry, *value);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 12;
}

short Xor(BYTE value, enum operandType valueType)
{
    // Xor register A with value and store result in register A
    regAF.hi ^= value;

    // Unset all flags
    regAF.lo = 0x00;

    // Set zero flag if the operation resulted in zero, otherwise unset
    ModifyFlag(zero, regAF.hi);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

short Bit(short position, BYTE *toTest)
{
    // Find the value of the bit at position in toTest
    short value = (*toTest >> position) & 1;

    // Set zero flag if value is zero, otherwise unset
    ModifyFlag(zero, value);

    // Set and unset other flags as necessary
    UnsetFlag(subtract);
    SetFlag(halfCarry);

    return 8;
}