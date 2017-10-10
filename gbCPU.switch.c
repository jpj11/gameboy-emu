#include <stdio.h>

#include "gbCPU.h"

#define OPSS "%-4s %-5s %-4s"
#define OPSX "%-4s %-5s %0#4x"

void DecodeExecute(WORD opcode, FILE *output)
{
    switch(opcode)
    {
        // BEGIN LOAD BYTE REGISTER INTO REGISTER (4 CYCLES) //

        // Destination = A
        case 0x7F: fprintf(output, OPSS, "LD", "A,", "A"); LoadByte(&regAF.hi, regAF.hi, 4, output); break;
        case 0x78: fprintf(output, OPSS, "LD", "A,", "B"); LoadByte(&regAF.hi, regBC.hi, 4, output); break;
        case 0x79: fprintf(output, OPSS, "LD", "A,", "C"); LoadByte(&regAF.hi, regBC.lo, 4, output); break;
        case 0x7A: fprintf(output, OPSS, "LD", "A,", "D"); LoadByte(&regAF.hi, regDE.hi, 4, output); break;
        case 0x7B: fprintf(output, OPSS, "LD", "A,", "E"); LoadByte(&regAF.hi, regDE.lo, 4, output); break;
        case 0x7C: fprintf(output, OPSS, "LD", "A,", "H"); LoadByte(&regAF.hi, regHL.hi, 4, output); break;
        case 0x7D: fprintf(output, OPSS, "LD", "A,", "L"); LoadByte(&regAF.hi, regHL.lo, 4, output); break;

        // Destination = B
        case 0x40: fprintf(output, OPSS, "LD", "B,", "B"); LoadByte(&regBC.hi, regBC.hi, 4, output); break;
        case 0x41: fprintf(output, OPSS, "LD", "B,", "C"); LoadByte(&regBC.hi, regBC.lo, 4, output); break;
        case 0x42: fprintf(output, OPSS, "LD", "B,", "D"); LoadByte(&regBC.hi, regDE.hi, 4, output); break;
        case 0x43: fprintf(output, OPSS, "LD", "B,", "E"); LoadByte(&regBC.hi, regDE.lo, 4, output); break;
        case 0x44: fprintf(output, OPSS, "LD", "B,", "H"); LoadByte(&regBC.hi, regHL.hi, 4, output); break;
        case 0x45: fprintf(output, OPSS, "LD", "B,", "L"); LoadByte(&regBC.hi, regHL.lo, 4, output); break;

        // Destination = C
        case 0x48: fprintf(output, OPSS, "LD", "C,", "B"); LoadByte(&regBC.lo, regBC.hi, 4, output); break;
        case 0x49: fprintf(output, OPSS, "LD", "C,", "C"); LoadByte(&regBC.lo, regBC.lo, 4, output); break;
        case 0x4A: fprintf(output, OPSS, "LD", "C,", "D"); LoadByte(&regBC.lo, regDE.hi, 4, output); break;
        case 0x4B: fprintf(output, OPSS, "LD", "C,", "E"); LoadByte(&regBC.lo, regDE.lo, 4, output); break;
        case 0x4C: fprintf(output, OPSS, "LD", "C,", "H"); LoadByte(&regBC.lo, regHL.hi, 4, output); break;
        case 0x4D: fprintf(output, OPSS, "LD", "C,", "L"); LoadByte(&regBC.lo, regHL.lo, 4, output); break;

        // Destination = D
        case 0x50: fprintf(output, OPSS, "LD", "D,", "B"); LoadByte(&regDE.hi, regBC.hi, 4, output); break;
        case 0x51: fprintf(output, OPSS, "LD", "D,", "C"); LoadByte(&regDE.hi, regBC.lo, 4, output); break;
        case 0x52: fprintf(output, OPSS, "LD", "D,", "D"); LoadByte(&regDE.hi, regDE.hi, 4, output); break;
        case 0x53: fprintf(output, OPSS, "LD", "D,", "E"); LoadByte(&regDE.hi, regDE.lo, 4, output); break;
        case 0x54: fprintf(output, OPSS, "LD", "D,", "H"); LoadByte(&regDE.hi, regHL.hi, 4, output); break;
        case 0x55: fprintf(output, OPSS, "LD", "D,", "L"); LoadByte(&regDE.hi, regHL.lo, 4, output); break;

        // Destination = E
        case 0x58: fprintf(output, OPSS, "LD", "E,", "B"); LoadByte(&regDE.lo, regBC.hi, 4, output); break;
        case 0x59: fprintf(output, OPSS, "LD", "E,", "C"); LoadByte(&regDE.lo, regBC.lo, 4, output); break;
        case 0x5A: fprintf(output, OPSS, "LD", "E,", "D"); LoadByte(&regDE.lo, regDE.hi, 4, output); break;
        case 0x5B: fprintf(output, OPSS, "LD", "E,", "E"); LoadByte(&regDE.lo, regDE.lo, 4, output); break;
        case 0x5C: fprintf(output, OPSS, "LD", "E,", "H"); LoadByte(&regDE.lo, regHL.hi, 4, output); break;
        case 0x5D: fprintf(output, OPSS, "LD", "E,", "L"); LoadByte(&regDE.lo, regHL.lo, 4, output); break;

        // Destination = H
        case 0x60: fprintf(output, OPSS, "LD", "H,", "B"); LoadByte(&regHL.hi, regBC.hi, 4, output); break;
        case 0x61: fprintf(output, OPSS, "LD", "H,", "C"); LoadByte(&regHL.hi, regBC.lo, 4, output); break;
        case 0x62: fprintf(output, OPSS, "LD", "H,", "D"); LoadByte(&regHL.hi, regDE.hi, 4, output); break;
        case 0x63: fprintf(output, OPSS, "LD", "H,", "E"); LoadByte(&regHL.hi, regDE.lo, 4, output); break;
        case 0x64: fprintf(output, OPSS, "LD", "H,", "H"); LoadByte(&regHL.hi, regHL.hi, 4, output); break;
        case 0x65: fprintf(output, OPSS, "LD", "H,", "L"); LoadByte(&regHL.hi, regHL.lo, 4, output); break;

        // Destination = L
        case 0x68: fprintf(output, OPSS, "LD", "L,", "B"); LoadByte(&regHL.lo, regBC.hi, 4, output); break;
        case 0x69: fprintf(output, OPSS, "LD", "L,", "C"); LoadByte(&regHL.lo, regBC.lo, 4, output); break;
        case 0x6A: fprintf(output, OPSS, "LD", "L,", "D"); LoadByte(&regHL.lo, regDE.hi, 4, output); break;
        case 0x6B: fprintf(output, OPSS, "LD", "L,", "E"); LoadByte(&regHL.lo, regDE.lo, 4, output); break;
        case 0x6C: fprintf(output, OPSS, "LD", "L,", "H"); LoadByte(&regHL.lo, regHL.hi, 4, output); break;
        case 0x6D: fprintf(output, OPSS, "LD", "L,", "L"); LoadByte(&regHL.lo, regHL.lo, 4, output); break;

        // END LOAD BYTE REGISTER INTO REGISTER (4 CYCLES) //

        // ==================================================================================== //

        // BEGIN LOAD BYTE IMMEDIATE INTO REGISTER (8 CYCLES) //

        case 0x06: fprintf(output, OPSX, "LD", "B,", memMainRAM[PC.word]); LoadByte(&regBC.hi, memMainRAM[PC.word++], 8, output); break;
        case 0x0E: fprintf(output, OPSX, "LD", "C,", memMainRAM[PC.word]); LoadByte(&regBC.lo, memMainRAM[PC.word++], 8, output); break;
        case 0x16: fprintf(output, OPSX, "LD", "D,", memMainRAM[PC.word]); LoadByte(&regDE.hi, memMainRAM[PC.word++], 8, output); break;
        case 0x1E: fprintf(output, OPSX, "LD", "E,", memMainRAM[PC.word]); LoadByte(&regDE.lo, memMainRAM[PC.word++], 8, output); break;
        case 0x26: fprintf(output, OPSX, "LD", "H,", memMainRAM[PC.word]); LoadByte(&regHL.hi, memMainRAM[PC.word++], 8, output); break;
        case 0x2E: fprintf(output, OPSX, "LD", "L,", memMainRAM[PC.word]); LoadByte(&regHL.lo, memMainRAM[PC.word++], 8, output); break;

        // END LOAD BYTE IMMEDIATE INTO REGISTER (8 CYCLES) //

        // ==================================================================================== //
        
        // BEGIN LOAD BYTE MEMORY INTO REGISTER (8 CYCLES) //

        case 0x7E: fprintf(output, OPSS, "LD", "A,", "(HL)"); LoadByte(&regAF.hi, ReadByte(regHL.word, output), 8, output); break;
        case 0x46: fprintf(output, OPSS, "LD", "B,", "(HL)"); LoadByte(&regBC.hi, ReadByte(regHL.word, output), 8, output); break;
        case 0x4E: fprintf(output, OPSS, "LD", "C,", "(HL)"); LoadByte(&regBC.lo, ReadByte(regHL.word, output), 8, output); break;
        case 0x56: fprintf(output, OPSS, "LD", "D,", "(HL)"); LoadByte(&regDE.hi, ReadByte(regHL.word, output), 8, output); break;
        case 0x5E: fprintf(output, OPSS, "LD", "E,", "(HL)"); LoadByte(&regDE.lo, ReadByte(regHL.word, output), 8, output); break;
        case 0x66: fprintf(output, OPSS, "LD", "H,", "(HL)"); LoadByte(&regHL.hi, ReadByte(regHL.word, output), 8, output); break;
        case 0x6E: fprintf(output, OPSS, "LD", "L,", "(HL)"); LoadByte(&regHL.lo, ReadByte(regHL.word, output), 8, output); break;

        // END LOAD BYTE MEMORY INTO REGISTER (8 CYCLES) //

        // ==================================================================================== //
        
        default: fprintf(output, " ");
    }
}