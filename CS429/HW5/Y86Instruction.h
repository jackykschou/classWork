//header file for different operations in Y86ISA

#ifndef Y86INSTRUCTION_H_INCLUDED
#define Y86INSTRUCTION_H_INCLUDED

#include "programStateStruct.h"

void invalidReg();
void invalidOpcode(unsigned char);
void halt();
void nop();
void rrmovl();
void cmovle();
void cmovl();
void cmove();
void cmovne();
void cmovge();
void cmovg();
void irmovl();
void rmmovl();
void mrmovl();
void call();
void ret();
void pushl();
void popl();
void addl();
void subl();
void andl();
void xorl();
void jmp();
void jle();
void jl();
void je();
void jne();
void jge();
void jg();

#endif // Y86INSTRUCTION_H_INCLUDED
