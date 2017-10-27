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
    // Retrieve value off of stack
    BYTE lo = mainMemory[++SP.word];
    *dest = mainMemory[++SP.word];

    // Form WORD dest from hi and lo BYTEs
    *dest <<= 8;
    *dest |= lo;

    // If dest is regsiter AF, then set and unset flags as necessary
    if (dest == &regAF.word)
    {
        *dest == 0x0000 ? SetFlag(zero) : UnsetFlag(zero);
        UnsetFlag(subtract);
        *dest > 0x000f ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
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
    mainMemory[SP.word--] = PC.hi;
    mainMemory[SP.word--] = PC.lo;

    // Jump to address
    PC.word = address;

    return 24;
}

short Return()
{
    // Load return address from stack into PC
    PC.lo = mainMemory[++SP.word];
    PC.hi = mainMemory[++SP.word];

    return 16;
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

    // Set and unset flags as necessary
    *value == 0x00 ? SetFlag(zero) : UnsetFlag(zero);
    UnsetFlag(subtract);
    *value > 0x0f ? SetFlag(halfCarry) : UnsetFlag(halfCarry);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 12;
}

short Subtract(BYTE value, enum operandType valueType)
{
    // Set flags based on operands
    (regAF.hi & 0x0f) < (value & 0x0f) ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    regAF.hi < value ? SetFlag(carry) : UnsetFlag(carry);
    SetFlag(subtract);

    // Calculate the difference
    regAF.hi -= value;
    
    // Set flags based on result
    regAF.hi == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

short Xor(BYTE value, enum operandType valueType)
{
    // Xor register A with value and store result in register A
    regAF.hi ^= value;

    // Unset all flags
    regAF.lo = 0x00;

    // Set and unset flags as necessary
    regAF.hi == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

short Compare(BYTE value, enum operandType valueType)
{
    // Set flags based on operands
    (regAF.hi & 0x0f) < (value & 0x0f) ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    regAF.hi < value ? SetFlag(carry) : UnsetFlag(carry);
    SetFlag(subtract);

    // Calculate the comparison
    BYTE result = regAF.hi - value;

    // Set flags based on result
    result == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

short DecrementByte(BYTE *value, enum operandType valueType)
{
    // Set flags based on operands
    (*value & 0x0f) < 1 ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    *value < 1 ? SetFlag(carry) : UnsetFlag(carry);
    SetFlag(subtract);

    // Decrement byte
    *value = *value - 1;

    // Set flags based on result
    *value == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 12;
}

short Bit(short position, BYTE *toTest, enum operandType toTestType)
{
    // Find the value of the bit at position in toTest
    short value = (*toTest >> position) & 1;

    // Set and unset flags as necessary
    value == 0 ? SetFlag(zero) : UnsetFlag(zero);
    UnsetFlag(subtract);
    SetFlag(halfCarry);

    // Return appropriate number of cycles
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
    *value == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

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