//source file for functions that manipulates and manages the memory of the Y86 ISA


#ifndef MEMORYPAGESIZE
#define MEMORYPAGESIZE 66536
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

extern char **memory;

//function that print a memory not enough message and exit the program
void issueMemoryError()
{
    printf("Not enough memory!\n");
    exit(1);
}

//function that print a invalid memory address message and exit the program
void issueInvalidAddress()
{
    printf("invalid memory address\n");
    exit(1);
}

//function that sets a byte of memory
void setMemoryChar(unsigned int address, char data)
{
    //allocate memory if necessary
    if(!(memory[address/MEMORYPAGESIZE]))
        if(!(memory[address/MEMORYPAGESIZE] = (char*) calloc(MEMORYPAGESIZE, 1))) //allocate memory, issue error if allocation fail
            issueMemoryError();
    ((memory)[address/MEMORYPAGESIZE])[address%MEMORYPAGESIZE] = data;
}

//function that sets 4 bytes of memory
void setMemoryInt(unsigned int address, int data)
{
    //allocate memory if necessary
    if(!(memory[address/MEMORYPAGESIZE]))
        if(!(memory[address/MEMORYPAGESIZE] = (char*) calloc(MEMORYPAGESIZE, 1))) //allocate memory, issue error if allocation fail
            issueMemoryError();
    unsigned int *intMemory = (int*)&(memory[address/MEMORYPAGESIZE][address%MEMORYPAGESIZE]);
    *intMemory = data;
}

//function that gets a char from the memory address given
void getMemoryChar(unsigned int address, char* dest)
{
    if(!(memory[address/MEMORYPAGESIZE])) //issue error is fail to access memory
        issueInvalidAddress();
    *dest = memory[address/MEMORYPAGESIZE][address%MEMORYPAGESIZE];
}

//function that gets 4 bytes from the memory address given
void getMemoryInt(unsigned int address, int* dest)
{
    if(!(memory[address/MEMORYPAGESIZE])) //issue error is fail to access memory
        issueInvalidAddress();
    *dest = *((int*)((*(memory + address/MEMORYPAGESIZE)) + (address%MEMORYPAGESIZE)));
}
