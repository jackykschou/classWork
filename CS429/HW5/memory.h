//head file for functions that manipulates and manages the memory of the Y86 ISA

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

void issueMemoryError();
void issueInvalidAddress();
void setMemoryChar(unsigned int, char);
void setMemoryInt(unsigned int, int);
void getMemoryChar(unsigned int, char*);
void getMemoryInt(unsigned int, int*);

#endif // MEMORY_H_INCLUDED
