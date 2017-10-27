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

short DecodeExecuteCB(BYTE opcode, FILE *output);

short DecodeExecute(BYTE opcode, FILE *output)
{
    short cycles = -1;
    BYTE byteImmediate = 0x00;
    WORD address = 0x0000;

    switch(opcode)
    {
        // Load immediate word into register
        case 0x01: cycles = LoadWord(&regBC.word, GetImmediateWord(output), immediate);
                   fprintf(output, OPRW, "LD", "BC", regBC.word);
                   break;
        case 0x11: cycles = LoadWord(&regDE.word, GetImmediateWord(output), immediate);
                   fprintf(output, OPRW, "LD", "DE", regDE.word);
                   break;
        case 0x21: cycles = LoadWord(&regHL.word, GetImmediateWord(output), immediate);
                   fprintf(output, OPRW, "LD", "HL", regHL.word);
                   break;
        case 0x31: cycles = LoadWord(&SP.word, GetImmediateWord(output), immediate);
                   fprintf(output, OPRW, "LD", "SP", SP.word);
                   break;

        // Load / Store byte in memory into register A
        case 0x02: cycles = LoadByte(&mainMemory[regBC.word], memory, regAF.hi, reg);
                   fprintf(output, OPRR, "LD", "(BC)", "A");
                   break;
        case 0x12: cycles = LoadByte(&mainMemory[regDE.word], memory, regAF.hi, reg);
                   fprintf(output, OPRR, "LD", "(DE)", "A");
                   break;
        case 0x22: cycles = LoadByte(&mainMemory[regHL.word++], memory, regAF.hi, reg);
                   fprintf(output, OPRR, "LD", "(HL+)", "A");
                   break;
        case 0x32: cycles = LoadByte(&mainMemory[regHL.word--], memory, regAF.hi, reg);
                   fprintf(output, OPRR, "LD", "(HL-)", "A");
                   break;
        case 0x0a: cycles = LoadByte(&regAF.hi, reg, mainMemory[regBC.word], memory);
                   fprintf(output, OPRR, "LD", "A", "(BC)");
                   break;
        case 0x1a: cycles = LoadByte(&regAF.hi, reg, mainMemory[regDE.word], memory);
                   fprintf(output, OPRR, "LD", "A", "(DE)");
                   break;
        case 0x2a: cycles = LoadByte(&regAF.hi, reg, mainMemory[regHL.word++], memory);
                   fprintf(output, OPRR, "LD", "A", "(HL+)");
                   break;
        case 0x3a: cycles = LoadByte(&regAF.hi, reg, mainMemory[regHL.word--], memory);
                   fprintf(output, OPRR, "LD", "A", "(HL-)");
                   break;

        // Increment word
        case 0x03: cycles = IncrementWord(&regBC.word);
                   fprintf(output, OPR, "INC", "BC");
                   break;
        case 0x13: cycles = IncrementWord(&regDE.word);
                   fprintf(output, OPR, "INC", "DE");
                   break;
        case 0x23: cycles = IncrementWord(&regHL.word);
                   fprintf(output, OPR, "INC", "HL");
                   break;
        case 0x33: cycles = IncrementWord(&SP.word);
                   fprintf(output, OPR, "INC", "SP");
                   break;                                      

        // Increment byte
        case 0x04: cycles = IncrementByte(&regBC.hi, reg);
                   fprintf(output, OPR, "INC", "B");
                   break;
        case 0x0c: cycles = IncrementByte(&regBC.lo, reg);
                   fprintf(output, OPR, "INC", "C");
                   break;
        case 0x14: cycles = IncrementByte(&regDE.hi, reg);
                   fprintf(output, OPR, "INC", "D");
                   break;
        case 0x1c: cycles = IncrementByte(&regDE.lo, reg);
                   fprintf(output, OPR, "INC", "E");
                   break;
        case 0x24: cycles = IncrementByte(&regHL.hi, reg);
                   fprintf(output, OPR, "INC", "H");
                   break;
        case 0x2c: cycles = IncrementByte(&regHL.lo, reg);
                   fprintf(output, OPR, "INC", "L");
                   break;
        case 0x34: cycles = IncrementByte(&mainMemory[regHL.word], memory);
                   fprintf(output, OPR, "INC", "(HL)");
                   break;
        case 0x3c: cycles = IncrementByte(&regAF.hi, reg);
                   fprintf(output, OPR, "INC", "A");
                   break;

        // Decrement byte
        case 0x05: cycles = DecrementByte(&regBC.hi, reg);
				   fprintf(output, OPR, "DEC", "B");
                   break;
        case 0x0d: cycles = DecrementByte(&regBC.lo, reg);
				   fprintf(output, OPR, "DEC", "C");
                   break;
        case 0x15: cycles = DecrementByte(&regDE.hi, reg);
				   fprintf(output, OPR, "DEC", "D");
                   break;
        case 0x1d: cycles = DecrementByte(&regDE.lo, reg);
				   fprintf(output, OPR, "DEC", "E");
                   break;
        case 0x25: cycles = DecrementByte(&regHL.hi, reg);
				   fprintf(output, OPR, "DEC", "H");
                   break;
        case 0x2d: cycles = DecrementByte(&regHL.lo, reg);
				   fprintf(output, OPR, "DEC", "L");
                   break;
        case 0x35: cycles = DecrementByte(&mainMemory[regHL.word], memory);
                   fprintf(output, OPR, "DEC", "(HL)");
                   break;
        case 0x3d: cycles = DecrementByte(&regAF.hi, reg);
				   fprintf(output, OPR, "DEC", "A");
                   break;                      

        // Load immediate byte into register
        case 0x06: cycles = LoadByte(&regBC.hi, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "B", regBC.hi);
                   break;
        case 0x0e: cycles = LoadByte(&regBC.lo, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "C", regBC.lo);
                   break;           
        case 0x16: cycles = LoadByte(&regDE.hi, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "D", regDE.hi);
                   break;
        case 0x1e: cycles = LoadByte(&regDE.lo, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "E", regDE.lo);
                   break;
        case 0x26: cycles = LoadByte(&regHL.hi, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "H", regHL.hi);
                   break;
        case 0x2e: cycles = LoadByte(&regHL.lo, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "L", regHL.lo);
                   break;
        case 0x36: cycles = LoadByte(&mainMemory[regHL.word], memory, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "(HL)", mainMemory[regHL.word]);
                   break;
        case 0x3e: cycles = LoadByte(&regAF.hi, reg, Fetch(output), immediate);
                   fprintf(output, OPRB, "LD", "A", regAF.hi);
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
        case 0x17: cycles = RotateLeftAccu();
                   fprintf(output, OP, "RLA");
                   break;
        case 0x1F: fprintf(output, OP, "RRA");
                   break;

        // Jump relative to current PC
        case 0x18: fprintf(output, OPL, "JR", (S_BYTE)Fetch(output));
                   break;
        case 0x28: byteImmediate = Fetch(output);
                   cycles = JumpRelativeCond(zero, true, (S_BYTE)byteImmediate);
                   fprintf(output, OPRL, "JR", "Z", (S_BYTE)byteImmediate);
                   break;
        case 0x38: byteImmediate = Fetch(output);
                   cycles = JumpRelativeCond(carry, true, (S_BYTE)byteImmediate);
                   fprintf(output, OPRL, "JR", "C", (S_BYTE)byteImmediate);
                   break;                   
        case 0x20: byteImmediate = Fetch(output);
                   cycles = JumpRelativeCond(zero, false, (S_BYTE)byteImmediate);
                   fprintf(output, OPRL, "JR", "NZ", (S_BYTE)byteImmediate);
                   break;
        case 0x30: byteImmediate = Fetch(output);
                   cycles = JumpRelativeCond(carry, false, (S_BYTE)byteImmediate);
                   fprintf(output, OPRL, "JR", "NC", (S_BYTE)byteImmediate);
                   break;                                      

        // Load byte from register (or (HL)) into register B
        case 0x40: cycles = LoadByte(&regBC.hi, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "B", "B");
                   break;
        case 0x41: cycles = LoadByte(&regBC.hi, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "B", "C");
                   break;
        case 0x42: cycles = LoadByte(&regBC.hi, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "B", "D");
                   break;
        case 0x43: cycles = LoadByte(&regBC.hi, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "B", "E");
                   break;
        case 0x44: cycles = LoadByte(&regBC.hi, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "B", "H");
                   break;
        case 0x45: cycles = LoadByte(&regBC.hi, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "B", "L");
                   break;
        case 0x46: cycles = LoadByte(&regBC.hi, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "B", "(HL)");
                   break;
        case 0x47: cycles = LoadByte(&regBC.hi, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "B", "A");
                   break;                   

        // Load byte from register (or (HL)) into register C
        case 0x48: cycles = LoadByte(&regBC.lo, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "C", "B");
                   break;
        case 0x49: cycles = LoadByte(&regBC.lo, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "C", "C");
                   break;
        case 0x4a: cycles = LoadByte(&regBC.lo, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "C", "D");
                   break;
        case 0x4b: cycles = LoadByte(&regBC.lo, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "C", "E");
                   break;
        case 0x4c: cycles = LoadByte(&regBC.lo, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "C", "H");
                   break;
        case 0x4d: cycles = LoadByte(&regBC.lo, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "C", "L");
                   break;
        case 0x4e: cycles = LoadByte(&regBC.lo, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "C", "(HL)");
                   break;
        case 0x4f: cycles = LoadByte(&regBC.lo, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "C", "A");
                   break;  

        // Load byte from register (or (HL)) into register D
        case 0x50: cycles = LoadByte(&regDE.hi, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "D", "B");
                   break;
        case 0x51: cycles = LoadByte(&regDE.hi, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "D", "C");
                   break;
        case 0x52: cycles = LoadByte(&regDE.hi, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "D", "D");
                   break;
        case 0x53: cycles = LoadByte(&regDE.hi, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "D", "E");
                   break;
        case 0x54: cycles = LoadByte(&regDE.hi, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "D", "H");
                   break;
        case 0x55: cycles = LoadByte(&regDE.hi, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "D", "L");
                   break;
        case 0x56: cycles = LoadByte(&regDE.hi, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "D", "(HL)");
                   break;
        case 0x57: cycles = LoadByte(&regDE.hi, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "D", "A");
                   break;                   

        // Load byte from register (or (HL)) into register E
        case 0x58: cycles = LoadByte(&regDE.lo, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "E", "B");
                   break;
        case 0x59: cycles = LoadByte(&regDE.lo, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "E", "C");
                   break;
        case 0x5a: cycles = LoadByte(&regDE.lo, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "E", "D");
                   break;
        case 0x5b: cycles = LoadByte(&regDE.lo, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "E", "E");
                   break;
        case 0x5c: cycles = LoadByte(&regDE.lo, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "E", "H");
                   break;
        case 0x5d: cycles = LoadByte(&regDE.lo, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "E", "L");
                   break;
        case 0x5e: cycles = LoadByte(&regDE.lo, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "E", "(HL)");
                   break;
        case 0x5f: cycles = LoadByte(&regDE.lo, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "E", "A");
                   break;  

        // Load byte from register (or (HL)) into register H
        case 0x60: cycles = LoadByte(&regHL.hi, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "H", "B");
                   break;
        case 0x61: cycles = LoadByte(&regHL.hi, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "H", "C");
                   break;
        case 0x62: cycles = LoadByte(&regHL.hi, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "H", "D");
                   break;
        case 0x63: cycles = LoadByte(&regHL.hi, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "H", "E");
                   break;
        case 0x64: cycles = LoadByte(&regHL.hi, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "H", "H");
                   break;
        case 0x65: cycles = LoadByte(&regHL.hi, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "H", "L");
                   break;
        case 0x66: cycles = LoadByte(&regHL.hi, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "H", "(HL)");
                   break;
        case 0x67: cycles = LoadByte(&regHL.hi, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "H", "A");
                   break;                   

        // Load byte from register (or (HL)) into register L
        case 0x68: cycles = LoadByte(&regHL.lo, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "L", "B");
                   break;
        case 0x69: cycles = LoadByte(&regHL.lo, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "L", "C");
                   break;
        case 0x6a: cycles = LoadByte(&regHL.lo, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "L", "D");
                   break;
        case 0x6b: cycles = LoadByte(&regHL.lo, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "L", "E");
                   break;
        case 0x6c: cycles = LoadByte(&regHL.lo, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "L", "H");
                   break;
        case 0x6d: cycles = LoadByte(&regHL.lo, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "L", "L");
                   break;
        case 0x6e: cycles = LoadByte(&regHL.lo, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "L", "(HL)");
                   break;
        case 0x6f: cycles = LoadByte(&regHL.lo, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "L", "A");
                   break;  

        // Load byte from register (or (HL)) into memory at HL
        case 0x70: cycles = LoadByte(&mainMemory[regHL.word], memory, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "B");
                   break;
        case 0x71: cycles = LoadByte(&mainMemory[regHL.word], memory, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "C");
                   break;
        case 0x72: cycles = LoadByte(&mainMemory[regHL.word], memory, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "D");
                   break;
        case 0x73: cycles = LoadByte(&mainMemory[regHL.word], memory, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "E");
                   break;
        case 0x74: cycles = LoadByte(&mainMemory[regHL.word], memory, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "H");
                   break;
        case 0x75: cycles = LoadByte(&mainMemory[regHL.word], memory, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "L");
                   break;
        case 0x77: cycles = LoadByte(&mainMemory[regHL.word], memory, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "(HL)", "A");
                   break;                   

        // Load byte from register (or (HL)) into register A
        case 0x78: cycles = LoadByte(&regAF.hi, reg, regBC.hi, reg);
				   fprintf(output, OPRR, "LD", "A", "B");
                   break;
        case 0x79: cycles = LoadByte(&regAF.hi, reg, regBC.lo, reg);
				   fprintf(output, OPRR, "LD", "A", "C");
                   break;
        case 0x7a: cycles = LoadByte(&regAF.hi, reg, regDE.hi, reg);
				   fprintf(output, OPRR, "LD", "A", "D");
                   break;
        case 0x7b: cycles = LoadByte(&regAF.hi, reg, regDE.lo, reg);
				   fprintf(output, OPRR, "LD", "A", "E");
                   break;
        case 0x7c: cycles = LoadByte(&regAF.hi, reg, regHL.hi, reg);
				   fprintf(output, OPRR, "LD", "A", "H");
                   break;
        case 0x7d: cycles = LoadByte(&regAF.hi, reg, regHL.lo, reg);
				   fprintf(output, OPRR, "LD", "A", "L");
                   break;
        case 0x7e: cycles = LoadByte(&regAF.hi, reg, mainMemory[regHL.word], memory);
                   fprintf(output, OPRR, "LD", "A", "(HL)");
                   break;
        case 0x7f: cycles = LoadByte(&regAF.hi, reg, regAF.hi, reg);
				   fprintf(output, OPRR, "LD", "A", "A");
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
        case 0x90: cycles = Subtract(regBC.hi, reg);
				   fprintf(output, OPR, "SUB", "B");
                   break;
        case 0x91: cycles = Subtract(regBC.lo, reg);
				   fprintf(output, OPR, "SUB", "C");
                   break;
        case 0x92: cycles = Subtract(regDE.hi, reg);
				   fprintf(output, OPR, "SUB", "D");
                   break;
        case 0x93: cycles = Subtract(regDE.lo, reg);
				   fprintf(output, OPR, "SUB", "E");
                   break;
        case 0x94: cycles = Subtract(regHL.hi, reg);
				   fprintf(output, OPR, "SUB", "H");
                   break;
        case 0x95: cycles = Subtract(regHL.lo, reg);
				   fprintf(output, OPR, "SUB", "L");
                   break;
        case 0x96: cycles = Subtract(mainMemory[regHL.word], memory);
                   fprintf(output, OPR, "SUB", "(HL)");
                   break;
        case 0x97: cycles = Subtract(regAF.hi, reg);
				   fprintf(output, OPR, "SUB", "A");
                   break;                                                                                                                  

        // Xor register / memory
        case 0xa8: cycles = Xor(regBC.hi, reg);
                   fprintf(output, OPR, "XOR", "B");
                   break;
        case 0xa9: cycles = Xor(regBC.lo, reg);
                   fprintf(output, OPR, "XOR", "C");
                   break;
        case 0xaa: cycles = Xor(regDE.hi, reg);
                   fprintf(output, OPR, "XOR", "D");
                   break;
        case 0xab: cycles = Xor(regDE.lo, reg);
                   fprintf(output, OPR, "XOR", "E");
                   break;
        case 0xac: cycles = Xor(regHL.hi, reg);
                   fprintf(output, OPR, "XOR", "H");
                   break;
        case 0xad: cycles = Xor(regHL.lo, reg);
                   fprintf(output, OPR, "XOR", "L");
                   break;
        case 0xae: cycles = Xor(mainMemory[regHL.word], memory);
                   fprintf(output, OPR, "XOR", "(HL)");
                   break;
        case 0xaf: cycles = Xor(regAF.hi, reg);
                   fprintf(output, OPR, "XOR", "A");
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
        case 0xc1: cycles = Pop(&regBC.word);
                   fprintf(output, OPR, "POP", "BC");
                   break;
        case 0xd1: cycles = Pop(&regDE.word);
                   fprintf(output, OPR, "POP", "DE");
                   break;
        case 0xe1: cycles = Pop(&regHL.word);
                   fprintf(output, OPR, "POP", "HL");
                   break;
        case 0xf1: cycles = Pop(&regAF.word);
                   fprintf(output, OPR, "POP", "AF");
                   break;                                      

        // Push word onto stack
        case 0xc5: cycles = Push(regBC.word);
                   fprintf(output, OPR, "PUSH", "BC");
                   break;
        case 0xd5: cycles = Push(regDE.word);
                   fprintf(output, OPR, "PUSH", "DE");
                   break;
        case 0xe5: cycles = Push(regHL.word);
                   fprintf(output, OPR, "PUSH", "HL");
                   break;
        case 0xf5: cycles = Push(regAF.word);
                   fprintf(output, OPR, "PUSH", "AF");
                   break;                                      

        // Return
        case 0xc9: cycles = Return();
                   fprintf(output, OP, "RET");
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
        case 0xcb: cycles = DecodeExecuteCB(Fetch(output), output);
                   break;

        // Call subroutine at immediate word address
        case 0xcd: address = GetImmediateWord(output);
                   cycles = Call(address);
                   fprintf(output, OPW, "CALL", address);
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
        case 0xe0: byteImmediate = Fetch(output);
                   cycles = LoadByte(&mainMemory[byteImmediate + REG_P1], immediateOffset, regAF.hi, reg);
                   fprintf(output, OPBMR, "LDH", byteImmediate, "A");
                   break;
        case 0xf0: byteImmediate = Fetch(output);
                   cycles = LoadByte(&regAF.hi, reg, mainMemory[byteImmediate + REG_P1], immediateOffset);
                   fprintf(output, OPRBM, "LDH", "A", Fetch(output));
                   break;

        // Load / Store byte at register P1 (address 0xff00) + C into register A
        case 0xe2: cycles = LoadByte(&mainMemory[regBC.lo + REG_P1], regOffset, regAF.hi, reg);
                   fprintf(output, OPRR, "LD", "(C)", "A");
                   break;
        case 0xf2: cycles = LoadByte(&regAF.hi, reg, mainMemory[regBC.lo + REG_P1], regOffset);
                   fprintf(output, OPRR, "LD", "A", "(C)");
                   break;

        // Load / Store word immediate into register A
        case 0xea: address = GetImmediateWord(output);
                   cycles = LoadByte(&mainMemory[address], memAtImmediate, regAF.hi, reg);
                   fprintf(output, OPWMR, "LD", address, "A");
                   break;
        case 0xfa: address = GetImmediateWord(output);
                   cycles = LoadByte(&regAF.hi, reg, mainMemory[address], memAtImmediate);
                   fprintf(output, OPRWM, "LD", "A", GetImmediateWord(output));
                   break;

        // Compare immediate byte with register A (and set appropriate flags)
        case 0xfe: byteImmediate = Fetch(output);
                   cycles = Compare(byteImmediate, immediate);
                   fprintf(output, OPB, "CP", byteImmediate);
                   break;
    }
    return cycles;
}

short DecodeExecuteCB(BYTE opcode, FILE *output)
{
    short cycles = -1;

    switch (opcode)
    {
        // Rotate left through carry
        case 0x10: cycles = RotateLeft(&regBC.hi, reg);
                   fprintf(output, OPR, "RL", "B");
                   break;
        case 0x11: cycles = RotateLeft(&regBC.lo, reg);
                   fprintf(output, OPR, "RL", "C");
                   break;
        case 0x12: cycles = RotateLeft(&regDE.hi, reg);
                   fprintf(output, OPR, "RL", "D");
                   break;
        case 0x13: cycles = RotateLeft(&regDE.lo, reg);
                   fprintf(output, OPR, "RL", "E");
                   break;
        case 0x14: cycles = RotateLeft(&regHL.hi, reg);
                   fprintf(output, OPR, "RL", "H");
                   break;
        case 0x15: cycles = RotateLeft(&regHL.lo, reg);
                   fprintf(output, OPR, "RL", "L");
                   break;
        case 0x16: cycles = RotateLeft(&mainMemory[regHL.word], memory);
                   fprintf(output, OPR, "RL", "(HL)");
                   break;
        case 0x17: cycles = RotateLeft(&regAF.hi, reg);
                   fprintf(output, OPR, "RL", "A");
                   break;                                                                                                                  

        // Test bit 0
        case 0x40: cycles = Bit(0, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "0", "B");
                   break;
        case 0x41: cycles = Bit(0, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "0", "C");
                   break;
        case 0x42: cycles = Bit(0, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "0", "D");
                   break;
        case 0x43: cycles = Bit(0, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "0", "E");
                   break;
        case 0x44: cycles = Bit(0, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "0", "H");
                   break;
        case 0x45: cycles = Bit(0, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "0", "L");
                   break;
        case 0x46: cycles = Bit(0, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "0", "(HL)");
                   break;
        case 0x47: cycles = Bit(0, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "0", "A");
                   break;

        // Test bit 1
        case 0x48: cycles = Bit(1, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "1", "B");
                   break;
        case 0x49: cycles = Bit(1, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "1", "C");
                   break;
        case 0x4a: cycles = Bit(1, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "1", "D");
                   break;
        case 0x4b: cycles = Bit(1, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "1", "E");
                   break;
        case 0x4c: cycles = Bit(1, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "1", "H");
                   break;
        case 0x4d: cycles = Bit(1, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "1", "L");
                   break;
        case 0x4e: cycles = Bit(1, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "1", "(HL)");
                   break;
        case 0x4f: cycles = Bit(1, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "1", "A");
                   break;

        // Test bit 2
        case 0x50: cycles = Bit(2, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "2", "B");
                   break;
        case 0x51: cycles = Bit(2, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "2", "C");
                   break;
        case 0x52: cycles = Bit(2, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "2", "D");
                   break;
        case 0x53: cycles = Bit(2, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "2", "E");
                   break;
        case 0x54: cycles = Bit(2, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "2", "H");
                   break;
        case 0x55: cycles = Bit(2, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "2", "L");
                   break;
        case 0x56: cycles = Bit(2, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "2", "(HL)");
                   break;
        case 0x57: cycles = Bit(2, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "2", "A");
                   break;

        // Test bit 3
        case 0x58: cycles = Bit(3, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "3", "B");
                   break;
        case 0x59: cycles = Bit(3, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "3", "C");
                   break;
        case 0x5a: cycles = Bit(3, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "3", "D");
                   break;
        case 0x5b: cycles = Bit(3, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "3", "E");
                   break;
        case 0x5c: cycles = Bit(3, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "3", "H");
                   break;
        case 0x5d: cycles = Bit(3, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "3", "L");
                   break;
        case 0x5e: cycles = Bit(3, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "3", "(HL)");
                   break;
        case 0x5f: cycles = Bit(3, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "3", "A");
                   break;

        // Test bit 4
        case 0x60: cycles = Bit(4, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "4", "B");
                   break;
        case 0x61: cycles = Bit(4, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "4", "C");
                   break;
        case 0x62: cycles = Bit(4, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "4", "D");
                   break;
        case 0x63: cycles = Bit(4, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "4", "E");
                   break;
        case 0x64: cycles = Bit(4, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "4", "H");
                   break;
        case 0x65: cycles = Bit(4, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "4", "L");
                   break;
        case 0x66: cycles = Bit(4, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "4", "(HL)");
                   break;
        case 0x67: cycles = Bit(4, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "4", "A");
                   break;                    

        // Test bit 5
        case 0x68: cycles = Bit(5, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "5", "B");
                   break;
        case 0x69: cycles = Bit(5, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "5", "C");
                   break;
        case 0x6a: cycles = Bit(5, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "5", "D");
                   break;
        case 0x6b: cycles = Bit(5, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "5", "E");
                   break;
        case 0x6c: cycles = Bit(5, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "5", "H");
                   break;
        case 0x6d: cycles = Bit(5, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "5", "L");
                   break;
        case 0x6e: cycles = Bit(5, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "5", "(HL)");
                   break;
        case 0x6f: cycles = Bit(5, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "5", "A");
                   break;        
                   
        // Test bit 6
        case 0x70: cycles = Bit(6, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "6", "B");
                   break;
        case 0x71: cycles = Bit(6, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "6", "C");
                   break;
        case 0x72: cycles = Bit(6, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "6", "D");
                   break;
        case 0x73: cycles = Bit(6, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "6", "E");
                   break;
        case 0x74: cycles = Bit(6, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "6", "H");
                   break;
        case 0x75: cycles = Bit(6, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "6", "L");
                   break;
        case 0x76: cycles = Bit(6, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "6", "(HL)");
                   break;
        case 0x77: cycles = Bit(6, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "6", "A");
                   break;                    

        // Test bit 7
        case 0x78: cycles = Bit(7, &regBC.hi, reg);
				   fprintf(output, OPRR, "BIT", "7", "B");
                   break;
        case 0x79: cycles = Bit(7, &regBC.lo, reg);
				   fprintf(output, OPRR, "BIT", "7", "C");
                   break;
        case 0x7a: cycles = Bit(7, &regDE.hi, reg);
				   fprintf(output, OPRR, "BIT", "7", "D");
                   break;
        case 0x7b: cycles = Bit(7, &regDE.lo, reg);
				   fprintf(output, OPRR, "BIT", "7", "E");
                   break;
        case 0x7c: cycles = Bit(7, &regHL.hi, reg);
				   fprintf(output, OPRR, "BIT", "7", "H");
                   break;
        case 0x7d: cycles = Bit(7, &regHL.lo, reg);
				   fprintf(output, OPRR, "BIT", "7", "L");
                   break;
        case 0x7e: cycles = Bit(7, &mainMemory[regHL.word], memory);
				   fprintf(output, OPRR, "BIT", "7", "(HL)");
                   break;
        case 0x7f: cycles = Bit(7, &regAF.hi, reg);
				   fprintf(output, OPRR, "BIT", "7", "A");
                   break;                       
    }
    return cycles;
}