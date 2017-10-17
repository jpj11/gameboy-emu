#include <stdio.h>
#include "gbCPU.h"

#define OP    "%s"
#define OPL   "%s %d"
#define OPR   "%s %s"
#define OPB   "%s %0#4x"
#define OPW   "%s %0#6x"
#define OPRL  "%s %s, %d"
#define OPRR  "%s %s, %s"
#define OPRB  "%s %s, %0#4x"
#define OPRW  "%s %s, %0#6x"
#define OPBMR "%s (%0#4x), %s"
#define OPRBM "%s %s, (%0#4x)"
#define OPWMR "%s (%0#6x), %s"
#define OPRWM "%s %s, (%0#6x)"

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

        // Load / Store byte in memory into register A
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

        // Increment word
        case 0x03: fprintf(output, OPR, "INC", "BC");
                   break;
        case 0x13: fprintf(output, OPR, "INC", "DE");
                   break;
        case 0x23: fprintf(output, OPR, "INC", "HL");
                   break;
        case 0x33: fprintf(output, OPR, "INC", "SP");
                   break;                                      

        // Increment byte
        case 0x04: fprintf(output, OPR, "INC", "B");
                   break;
        case 0x0c: fprintf(output, OPR, "INC", "C");
                   break;
        case 0x14: fprintf(output, OPR, "INC", "D");
                   break;
        case 0x1c: fprintf(output, OPR, "INC", "E");
                   break;
        case 0x24: fprintf(output, OPR, "INC", "H");
                   break;
        case 0x2c: fprintf(output, OPR, "INC", "L");
                   break;
        case 0x34: fprintf(output, OPR, "INC", "(HL)");
                   break;
        case 0x3c: fprintf(output, OPR, "INC", "A");
                   break;

        // Decrement byte
        case 0x05: fprintf(output, OPR, "DEC", "B");
                   break;
        case 0x0d: fprintf(output, OPR, "DEC", "C");
                   break;
        case 0x15: fprintf(output, OPR, "DEC", "D");
                   break;
        case 0x1d: fprintf(output, OPR, "DEC", "E");
                   break;
        case 0x25: fprintf(output, OPR, "DEC", "H");
                   break;
        case 0x2d: fprintf(output, OPR, "DEC", "L");
                   break;
        case 0x35: fprintf(output, OPR, "DEC", "(HL)");
                   break;
        case 0x3d: fprintf(output, OPR, "DEC", "A");
                   break;                      

        // Load immediate byte into register
        case 0x06: fprintf(output, OPRB, "LD", "B", Fetch(output));
                   break;
        case 0x0e: fprintf(output, OPRB, "LD", "C", Fetch(output));
                   break;           
        case 0x16: fprintf(output, OPRB, "LD", "D", Fetch(output));
                   break;
        case 0x1e: fprintf(output, OPRB, "LD", "E", Fetch(output));
                   break;
        case 0x26: fprintf(output, OPRB, "LD", "H", Fetch(output));
                   break;
        case 0x2e: fprintf(output, OPRB, "LD", "L", Fetch(output));
                   break;
        case 0x36: fprintf(output, OPRB, "LD", "(HL)", Fetch(output));
                   break;
        case 0x3e: fprintf(output, OPRB, "LD", "A", Fetch(output));
                   break;                                                                                               

        // Decrement word
        case 0x0b: fprintf(output, OPR, "DEC", "BC");
                   break;
        case 0x1b: fprintf(output, OPR, "DEC", "DE");
                   break;
        case 0x2b: fprintf(output, OPR, "DEC", "HL");
                   break;
        case 0x3b: fprintf(output, OPR, "DEC", "SP");
                   break;                                                                                                                                                                     

        // Rotate register A left / right through carry flag
        case 0x17: fprintf(output, OP, "RLA");
                   break;
        case 0x1F: fprintf(output, OP, "RRA");
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

        // Load byte from register (or (HL)) into register B
        case 0x40: fprintf(output, OPRR, "LD", "B", "B");
                   break;
        case 0x41: fprintf(output, OPRR, "LD", "B", "C");
                   break;
        case 0x42: fprintf(output, OPRR, "LD", "B", "D");
                   break;
        case 0x43: fprintf(output, OPRR, "LD", "B", "E");
                   break;
        case 0x44: fprintf(output, OPRR, "LD", "B", "H");
                   break;
        case 0x45: fprintf(output, OPRR, "LD", "B", "L");
                   break;
        case 0x46: fprintf(output, OPRR, "LD", "B", "(HL)");
                   break;
        case 0x47: fprintf(output, OPRR, "LD", "B", "A");
                   break;                   

        // Load byte from register (or (HL)) into register C
        case 0x48: fprintf(output, OPRR, "LD", "C", "B");
                   break;
        case 0x49: fprintf(output, OPRR, "LD", "C", "C");
                   break;
        case 0x4a: fprintf(output, OPRR, "LD", "C", "D");
                   break;
        case 0x4b: fprintf(output, OPRR, "LD", "C", "E");
                   break;
        case 0x4c: fprintf(output, OPRR, "LD", "C", "H");
                   break;
        case 0x4d: fprintf(output, OPRR, "LD", "C", "L");
                   break;
        case 0x4e: fprintf(output, OPRR, "LD", "C", "(HL)");
                   break;
        case 0x4f: fprintf(output, OPRR, "LD", "C", "A");
                   break;  

        // Load byte from register (or (HL)) into register D
        case 0x50: fprintf(output, OPRR, "LD", "D", "B");
                   break;
        case 0x51: fprintf(output, OPRR, "LD", "D", "C");
                   break;
        case 0x52: fprintf(output, OPRR, "LD", "D", "D");
                   break;
        case 0x53: fprintf(output, OPRR, "LD", "D", "E");
                   break;
        case 0x54: fprintf(output, OPRR, "LD", "D", "H");
                   break;
        case 0x55: fprintf(output, OPRR, "LD", "D", "L");
                   break;
        case 0x56: fprintf(output, OPRR, "LD", "D", "(HL)");
                   break;
        case 0x57: fprintf(output, OPRR, "LD", "D", "A");
                   break;                   

        // Load byte from register (or (HL)) into register E
        case 0x58: fprintf(output, OPRR, "LD", "E", "B");
                   break;
        case 0x59: fprintf(output, OPRR, "LD", "E", "C");
                   break;
        case 0x5a: fprintf(output, OPRR, "LD", "E", "D");
                   break;
        case 0x5b: fprintf(output, OPRR, "LD", "E", "E");
                   break;
        case 0x5c: fprintf(output, OPRR, "LD", "E", "H");
                   break;
        case 0x5d: fprintf(output, OPRR, "LD", "E", "L");
                   break;
        case 0x5e: fprintf(output, OPRR, "LD", "E", "(HL)");
                   break;
        case 0x5f: fprintf(output, OPRR, "LD", "E", "A");
                   break;  

        // Load byte from register (or (HL)) into register H
        case 0x60: fprintf(output, OPRR, "LD", "H", "B");
                   break;
        case 0x61: fprintf(output, OPRR, "LD", "H", "C");
                   break;
        case 0x62: fprintf(output, OPRR, "LD", "H", "D");
                   break;
        case 0x63: fprintf(output, OPRR, "LD", "H", "E");
                   break;
        case 0x64: fprintf(output, OPRR, "LD", "H", "H");
                   break;
        case 0x65: fprintf(output, OPRR, "LD", "H", "L");
                   break;
        case 0x66: fprintf(output, OPRR, "LD", "H", "(HL)");
                   break;
        case 0x67: fprintf(output, OPRR, "LD", "H", "A");
                   break;                   

        // Load byte from register (or (HL)) into register L
        case 0x68: fprintf(output, OPRR, "LD", "L", "B");
                   break;
        case 0x69: fprintf(output, OPRR, "LD", "L", "C");
                   break;
        case 0x6a: fprintf(output, OPRR, "LD", "L", "D");
                   break;
        case 0x6b: fprintf(output, OPRR, "LD", "L", "E");
                   break;
        case 0x6c: fprintf(output, OPRR, "LD", "L", "H");
                   break;
        case 0x6d: fprintf(output, OPRR, "LD", "L", "L");
                   break;
        case 0x6e: fprintf(output, OPRR, "LD", "L", "(HL)");
                   break;
        case 0x6f: fprintf(output, OPRR, "LD", "L", "A");
                   break;  

        // Load byte from register (or (HL)) into memory at HL
        case 0x70: fprintf(output, OPRR, "LD", "(HL)", "B");
                   break;
        case 0x71: fprintf(output, OPRR, "LD", "(HL)", "C");
                   break;
        case 0x72: fprintf(output, OPRR, "LD", "(HL)", "D");
                   break;
        case 0x73: fprintf(output, OPRR, "LD", "(HL)", "E");
                   break;
        case 0x74: fprintf(output, OPRR, "LD", "(HL)", "H");
                   break;
        case 0x75: fprintf(output, OPRR, "LD", "(HL)", "L");
                   break;
        case 0x77: fprintf(output, OPRR, "LD", "(HL)", "A");
                   break;                   

        // Load byte from register (or (HL)) into register A
        case 0x78: fprintf(output, OPRR, "LD", "A", "B");
                   break;
        case 0x79: fprintf(output, OPRR, "LD", "A", "C");
                   break;
        case 0x7a: fprintf(output, OPRR, "LD", "A", "D");
                   break;
        case 0x7b: fprintf(output, OPRR, "LD", "A", "E");
                   break;
        case 0x7c: fprintf(output, OPRR, "LD", "A", "H");
                   break;
        case 0x7d: fprintf(output, OPRR, "LD", "A", "L");
                   break;
        case 0x7e: fprintf(output, OPRR, "LD", "A", "(HL)");
                   break;
        case 0x7f: fprintf(output, OPRR, "LD", "A", "A");
                   break;  

        // Add byte to register A and store result in register A
        case 0x80: fprintf(output, OPRR, "ADD", "A", "B");
                   break;
        case 0x81: fprintf(output, OPRR, "ADD", "A", "C");
                   break;
        case 0x82: fprintf(output, OPRR, "ADD", "A", "D");
                   break;
        case 0x83: fprintf(output, OPRR, "ADD", "A", "E");
                   break;
        case 0x84: fprintf(output, OPRR, "ADD", "A", "H");
                   break;
        case 0x85: fprintf(output, OPRR, "ADD", "A", "L");
                   break;
        case 0x86: fprintf(output, OPRR, "ADD", "A", "(HL)");
                   break;
        case 0x87: fprintf(output, OPRR, "ADD", "A", "A");
                   break;                                                         

        // Subtract register / memory
        case 0x90: fprintf(output, OPR, "SUB", "B");
                   break;
        case 0x91: fprintf(output, OPR, "SUB", "C");
                   break;
        case 0x92: fprintf(output, OPR, "SUB", "D");
                   break;
        case 0x93: fprintf(output, OPR, "SUB", "E");
                   break;
        case 0x94: fprintf(output, OPR, "SUB", "H");
                   break;
        case 0x95: fprintf(output, OPR, "SUB", "L");
                   break;
        case 0x96: fprintf(output, OPR, "SUB", "(HL)");
                   break;
        case 0x97: fprintf(output, OPR, "SUB", "A");
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

        // Compare byte with register A
        case 0xb8: fprintf(output, OPR, "CP", "B");
                   break;
        case 0xb9: fprintf(output, OPR, "CP", "C");
                   break;
        case 0xba: fprintf(output, OPR, "CP", "D");
                   break;
        case 0xbb: fprintf(output, OPR, "CP", "E");
                   break;
        case 0xbc: fprintf(output, OPR, "CP", "H");
                   break;
        case 0xbd: fprintf(output, OPR, "CP", "L");
                   break;
        case 0xbe: fprintf(output, OPR, "CP", "(HL)");
                   break;
        case 0xbf: fprintf(output, OPR, "CP", "A");
                   break;                                                                                                                  

        // Pop word off of stack
        case 0xc1: fprintf(output, OPR, "POP", "BC");
                   break;
        case 0xd1: fprintf(output, OPR, "POP", "DE");
                   break;
        case 0xe1: fprintf(output, OPR, "POP", "HL");
                   break;
        case 0xf1: fprintf(output, OPR, "POP", "AF");
                   break;                                      

        // Push word onto stack
        case 0xc5: fprintf(output, OPR, "PUSH", "BC");
                   break;
        case 0xd5: fprintf(output, OPR, "PUSH", "DE");
                   break;
        case 0xe5: fprintf(output, OPR, "PUSH", "HL");
                   break;
        case 0xf5: fprintf(output, OPR, "PUSH", "AF");
                   break;                                      

        // Return
        case 0xc9: fprintf(output, OP, "RET");
                   break;
        case 0xd9: fprintf(output, OP, "RETI");
                   break;
        case 0xc8: fprintf(output, OPR, "RET", "Z");
                   break;
        case 0xd8: fprintf(output, OPR, "RET", "C");
                   break;
        case 0xc0: fprintf(output, OPR, "RET", "NZ");
                   break;
        case 0xd0: fprintf(output, OPR, "RET", "NC");
                   break;

        // Opcode cb is a prefix for many instructions. Fetch next opcode and decode to determine
        // the appropriate operation to execute
        case 0xcb: DecodeExecuteCB(Fetch(output), output);
                   break;

        // Call subroutine at immediate word address
        case 0xcd: fprintf(output, OPW, "CALL", GetImmediateWord(output));
                   break;
        case 0xcc: fprintf(output, OPRW, "CALL", "Z", GetImmediateWord(output));
                   break;
        case 0xdc: fprintf(output, OPRW, "CALL", "C", GetImmediateWord(output));
                   break;
        case 0xc4: fprintf(output, OPRW, "CALL", "NZ", GetImmediateWord(output));
                   break;
        case 0xd4: fprintf(output, OPRW, "CALL", "NC", GetImmediateWord(output));
                   break;

        // Load / Store byte at register P1 (address 0xff00) + byte immediate into register A
        case 0xe0: fprintf(output, OPBMR, "LDH", Fetch(output), "A");
                   break;
        case 0xf0: fprintf(output, OPRBM, "LDH", "A", Fetch(output));
                   break;

        // Load / Store byte at register P1 (address 0xff00) + C into register A
        case 0xe2: fprintf(output, OPRR, "LD", "(C)", "A");
                   break;
        case 0xf2: fprintf(output, OPRR, "LD", "A", "(C)");
                   break;

        // Load / Store word immediate into register A
        case 0xea: fprintf(output, OPWMR, "LD", GetImmediateWord(output), "A");
                   break;
        case 0xfa: fprintf(output, OPRWM, "LD", "A", GetImmediateWord(output));
                   break;

        // Compare immediate byte with register A (and set appropriate flags)
        case 0xfe: fprintf(output, OPB, "CP", Fetch(output));
                   break;
    }
}

void DecodeExecuteCB(BYTE opcode, FILE *output)
{
    switch (opcode)
    {
        // Rotate left through carry
        case 0x10: fprintf(output, OPR, "RL", "B");
                   break;
        case 0x11: fprintf(output, OPR, "RL", "C");
                   break;
        case 0x12: fprintf(output, OPR, "RL", "D");
                   break;
        case 0x13: fprintf(output, OPR, "RL", "E");
                   break;
        case 0x14: fprintf(output, OPR, "RL", "H");
                   break;
        case 0x15: fprintf(output, OPR, "RL", "L");
                   break;
        case 0x16: fprintf(output, OPR, "RL", "(HL)");
                   break;
        case 0x17: fprintf(output, OPR, "RL", "A");
                   break;                                                                                                                  

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