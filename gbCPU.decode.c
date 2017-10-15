#include <stdio.h>
#include "gbCPU.h"

#define OPL  "%s %d"
#define OPR  "%s %s"
#define OPRL "%s %s, %d"
#define OPRR "%s %s, %s"
#define OPRW "%s %s, %0#6x"

void DecodeExecuteCB(BYTE opcode, FILE *output);

void DecodeExecute(BYTE opcode, FILE *output)
{
    switch(opcode)
    {
        // Load immediate word into register
        case 0x01: fprintf(output, OPRW, "LD", "BC", GetImmediateWord(output));
                   break;
        case 0x11: fprintf(output, OPRW, "LD", "DE", GetImmediateWord(output));
                   break;
        case 0x21: fprintf(output, OPRW, "LD", "HL", GetImmediateWord(output));
                   break;
        case 0x31: fprintf(output, OPRW, "LD", "SP", GetImmediateWord(output));
                   break;

        // Load / Store byte into register A
        case 0x02: fprintf(output, OPRR, "LD", "(BC)", "A");
                   break;
        case 0x12: fprintf(output, OPRR, "LD", "(DE)", "A");
                   break;
        case 0x22: fprintf(output, OPRR, "LD", "(HL+)", "A");
                   break;
        case 0x32: fprintf(output, OPRR, "LD", "(HL-)", "A");
                   break;
        case 0x0a: fprintf(output, OPRR, "LD", "A", "(BC)");
                   break;
        case 0x1a: fprintf(output, OPRR, "LD", "A", "(DE)");
                   break;
        case 0x2a: fprintf(output, OPRR, "LD", "A", "(HL+)");
                   break;
        case 0x3a: fprintf(output, OPRR, "LD", "A", "(HL-)");
                   break;

        // Jump relative to current PC
        case 0x18: fprintf(output, OPL, "JR", (S_BYTE)Fetch(output));
                   break;
        case 0x28: fprintf(output, OPRL, "JR", "Z", (S_BYTE)Fetch(output));
                   break;
        case 0x38: fprintf(output, OPRL, "JR", "C", (S_BYTE)Fetch(output));
                   break;                   
        case 0x20: fprintf(output, OPRL, "JR", "NZ", (S_BYTE)Fetch(output));
                   break;
        case 0x30: fprintf(output, OPRL, "JR", "NC", (S_BYTE)Fetch(output));
                   break;                                      

        // Xor register / memory
        case 0xa8: fprintf(output, OPR, "XOR", "B");
                   break;
        case 0xa9: fprintf(output, OPR, "XOR", "C");
                   break;
        case 0xaa: fprintf(output, OPR, "XOR", "D");
                   break;
        case 0xab: fprintf(output, OPR, "XOR", "E");
                   break;
        case 0xac: fprintf(output, OPR, "XOR", "H");
                   break;
        case 0xad: fprintf(output, OPR, "XOR", "L");
                   break;
        case 0xae: fprintf(output, OPR, "XOR", "(HL)");
                   break;
        case 0xaf: fprintf(output, OPR, "XOR", "A");
                   break;

        // Opcode cb is a prefix for many instructions. Fetch next opcode and decode to determine
        // the appropriate operation to execute
        case 0xcb: DecodeExecuteCB(Fetch(output), output);
                   break;
    }
}

void DecodeExecuteCB(BYTE opcode, FILE *output)
{
    switch (opcode)
    {
        // Test bit 0
        case 0x40: fprintf(output, OPRR, "BIT", "0", "B");
                   break;
        case 0x41: fprintf(output, OPRR, "BIT", "0", "C");
                   break;
        case 0x42: fprintf(output, OPRR, "BIT", "0", "D");
                   break;
        case 0x43: fprintf(output, OPRR, "BIT", "0", "E");
                   break;
        case 0x44: fprintf(output, OPRR, "BIT", "0", "H");
                   break;
        case 0x45: fprintf(output, OPRR, "BIT", "0", "L");
                   break;
        case 0x46: fprintf(output, OPRR, "BIT", "0", "(HL)");
                   break;
        case 0x47: fprintf(output, OPRR, "BIT", "0", "A");
                   break;

        // Test bit 1
        case 0x48: fprintf(output, OPRR, "BIT", "1", "B");
                   break;
        case 0x49: fprintf(output, OPRR, "BIT", "1", "C");
                   break;
        case 0x4a: fprintf(output, OPRR, "BIT", "1", "D");
                   break;
        case 0x4b: fprintf(output, OPRR, "BIT", "1", "E");
                   break;
        case 0x4c: fprintf(output, OPRR, "BIT", "1", "H");
                   break;
        case 0x4d: fprintf(output, OPRR, "BIT", "1", "L");
                   break;
        case 0x4e: fprintf(output, OPRR, "BIT", "1", "(HL)");
                   break;
        case 0x4f: fprintf(output, OPRR, "BIT", "1", "A");
                   break;

        // Test bit 2
        case 0x50: fprintf(output, OPRR, "BIT", "2", "B");
                   break;
        case 0x51: fprintf(output, OPRR, "BIT", "2", "C");
                   break;
        case 0x52: fprintf(output, OPRR, "BIT", "2", "D");
                   break;
        case 0x53: fprintf(output, OPRR, "BIT", "2", "E");
                   break;
        case 0x54: fprintf(output, OPRR, "BIT", "2", "H");
                   break;
        case 0x55: fprintf(output, OPRR, "BIT", "2", "L");
                   break;
        case 0x56: fprintf(output, OPRR, "BIT", "2", "(HL)");
                   break;
        case 0x57: fprintf(output, OPRR, "BIT", "2", "A");
                   break;

        // Test bit 3
        case 0x58: fprintf(output, OPRR, "BIT", "3", "B");
                   break;
        case 0x59: fprintf(output, OPRR, "BIT", "3", "C");
                   break;
        case 0x5a: fprintf(output, OPRR, "BIT", "3", "D");
                   break;
        case 0x5b: fprintf(output, OPRR, "BIT", "3", "E");
                   break;
        case 0x5c: fprintf(output, OPRR, "BIT", "3", "H");
                   break;
        case 0x5d: fprintf(output, OPRR, "BIT", "3", "L");
                   break;
        case 0x5e: fprintf(output, OPRR, "BIT", "3", "(HL)");
                   break;
        case 0x5f: fprintf(output, OPRR, "BIT", "3", "A");
                   break;

        // Test bit 4
        case 0x60: fprintf(output, OPRR, "BIT", "4", "B");
                   break;
        case 0x61: fprintf(output, OPRR, "BIT", "4", "C");
                   break;
        case 0x62: fprintf(output, OPRR, "BIT", "4", "D");
                   break;
        case 0x63: fprintf(output, OPRR, "BIT", "4", "E");
                   break;
        case 0x64: fprintf(output, OPRR, "BIT", "4", "H");
                   break;
        case 0x65: fprintf(output, OPRR, "BIT", "4", "L");
                   break;
        case 0x66: fprintf(output, OPRR, "BIT", "4", "(HL)");
                   break;
        case 0x67: fprintf(output, OPRR, "BIT", "4", "A");
                   break;                    

        // Test bit 5
        case 0x68: fprintf(output, OPRR, "BIT", "5", "B");
                   break;
        case 0x69: fprintf(output, OPRR, "BIT", "5", "C");
                   break;
        case 0x6a: fprintf(output, OPRR, "BIT", "5", "D");
                   break;
        case 0x6b: fprintf(output, OPRR, "BIT", "5", "E");
                   break;
        case 0x6c: fprintf(output, OPRR, "BIT", "5", "H");
                   break;
        case 0x6d: fprintf(output, OPRR, "BIT", "5", "L");
                   break;
        case 0x6e: fprintf(output, OPRR, "BIT", "5", "(HL)");
                   break;
        case 0x6f: fprintf(output, OPRR, "BIT", "5", "A");
                   break;        
                   
        // Test bit 6
        case 0x70: fprintf(output, OPRR, "BIT", "6", "B");
                   break;
        case 0x71: fprintf(output, OPRR, "BIT", "6", "C");
                   break;
        case 0x72: fprintf(output, OPRR, "BIT", "6", "D");
                   break;
        case 0x73: fprintf(output, OPRR, "BIT", "6", "E");
                   break;
        case 0x74: fprintf(output, OPRR, "BIT", "6", "H");
                   break;
        case 0x75: fprintf(output, OPRR, "BIT", "6", "L");
                   break;
        case 0x76: fprintf(output, OPRR, "BIT", "6", "(HL)");
                   break;
        case 0x77: fprintf(output, OPRR, "BIT", "6", "A");
                   break;                    

        // Test bit 7
        case 0x78: fprintf(output, OPRR, "BIT", "7", "B");
                   break;
        case 0x79: fprintf(output, OPRR, "BIT", "7", "C");
                   break;
        case 0x7a: fprintf(output, OPRR, "BIT", "7", "D");
                   break;
        case 0x7b: fprintf(output, OPRR, "BIT", "7", "E");
                   break;
        case 0x7c: fprintf(output, OPRR, "BIT", "7", "H");
                   break;
        case 0x7d: fprintf(output, OPRR, "BIT", "7", "L");
                   break;
        case 0x7e: fprintf(output, OPRR, "BIT", "7", "(HL)");
                   break;
        case 0x7f: fprintf(output, OPRR, "BIT", "7", "A");
                   break;                       
    }
}