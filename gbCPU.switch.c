#include <stdio.h>

#include "gbCPU.h"

void DecodeExecute(WORD opcode)
{
    switch(opcode)
    {
        // BEGIN LOAD BYTE IMMEDIATE INTO REGISTER (8 CYCLES) //

        case 0x06: printf("LD B, 0x%02X", memMainRAM[PC.word]); LoadByte(&regBC.hi, memMainRAM[PC.word++], 8); break;
        case 0x0E: printf("LD C, 0x%02X", memMainRAM[PC.word]); LoadByte(&regBC.lo, memMainRAM[PC.word++], 8); break;
        case 0x16: printf("LD D, 0x%02X", memMainRAM[PC.word]); LoadByte(&regDE.hi, memMainRAM[PC.word++], 8); break;
        case 0x1E: printf("LD E, 0x%02X", memMainRAM[PC.word]); LoadByte(&regDE.lo, memMainRAM[PC.word++], 8); break;
        case 0x26: printf("LD H, 0x%02X", memMainRAM[PC.word]); LoadByte(&regHL.hi, memMainRAM[PC.word++], 8); break;
        case 0x2E: printf("LD L, 0x%02X", memMainRAM[PC.word]); LoadByte(&regHL.lo, memMainRAM[PC.word++], 8); break;

        // END LOAD BYTE IMMEDIATE INTO REGISTER (8 CYCLES) //

        // ==================================================================================== //

        // BEGIN LOAD BYTE REGISTER INTO REGISTER (4 CYCLES) //

        // Destination = A
        case 0x7F: printf("LD A, A"); LoadByte(&regAF.hi, regAF.hi, 4); break;
        case 0x78: printf("LD A, B"); LoadByte(&regAF.hi, regBC.hi, 4); break;
        case 0x79: printf("LD A, C"); LoadByte(&regAF.hi, regBC.lo, 4); break;
        case 0x7A: printf("LD A, D"); LoadByte(&regAF.hi, regDE.hi, 4); break;
        case 0x7B: printf("LD A, E"); LoadByte(&regAF.hi, regDE.lo, 4); break;
        case 0x7C: printf("LD A, H"); LoadByte(&regAF.hi, regHL.hi, 4); break;
        case 0x7D: printf("LD A, L"); LoadByte(&regAF.hi, regHL.lo, 4); break;

        // Destination = B
        case 0x40: printf("LD B, B"); LoadByte(&regBC.hi, regBC.hi, 4); break;
        case 0x41: printf("LD B, C"); LoadByte(&regBC.hi, regBC.lo, 4); break;
        case 0x42: printf("LD B, D"); LoadByte(&regBC.hi, regDE.hi, 4); break;
        case 0x43: printf("LD B, E"); LoadByte(&regBC.hi, regDE.lo, 4); break;
        case 0x44: printf("LD B, H"); LoadByte(&regBC.hi, regHL.hi, 4); break;
        case 0x45: printf("LD B, L"); LoadByte(&regBC.hi, regHL.lo, 4); break;

        // Destination = C
        case 0x48: printf("LD C, B"); LoadByte(&regBC.lo, regBC.hi, 4); break;
        case 0x49: printf("LD C, C"); LoadByte(&regBC.lo, regBC.lo, 4); break;
        case 0x4A: printf("LD C, D"); LoadByte(&regBC.lo, regDE.hi, 4); break;
        case 0x4B: printf("LD C, E"); LoadByte(&regBC.lo, regDE.lo, 4); break;
        case 0x4C: printf("LD C, H"); LoadByte(&regBC.lo, regHL.hi, 4); break;
        case 0x4D: printf("LD C, L"); LoadByte(&regBC.lo, regHL.lo, 4); break;

        // Destination = D
        case 0x50: printf("LD D, B"); LoadByte(&regDE.hi, regBC.hi, 4); break;
        case 0x51: printf("LD D, C"); LoadByte(&regDE.hi, regBC.lo, 4); break;
        case 0x52: printf("LD D, D"); LoadByte(&regDE.hi, regDE.hi, 4); break;
        case 0x53: printf("LD D, E"); LoadByte(&regDE.hi, regDE.lo, 4); break;
        case 0x54: printf("LD D, H"); LoadByte(&regDE.hi, regHL.hi, 4); break;
        case 0x55: printf("LD D, L"); LoadByte(&regDE.hi, regHL.lo, 4); break;

        // Destination = E
        case 0x58: printf("LD E, B"); LoadByte(&regDE.lo, regBC.hi, 4); break;
        case 0x59: printf("LD E, C"); LoadByte(&regDE.lo, regBC.lo, 4); break;
        case 0x5A: printf("LD E, D"); LoadByte(&regDE.lo, regDE.hi, 4); break;
        case 0x5B: printf("LD E, E"); LoadByte(&regDE.lo, regDE.lo, 4); break;
        case 0x5C: printf("LD E, H"); LoadByte(&regDE.lo, regHL.hi, 4); break;
        case 0x5D: printf("LD E, L"); LoadByte(&regDE.lo, regHL.lo, 4); break;

        // Destination = H
        case 0x60: printf("LD H, B"); LoadByte(&regHL.hi, regBC.hi, 4); break;
        case 0x61: printf("LD H, C"); LoadByte(&regHL.hi, regBC.lo, 4); break;
        case 0x62: printf("LD H, D"); LoadByte(&regHL.hi, regDE.hi, 4); break;
        case 0x63: printf("LD H, E"); LoadByte(&regHL.hi, regDE.lo, 4); break;
        case 0x64: printf("LD H, H"); LoadByte(&regHL.hi, regHL.hi, 4); break;
        case 0x65: printf("LD H, L"); LoadByte(&regHL.hi, regHL.lo, 4); break;

        // Destination = L
        case 0x68: printf("LD L, B"); LoadByte(&regHL.lo, regBC.hi, 4); break;
        case 0x69: printf("LD L, C"); LoadByte(&regHL.lo, regBC.lo, 4); break;
        case 0x6A: printf("LD L, D"); LoadByte(&regHL.lo, regDE.hi, 4); break;
        case 0x6B: printf("LD L, E"); LoadByte(&regHL.lo, regDE.lo, 4); break;
        case 0x6C: printf("LD L, H"); LoadByte(&regHL.lo, regHL.hi, 4); break;
        case 0x6D: printf("LD L, L"); LoadByte(&regHL.lo, regHL.lo, 4); break;

        // END LOAD BYTE REGISTER INTO REGISTER (4 CYCLES) //

        // ==================================================================================== //
        
        // BEGIN LOAD BYTE REGISTER INTO MEMORY (8 CYCLES) //



        // END LOAD BYTE REGISTER INTO MEMORY (8 CYCLES) //

        // ==================================================================================== //
        
        default: printf("Not Implemented");
    }
}