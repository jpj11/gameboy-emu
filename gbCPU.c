#include <stdio.h>
#include <string.h>
#include "gbCPU.h"

bool GetFlag(enum cpuFlag flag)
{
    return (regAF.lo >> flag) & 1;
}

void SetFlag(enum cpuFlag flag)
{
    regAF.lo |= 1 << flag;
}

void UnsetFlag(enum cpuFlag flag)
{
    regAF.lo &= ~(1 << flag);
}

void ModifyFlag(enum cpuFlag flag, WORD value)
{
    switch (flag)
    {
        case zero:
            if(value == 0)
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
            if(value > 0x00FF)
                SetFlag(carry);
            else
                UnsetFlag(carry);
            break;

        default: break;
    }
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

short LoadByte(BYTE *dest, enum operandType destType, BYTE src, enum operandType srcType)
{
    // Load source into destination
    *dest = src;

    // Return the appropriate number of cycles
    if(destType == reg && srcType == reg)
        return 4;

    else if((destType == memory && srcType == immediate) ||
            (destType == reg && srcType == immediateOffset) ||
            (destType == immediateOffset && srcType == reg))
        return 12;

    else if((destType == reg && srcType == memAtImmediate) ||
            (destType == memAtImmediate && srcType == reg))
        return 16;

    else
        return 8;
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

short Push(WORD value)
{
    // Calculate the hi and lo bytes of value
    BYTE hi = value >> 8;
    BYTE lo = value;

    // Store value on the stack
    mainMemory[SP.word--] = hi;
    mainMemory[SP.word--] = lo;

    return 16;
}

short Pop(WORD *dest)
{
    BYTE lo = mainMemory[SP.word++];
    *dest = mainMemory[SP.word++];

    *dest <<= 8;
    *dest |= lo;

    if (dest == &regAF.word)
    {
        ModifyFlag(zero, *dest);
        UnsetFlag(subtract);
        ModifyFlag(halfCarry, *dest);
        UnsetFlag(carry);
    }

    return 12;
}

short JumpRelativeCond(enum cpuFlag flag, bool condition, S_BYTE offset)
{
    // Get the state of the flag
    bool flagIsSet = GetFlag(flag);

    // If the flag state matches condition, jump
    if( (flagIsSet == true  && condition == true) ||
        (flagIsSet == false && condition == false) )
    {
        PC.word += offset;
        return 12;
    }
    else
        return 8;
}

short Call(WORD address)
{
    // Store the address of the next instruction on the stack
    PC.word++;
    mainMemory[SP.word--] = PC.hi;
    mainMemory[SP.word--] = PC.lo;

    // Jump to address
    PC.word = address;

    return 24;
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

short Bit(short position, BYTE *toTest, enum operandType toTestType)
{
    // Find the value of the bit at position in toTest
    short value = (*toTest >> position) & 1;

    // Set zero flag if value is zero, otherwise unset
    ModifyFlag(zero, value);

    // Set and unset other flags as necessary
    UnsetFlag(subtract);
    SetFlag(halfCarry);

    if (toTestType == reg)
        return 8;
    else
        return 12;
}

short RotateLeft(BYTE *value, enum operandType valueType)
{
    // Store the state of the carry flag
    short bitZero = GetFlag(carry);

    // Clear flags
    regAF.lo = 0x00;

    // Set the flag if most significant bit of value is 1
    if(*value >> 7)
        SetFlag(carry);

    // Shift left and put old value of carry flag at bit 0
    *value <<= 1;
    *value |= bitZero;

    // Set zero flag if rotation resulted in zero
    ModifyFlag(zero, *value);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 8;
    else
        return 16;
}

short RotateLeftAccu()
{
    // Store the state of the carry flag
    short bitZero = GetFlag(carry);

    // Clear flags
    regAF.lo = 0x00;

    // Set the flag if most significant bit of accumulator is 1
    if(regAF.hi >> 7)
        SetFlag(carry);

    // Shift left and put old value of carry flag at bit 0
    regAF.hi <<= 1;
    regAF.hi |= bitZero;

    return 4;
}