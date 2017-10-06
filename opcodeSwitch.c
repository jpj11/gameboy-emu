#include <stdio.h>

#include "gameboyCPU.h"

void DecodeExecute(WORD opcode)
{
    switch(opcode)
    {
        // Load byte immediate
        case 0x06: LoadByteImmediate(regBC.val.hi, memMainRAM[PC.val.word++]); break;
        case 0x0E: LoadByteImmediate(regBC.val.lo, memMainRAM[PC.val.word++]); break;
        case 0x16: LoadByteImmediate(regDE.val.hi, memMainRAM[PC.val.word++]); break;
        case 0x1E: LoadByteImmediate(regDE.val.lo, memMainRAM[PC.val.word++]); break;
        case 0x26: LoadByteImmediate(regHL.val.hi, memMainRAM[PC.val.word++]); break;
        case 0x2E: LoadByteImmediate(regHL.val.lo, memMainRAM[PC.val.word++]); break;

        default: printf("Not Implemented");
    }
}