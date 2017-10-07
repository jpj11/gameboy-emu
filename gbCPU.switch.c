#include <stdio.h>

#include "gbCPU.h"

void DecodeExecute(WORD opcode)
{
    switch(opcode)
    {
        // BEGIN LOAD BYTE IMMEDIATE (8 CYCLES) //

        case 0x06: printf("LD B, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regBC.hi, memMainRAM[PC.word++]); break;
        case 0x0E: printf("LD C, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regBC.lo, memMainRAM[PC.word++]); break;
        case 0x16: printf("LD D, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regDE.hi, memMainRAM[PC.word++]); break;
        case 0x1E: printf("LD E, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regDE.lo, memMainRAM[PC.word++]); break;
        case 0x26: printf("LD H, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regHL.hi, memMainRAM[PC.word++]); break;
        case 0x2E: printf("LD L, 0x%02X", memMainRAM[PC.word]); LoadByteImmediate(&regHL.lo, memMainRAM[PC.word++]); break;

        // END LOAD BYTE IMMEDIATE (8 CYCLES) //

        // ==================================================================================== //

        // BEGIN LOAD BYTE REGISTER (4 CYCLES) //

        // Destination = A
        case 0x7F: printf("LD A, A"); LoadByteRegister(&regAF.hi, regAF.hi); break;
        case 0x78: printf("LD A, B"); LoadByteRegister(&regAF.hi, regBC.hi); break;
        case 0x79: printf("LD A, C"); LoadByteRegister(&regAF.hi, regBC.lo); break;
        case 0x7A: printf("LD A, D"); LoadByteRegister(&regAF.hi, regDE.hi); break;
        case 0x7B: printf("LD A, E"); LoadByteRegister(&regAF.hi, regDE.lo); break;
        case 0x7C: printf("LD A, H"); LoadByteRegister(&regAF.hi, regHL.hi); break;
        case 0x7D: printf("LD A, L"); LoadByteRegister(&regAF.hi, regHL.lo); break;

        // Destination = B
        case 0x40: printf("LD B, B"); LoadByteRegister(&regBC.hi, regBC.hi); break;
        case 0x41: printf("LD B, C"); LoadByteRegister(&regBC.hi, regBC.lo); break;
        case 0x42: printf("LD B, D"); LoadByteRegister(&regBC.hi, regDE.hi); break;
        case 0x43: printf("LD B, E"); LoadByteRegister(&regBC.hi, regDE.lo); break;
        case 0x44: printf("LD B, H"); LoadByteRegister(&regBC.hi, regHL.hi); break;
        case 0x45: printf("LD B, L"); LoadByteRegister(&regBC.hi, regHL.lo); break;

        // Destination = C
        case 0x48: printf("LD C, B"); LoadByteRegister(&regBC.lo, regBC.hi); break;
        case 0x49: printf("LD C, C"); LoadByteRegister(&regBC.lo, regBC.lo); break;
        case 0x4A: printf("LD C, D"); LoadByteRegister(&regBC.lo, regDE.hi); break;
        case 0x4B: printf("LD C, E"); LoadByteRegister(&regBC.lo, regDE.lo); break;
        case 0x4C: printf("LD C, H"); LoadByteRegister(&regBC.lo, regHL.hi); break;
        case 0x4D: printf("LD C, L"); LoadByteRegister(&regBC.lo, regHL.lo); break;

        // Destination = D
        case 0x50: printf("LD D, B"); LoadByteRegister(&regDE.hi, regBC.hi); break;
        case 0x51: printf("LD D, C"); LoadByteRegister(&regDE.hi, regBC.lo); break;
        case 0x52: printf("LD D, D"); LoadByteRegister(&regDE.hi, regDE.hi); break;
        case 0x53: printf("LD D, E"); LoadByteRegister(&regDE.hi, regDE.lo); break;
        case 0x54: printf("LD D, H"); LoadByteRegister(&regDE.hi, regHL.hi); break;
        case 0x55: printf("LD D, L"); LoadByteRegister(&regDE.hi, regHL.lo); break;

        // Destination = E
        case 0x58: printf("LD E, B"); LoadByteRegister(&regDE.lo, regBC.hi); break;
        case 0x59: printf("LD E, C"); LoadByteRegister(&regDE.lo, regBC.lo); break;
        case 0x5A: printf("LD E, D"); LoadByteRegister(&regDE.lo, regDE.hi); break;
        case 0x5B: printf("LD E, E"); LoadByteRegister(&regDE.lo, regDE.lo); break;
        case 0x5C: printf("LD E, H"); LoadByteRegister(&regDE.lo, regHL.hi); break;
        case 0x5D: printf("LD E, L"); LoadByteRegister(&regDE.lo, regHL.lo); break;

        // Destination = H
        case 0x60: printf("LD H, B"); LoadByteRegister(&regHL.hi, regBC.hi); break;
        case 0x61: printf("LD H, C"); LoadByteRegister(&regHL.hi, regBC.lo); break;
        case 0x62: printf("LD H, D"); LoadByteRegister(&regHL.hi, regDE.hi); break;
        case 0x63: printf("LD H, E"); LoadByteRegister(&regHL.hi, regDE.lo); break;
        case 0x64: printf("LD H, H"); LoadByteRegister(&regHL.hi, regHL.hi); break;
        case 0x65: printf("LD H, L"); LoadByteRegister(&regHL.hi, regHL.lo); break;

        // Destination = L
        case 0x68: printf("LD L, B"); LoadByteRegister(&regHL.lo, regBC.hi); break;
        case 0x69: printf("LD L, C"); LoadByteRegister(&regHL.lo, regBC.lo); break;
        case 0x6A: printf("LD L, D"); LoadByteRegister(&regHL.lo, regDE.hi); break;
        case 0x6B: printf("LD L, E"); LoadByteRegister(&regHL.lo, regDE.lo); break;
        case 0x6C: printf("LD L, H"); LoadByteRegister(&regHL.lo, regHL.hi); break;
        case 0x6D: printf("LD L, L"); LoadByteRegister(&regHL.lo, regHL.lo); break;

        // END LOAD BYTE REGISTER (4 CYCLES) //

        // ==================================================================================== //
        
        default: printf("Not Implemented");
    }
}