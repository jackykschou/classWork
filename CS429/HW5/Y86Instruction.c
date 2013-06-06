#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Y86Instruction.h"
#include "memory.h"
#include "programStateStruct.h"
#ifndef NULLC
#define NULLC '\0'
#endif
//source file for different operations in Y86ISA

extern CC cc;
extern unsigned int pc;
extern unsigned char **memory;
extern Reg *regArray;

void invalidReg()
{
    printf(" -- invalid opcode(invalid register id)\n");
    exit(1);
}

void invalidOpcode(unsigned char opCode)
{
    printf("0x%02X -- invalid opcode\n", opCode);
    exit(1);
}

void halt()
{
    printf("\n");
}

void nop()
{
    printf("\n");
}

void rrmovl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    rB->value = rA->value;
    printf(" %s,%s%20s%s ->0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", rA->name, rA->value, rA->value, rB->name);
}

void cmovle()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    if((cc.SF^cc.OF)|cc.ZF) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%20sZF->%d, SF->%d, OF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.ZF, cc.SF, cc.OF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%20sZF->%d, SF->%d, OF->%d, no jump)\n", rA->name, rB->name, "(", cc.ZF, cc.SF, cc.OF);
}

void cmovl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    if(cc.SF^cc.OF) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%20sSF->%d, OF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.SF, cc.OF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%21sSF->%d, OF->%d, no jump)\n", rA->name, rB->name, "(", cc.SF, cc.OF);
}

void cmove()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    if(cc.ZF) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%21sZF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.ZF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%21sZF->%d, no jump)\n", rA->name, rB->name, "(", cc.ZF);
}

void cmovne()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    if(cc.ZF == 0) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%20sZF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.ZF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%20sZF->%d, no jump)\n", rA->name, rB->name, "(", cc.ZF);
}

void cmovge()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    if((cc.SF^cc.OF) == 0) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%20sSF->%d, OF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.SF, cc.OF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%20sSF->%d, OF->%d, no jump)\n", rA->name, rB->name, "(", cc.SF, cc.OF);
}

void cmovg()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    //printf("\n        SF: %d,   OF: %d,     ZF: %d\n", cc.SF, cc.OF, cc.ZF);
    if((!(cc.SF^cc.OF))&(!cc.ZF)) //check condition
    {
        rB->value = rA->value;
        printf(" %s,%s%21sZF->%d, SF->%d, OF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", rA->name, rB->name, "(", cc.ZF, cc.SF, cc.OF, rA->name, rA->value, rB->value, rB->name);
    }
    else
        printf(" %s,%s%21sZF->%d, SF->%d, OF->%d, no jump)\n", rA->name, rB->name, "(", cc.ZF, cc.SF, cc.OF);
}

void irmovl()
{
    //for getting rB and V
    char byte;
    Reg *rB = NULLC;
    int V;

    getMemoryChar(pc++, &byte);
    getMemoryInt(pc, &V);
    pc += 4;
    rB = &regArray[0XF & byte];
    if(((0XF & (byte >> 4)) != 0XF) || (!(rB)))//check register validity
        invalidReg();
    rB->value = V;
    printf(" 0x%08X,%s%14s0x%08X -> %s)\n", V, rB->name, "(", V, rB->name);
}

void rmmovl()
{
    //for getting rA, rB and D
    char byte;
    Reg *rA = NULLC, *rB = NULLC;
    int D;
    getMemoryChar(pc++, &byte);
    getMemoryInt(pc, &D);
    pc += 4;
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    setMemoryInt(rB->value + D, rA->value);
    printf(" %s,0x%08X(%s)%8s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> [0x%08X])\n", rA->name, D, rB->name, "(", rB->name, rB->value, rA->name, rA->value, rA->value, rB->value + D);
}

void mrmovl()
{
    //for getting rA, rB and D
    char byte;
    Reg *rA = NULLC, *rB = NULLC;
    int D;

    getMemoryChar(pc++, &byte);
    getMemoryInt(pc, &D);
    pc += 4;
    rA = &regArray[0XF & (byte >> 4)], rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();
    getMemoryInt(rB->value + D, &(rA->value));
    printf(" 0x%08X(%s),%s%8s%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s)\n", D, rB->name, rA->name, "(", rA->name, rB->value, rB->value + D, rA->value, rA->value, rA->name);
}

void call()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    pc += 4;

    //save address on the stack
    regArray[4].value -= 4;
    setMemoryInt((regArray[4].value), pc);

    printf(" 0x%08X%21s%s -> 0x%08X, 0x%08X -> [0x%08X], 0x%08X -> %s, 0x%08X -> PC)\n", Dest, "(", regArray[4].name, regArray[4].value + 4, pc, regArray[4].value, regArray[4].value, regArray[4].name, Dest);
    pc = Dest;
}

void ret()
{
    getMemoryInt(regArray[4].value, &pc);
    printf("%33s%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s, 0x%08X -> PC)\n", "(", regArray[4].name, regArray[4].value, regArray[4].value, pc, regArray[4].value+4, regArray[4].name, pc);
    regArray[4].value += 4;
}

void pushl()
{
    //for getting rA
    char byte;
    Reg *rA = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    if(((0XF & byte) != 0XF) || (!(rA)))//check register validity
        invalidReg();
    setMemoryInt(regArray[4].value - 4, rA->value);
    regArray[4].value -= 4;
    printf(" %s%26s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> [0x%08X], 0x%08X -> %s)\n", rA->name, "(", rA->name, rA->value, regArray[4].name, regArray[4].value+4, rA->value, regArray[4].value, regArray[4].value, regArray[4].name);
}

void popl()
{
    //for getting rA
    char byte;
    Reg *rA = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    if(((0XF & byte) != 0XF) || (!(rA)))//check register validity
        invalidReg();
    getMemoryInt(regArray[4].value, &(rA->value));
    printf(" %s%27s%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s, 0x%08X -> %s)\n", rA->name, "(", regArray[4].name, regArray[4].value, regArray[4].value, rA->value, regArray[4].value+4, regArray[4].name, rA->value, rA->name);
    regArray[4].value += 4;
}

void addl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();

    //update CC
    char Z = '.', S = '.', O = '.';
    if(((rB->value + rA->value) == 0) && (!cc.ZF)){cc.ZF = 1; Z = 'Z';}else  cc.ZF = 0;
    if(((rB->value + rA->value) < 0) && (!cc.SF)){cc.SF = 1; S = 'S';}else cc.SF = 0;
    if(((rA->value < 0 == rB->value < 0) && (((rB->value + rA->value) < 0) != (rA->value < 0))) && (!cc.OF)){cc.OF = 1; O = 'O';}else cc.OF = 0;

    printf(" %s,%s%22s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s, %c%c%c -> CC)\n", rA->name, rB->name, "(", rA->name, rA->value, rB->name, rB->value, rB->value + rA->value, rB->name, Z, S, O);

    rB->value = rB->value + rA->value;
}

void subl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();

    //update CC
    char Z = '.', S = '.', O = '.';
    if(((rB->value - rA->value) == 0) && (!cc.ZF)){cc.ZF = 1; Z = 'Z';}else  cc.ZF = 0;
    if(((rB->value - rA->value) < 0) && (!cc.SF)){cc.SF = 1; S = 'S';}else cc.SF = 0;
    if((((rB->value - rA->value) < rB->value) != (rA->value > 0)) && (!cc.OF)){cc.OF = 1; O = 'O';}else cc.OF = 0;

    printf(" %s,%s%22s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s, %c%c%c -> CC)\n", rA->name, rB->name, "(", rA->name, rA->value, rB->name, rB->value, rB->value - rA->value, rB->name, Z, S, O);

    rB->value = rB->value - rA->value;
}

void andl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();

    //update CC
    char Z = '.', S = '.', O = '.';
    if(((rB->value & rA->value) == 0) && (!cc.ZF)){cc.ZF = 1; Z = 'Z';}else  cc.ZF = 0;
    if(((rB->value & rA->value) < 0) && (!cc.SF)){cc.SF = 1; S = 'S';}else cc.SF = 0;
    if(cc.OF){cc.OF = 0; O = 'O';}

    printf(" %s,%s%22s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s, %c%c%c -> CC)\n", rA->name, rB->name, "(", rA->name, rA->value, rB->name, rB->value, rB->value & rA->value, rB->name, Z, S, O);

    rB->value = rB->value & rA->value;
}

void xorl()
{
    //for getting rA and rB
    char byte;
    Reg *rA = NULLC, *rB = NULLC;

    getMemoryChar(pc++, &byte);
    rA = &regArray[0XF & (byte >> 4)];
    rB = &regArray[0XF & byte];
    if((!(rA)) || (!(rB)))//check register validity
        invalidReg();

    //update CC
    char Z = '.', S = '.', O = '.';
    if(((rB->value ^ rA->value) == 0) && (!cc.ZF)){cc.ZF = 1; Z = 'Z';}else  cc.ZF = 0;
    if(((rB->value ^ rA->value) < 0) && (!cc.SF)){cc.SF = 1; S = 'S';}else cc.SF = 0;
    if(cc.OF){cc.OF = 0; O = 'O';}

    printf(" %s,%s%22s%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s, %c%c%c -> CC)\n", rA->name, rB->name, "(", rA->name, rA->value, rB->name, rB->value, rB->value ^ rA->value, rB->name, Z, S, O);

    rB->value = rB->value ^ rA->value;
}

void jmp()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    printf(" 0x%08X%22s0x%08X -> PC)\n", Dest, "(", Dest);
    pc = Dest;
}

void jle()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if((cc.SF^cc.OF)|cc.ZF) //check condition
    {
        printf(" 0x%08X%22sZF->%d, SF->%d, OF->%d, 0x%08X -> PC)\n", Dest, "(", cc.ZF, cc.SF, cc.OF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%22sZF->%d, SF->%d, OF->%d, no jump)\n", Dest, "(", cc.ZF, cc.SF, cc.OF);
        pc += 4;
    }
}

void jl()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if(cc.SF^cc.OF) //check condition
    {
        printf(" 0x%08X%23sSF->%d, OF->%d, 0x%08X -> PC)\n", Dest, "(", cc.SF, cc.OF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%23sSF->%d, OF->%d, no jump)\n", Dest, "(", cc.SF, cc.OF);
        pc += 4;
    }
}

void je()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if(cc.ZF) //check condition
    {
        printf(" 0x%08X%23sZF->%d, 0x%08X -> PC)\n", Dest, "(", cc.ZF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%23sZF->%d, no jump)\n", Dest, "(", cc.ZF);
        pc += 4;
    }
}

void jne()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if(cc.ZF == 0) //check condition
    {
        printf(" 0x%08X%22sZF->%d, 0x%08X -> PC)\n", Dest, "(", cc.ZF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%22sZF->%d, no jump)\n", Dest, "(", cc.ZF);
        pc += 4;
    }
}

void jge()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if(!(cc.SF^cc.OF)) //check condition
    {
        printf(" 0x%08X%22sSF->%d, OF->%d, 0x%08X -> PC)\n", Dest, "(", cc.SF, cc.OF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%22sSF->%d, OF->%d, no jump)\n", Dest, "(", cc.SF, cc.OF);
        pc += 4;
    }
}

void jg()
{
    int Dest; //for getting Dest
    getMemoryInt(pc, &Dest);
    if((!(cc.SF^cc.OF))&(!cc.ZF)) //check condition
    {
        printf(" 0x%08X%23sZF->%d, SF->%d, OF->%d, 0x%08X -> PC)\n", Dest, "(", cc.ZF, cc.SF, cc.OF, Dest);
        pc = Dest;
    }
    else
    {
        printf(" 0x%08X%23sZF->%d, SF->%d, OF->%d, no jump)\n", Dest, "(", cc.ZF, cc.SF, cc.OF);
        pc += 4;
    }
}
