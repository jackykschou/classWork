/* UT EID : kc32547 */
//this program stimulates the Y86 architecture, reading in a binary file and process it
#define MAGICNUMBER 0X7962
#define MAGICNUMBERSIZE 2
#define MEMORYPAGESIZE 65536
#define ADDRESSSIZE 2
#define BYTECOUNTSIZE 2
#define BUFFERSIZE 256
#define MAXINSTRUCTIONSIZE 256
#define MAXADDRESS 0XFFFFFFFF
#define NULLC '\0'
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "programStateStruct.h"
#include "Y86Instruction.h"

Reg *regArray; //an array of registers (the index of array is the corresponding id of the registers
CC cc = {0, 0, 0}; //create the condition code
unsigned int pc;
unsigned char **memory;

/*declare a function that returns a short by combining 2 unsigned characters
    (combine 2 bytes, the first argument being the most significicant byte)*/
unsigned short combine2char(unsigned char a, unsigned char b);

int main(int argc, unsigned char *argv[])
{

    //create the registers
    Reg eax = {"%EAX"};
    Reg ecx = {"%ECX"};
    Reg edx = {"%EDX"};
    Reg ebx = {"%EBX"};
    Reg esp = {"%ESP"};
    Reg ebp = {"%EBP"};
    Reg esi = {"%ESI"};
    Reg edi = {"%EDI"};

    FILE *fp;

    //allocation of memory
    regArray = (Reg*)calloc(sizeof(Reg), 16);
    unsigned char *buffer = (unsigned char*)malloc(BUFFERSIZE);
    memory = (unsigned char**)calloc(MEMORYPAGESIZE, sizeof(unsigned char*)); //memory is composed of a 32bytes array of 32bytes (total of ~4GB)
    if((!buffer) || (!memory) || (!regArray)) //if allocation of memory fail, print error
        issueMemoryError();
    //set the registers to the array
    regArray[0] = eax, regArray[1] = ecx, regArray[2] = edx, regArray[3] = ebx, regArray[4] = esp,
    regArray[5] = ebp, regArray[6] = esi, regArray[7] = edi;
    unsigned int address;
    unsigned short byteCount;
    unsigned int startAddress;
    int firstBlock = 1; //boolean to indicate the first time of block reading
    int endOfFile; //used to test if end of file is reached
    //array of string for holding instructions
    unsigned char *instruction[MAXINSTRUCTIONSIZE] = {[0x00] = "halt", [0x10] = "nop", [0x20] = "rrmovl", "cmovle", "cmovl",
                                            "cmove", "cmovne", "cmovge", "cmovg", [0x30] = "irmovl", [0x40] = "rmmovl",
                                            [0x50] = "mrmovl", [0x80] = "call", [0x90] = "ret", [0xA0] = "pushl",
                                            [0xB0] = "popl", [0x60] = "addl", "subl", "andl", "xorl", [0x70] = "jmp",
                                            "jle", "jl", "je", "jne", "jge", "jg"};
    //array of pointer to functions of the instruction operations
    void (*opFunction[MAXINSTRUCTIONSIZE])() = {[0x00] = &halt, [0x10] = &nop, [0x20] = &rrmovl, &cmovle, &cmovl,
                                                &cmove, &cmovne, &cmovge, &cmovg, [0x30] = &irmovl, [0x40] = &rmmovl,
                                                [0x50] = &mrmovl, [0x80] = &call, [0x90] = &ret, [0xA0] = &pushl,
                                                [0xB0] = &popl, [0x60] = &addl, &subl, &andl, &xorl, [0x70] = &jmp,
                                                &jle, &jl, &je, &jne, &jge, &jg};
    //first read data into memory (stop if error occur)
    if(fp = fopen(argv[1], "rb"))
    {
        //the file is opened successfully;
        fread(buffer, 1, MAGICNUMBERSIZE, fp);
        if(combine2char(buffer[0], buffer[1]) != (unsigned short)MAGICNUMBER)//test if it is a Y86 file
        {
            printf("The file is not a Y86 object file\n");
            return 1;
        }
        else
        {
            while((endOfFile = fread(buffer, 1, ADDRESSSIZE + BYTECOUNTSIZE, fp)) != 0) //while not end of file
            {
                if(endOfFile != ADDRESSSIZE + BYTECOUNTSIZE) //test if there is error in format of Y86
                {
                    printf("The Y86 object file is corrupted\n");
                    return 1;
                }
                //read address and byte count
                address = combine2char(buffer[0], buffer[1]);

                if(firstBlock) //get the first starting address
                {
                    startAddress = address;
                    firstBlock = 0;
                }
                byteCount = combine2char(buffer[2], buffer[3]);
                //read data
                if((fread(buffer, 1, byteCount, fp) != byteCount) || (byteCount + address) > MAXADDRESS)
                {
                    printf("The Y86 object file is corrupted\n"); //test if there is error in format of Y86
                    return 1;
                }
                //write data into memory
                int counter; //counter for for loop in iterating throught data
                for(counter = 0; counter < byteCount; counter++)
                {
                    setMemoryChar(address+counter, buffer[counter]);
                }
            }
        }
        fclose(fp);
        free(buffer);
    }
    else //fail to read a file
    {
        printf("File does not exist!\n");
        return 1;
    }

    unsigned char opCode; //holding the op Code
    pc = startAddress; //pc starts at the first loaded address
    //process the data
    do
    {
        printf("0x%08X: ", pc);
        getMemoryChar(pc++, &opCode); //get opcode
        if(!(instruction[opCode])) //issue error opcode if necessary
            invalidOpcode(opCode);
        printf("%s", instruction[opCode]);
        opFunction[opCode]();
    }while(strcmp(instruction[opCode], "halt"));

    //free the memory
    int i;
    for(i = 0; i < MEMORYPAGESIZE; i++)
    {
        if(memory[i])
            free(memory[i]);
    }
    free(memory);
    free(regArray);
    return 0;

}

/*a function that returns a short by combining 2 unsigned characters
    (combine 2 bytes, the first argument being the most significicant byte)*/
unsigned short combine2char(unsigned char a,unsigned char b)
{
    unsigned short result = b;
    result |= (a << 8);
    return result;
}


