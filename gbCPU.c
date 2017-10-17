#include <stdio.h>
#include "gbCPU.h"

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
    memMainRAM[0xFF05] = 0x00;  // TIMA
    memMainRAM[0xFF06] = 0x00;  // TMA
    memMainRAM[0xFF07] = 0x00;  // TAC
    memMainRAM[0xFF10] = 0x80;  // NR10
    memMainRAM[0xFF11] = 0xBF;  // NR11
    memMainRAM[0xFF12] = 0xF3;  // NR12
    memMainRAM[0xFF14] = 0xBF;  // NR14
    memMainRAM[0xFF16] = 0x3F;  // NR21
    memMainRAM[0xFF17] = 0x00;  // NR22
    memMainRAM[0xFF19] = 0xBF;  // NR24
    memMainRAM[0xFF1A] = 0x7F;  // NR30
    memMainRAM[0xFF1B] = 0xFF;  // NR31
    memMainRAM[0xFF1C] = 0x9F;  // NR32
    memMainRAM[0xFF1E] = 0xBF;  // NR33
    memMainRAM[0xFF20] = 0xFF;  // NR41
    memMainRAM[0xFF21] = 0x00;  // NR42
    memMainRAM[0xFF22] = 0x00;  // NR43
    memMainRAM[0xFF23] = 0xBF;  // NR30
    memMainRAM[0xFF24] = 0x77;  // NR50
    memMainRAM[0xFF25] = 0xF3;  // NR51
    memMainRAM[0xFF26] = 0xF1;  // NR52
    memMainRAM[0xFF40] = 0x91;  // LCDC
    memMainRAM[0xFF42] = 0x00;  // SCY
    memMainRAM[0xFF43] = 0x00;  // SCX
    memMainRAM[0xFF45] = 0x00;  // LYC
    memMainRAM[0xFF47] = 0xFC;  // BGP
    memMainRAM[0xFF48] = 0xFF;  // OBP0
    memMainRAM[0xFF49] = 0xFF;  // OBP1
    memMainRAM[0xFF4A] = 0x00;  // WY
    memMainRAM[0xFF4B] = 0x00;  // WX
    memMainRAM[0xFFFF] = 0x00;  // IE
}

// Fetch the next opcode to be executed
BYTE Fetch(FILE *output)
{
    BYTE val = memMainRAM[PC.word++];
    fprintf(output, "%0#4x --> ", val);
    return val;
}

WORD GetImmediateWord(FILE *output)
{
    WORD val = Fetch(output);
    val |= Fetch(output) << 8;
    return val;
}