#include <stdio.h>

#include "gbCPU.h"

void DecodeExecute(WORD opcode)
{
    switch(opcode)
    {
        // Load byte immediate
        case 0x06: printf("LD B, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regBC.hi, memMainRAM[PC.word++]); break;
        case 0x0E: printf("LD C, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regBC.lo, memMainRAM[PC.word++]); break;
        case 0x16: printf("LD D, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regDE.hi, memMainRAM[PC.word++]); break;
        case 0x1E: printf("LD E, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regDE.lo, memMainRAM[PC.word++]); break;
        case 0x26: printf("LD H, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regHL.hi, memMainRAM[PC.word++]); break;
        case 0x2E: printf("LD L, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regHL.lo, memMainRAM[PC.word++]); break;

        default: printf("Not Implemented");
    }
}