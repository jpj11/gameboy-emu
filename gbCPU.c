#include <stdio.h>
#include <string.h>
#include "gbCPU.h"


// ================= Primary Execution / Helper Functions ================= //

// Fetch the next BYTE from memory at PC
BYTE FetchByte(FILE *output)
{
    BYTE val = mainMemory[PC.word++];
    fprintf(output, "%0#4x --> ", val);
    return val;
}

// Fetch the next WORD from memory at PC
WORD FetchWord(FILE *output)
{
    WORD val = FetchByte(output);
    val |= FetchByte(output) << 8;
    return val;
}

// Functions to get, set, and unset cpuFlags
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


// ========================== Load Instructions =========================== //

// Load BYTE from src into address dest
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

// Load WORD from src into address dest
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

// Push WORD value onto the stack
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

// Pop WORD of off stack into address dest
short Pop(WORD *dest)
{
    // Retrieve value off of stack
    BYTE lo = mainMemory[++SP.word];
    *dest = mainMemory[++SP.word];

    // Form WORD dest from hi and lo BYTEs
    *dest <<= 8;
    *dest |= lo;

    return 12;
}


// ===================== Byte Arithmetic Instructions ===================== //

// Add BYTE value to register A and store sum in A
short AddByte(BYTE value, enum operandType valueType)
{
    // Set flags based on operands
    UnsetFlag(subtract);
    (regAF.hi & 0x0f) > 0x0f - (value & 0x0f) ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    regAF.hi > 0xff - value ? SetFlag(carry) : UnsetFlag(carry);

    // Calculate the sum
    regAF.hi += value;

    // Set flags based on result
    regAF.hi == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

// Subtract BYTE value from register A and store difference in A
short Subtract(BYTE value, enum operandType valueType)
{
    // Set flags based on operands
    SetFlag(subtract);
    (regAF.hi & 0x0f) < (value & 0x0f) ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    regAF.hi < value ? SetFlag(carry) : UnsetFlag(carry);

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

// Xor BYTE value with register A and store result in A
short Xor(BYTE value, enum operandType valueType)
{
    // Unset all flags
    regAF.lo = 0x00;

    // Xor register A with value and store result in register A
    regAF.hi ^= value;

    // Set and unset flags as necessary
    regAF.hi == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 8;
}

// Compare BYTE value with register A and set flags based on result
short Compare(BYTE value, enum operandType valueType)
{
    // Set flags based on operands
    SetFlag(subtract);
    (regAF.hi & 0x0f) < (value & 0x0f) ? SetFlag(halfCarry) : UnsetFlag(halfCarry);
    regAF.hi < value ? SetFlag(carry) : UnsetFlag(carry);

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

// Increment the BYTE at address value
short IncrementByte(BYTE *value, enum operandType valueType)
{
    // Set flags based on operands
    UnsetFlag(subtract);
    (*value & 0x0f) > 0x0e ? SetFlag(halfCarry) : UnsetFlag(halfCarry);

    // Increment byte
    *value = *value + 1;

    // Set flags based on result
    *value == 0x00 ? SetFlag(zero) : UnsetFlag(zero);

    // Return the appropriate number of cycles
    if(valueType == reg)
        return 4;
    else
        return 12;
}

// Decrement the BYTE at address value
short DecrementByte(BYTE *value, enum operandType valueType)
{
    // Set flags based on operands
    SetFlag(subtract);
    (*value & 0x0f) < 0x01 ? SetFlag(halfCarry) : UnsetFlag(halfCarry);

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


// ===================== Word Arithmetic Instructions ===================== //

// Increment WORD at address value
short IncrementWord(WORD *value)
{
    // Increment word by 1
    *value = *value + 1;
    return 8;
}

// Decrement WORD at address value
short DecrementWord(WORD *value)
{
    // Decrement word by 1
    *value = *value + 1;
    return 8;
}


// ==================== Rotate and Shift Instructions ===================== //

// Rotate the bits of register A left through the carry flag
short RotateAccuLeftThruCarry()
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

// Rotate the bits of register A right through the carry flag
short RotateAccuRightThruCarry()
{
    // Stor the state of the carry flag
    short bitSeven = GetFlag(carry);

    // Clear flags
    regAF.lo = 0x00;

    // Set the flag if the least significant bit of accumulator is 1
    if(regAF.hi & 0x01)
        SetFlag(carry);

    // Shift right and put old value of carry flag at bit 7
    regAF.hi >>= 1;
    regAF.hi |= bitSeven << 7;

    return 4;
}

// Rotate the bits of the BYTE at address value left through the carry flag
short RotateLeftThruCarry(BYTE *value, enum operandType valueType)
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


// ========================= Bitwise Instructions ========================= //

// Test the bit at position position of the BYTE at address toTest
short Bit(short position, BYTE *toTest, enum operandType toTestType)
{
    // Set flags based on operands
    UnsetFlag(subtract);
    SetFlag(halfCarry);

    // Find the value of the bit at position in toTest
    short value = (*toTest >> position) & 1;

    // Set flags based on results
    value == 0 ? SetFlag(zero) : UnsetFlag(zero);

    // Return appropriate number of cycles
    if (toTestType == reg)
        return 8;
    else
        return 12;
}


// =========================== Jump Instructions ========================== //

// Jump to PC + offset
short JumpRelative(S_BYTE offset)
{
    // Jump to PC + offset
    PC.word += offset;
    return 12;
}

// Jump to PC + offset if the flag and condition match
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

// Push return address on stack and jump to address
short Call(WORD address)
{
    // Store the address of the next instruction on the stack
    mainMemory[SP.word--] = PC.hi;
    mainMemory[SP.word--] = PC.lo;

    // Jump to address
    PC.word = address;

    return 24;
}

// Set PC to address popped off of stack
short Return()
{
    // Load return address from stack into PC
    PC.lo = mainMemory[++SP.word];
    PC.hi = mainMemory[++SP.word];

    return 16;
}