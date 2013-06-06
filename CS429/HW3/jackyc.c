/* UT EID : kc32547 */
//this program read a Y86 object file into memory and then print, stimulating the Y86 architecture
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#define MAGICNUMBER 0X7962
#define MAGICNUMBERSIZE 2
#define MEMORYSIZE 66536
#define ADDRESSSIZE 2
#define BYTECOUNTSIZE 2
#define BUFFERSIZE 256
#define MAXINSTRUCTIONSIZE 256
#define MAXADDRESS 0XFFFFFFFF

int main(int argc, char *argv[])
{
    /*declare a function that returns a short by combining 2 characters
    (combine 2 bytes, the first argument being the most significicant byte)*/
    unsigned short combine2Char(unsigned char a,unsigned char b);

    FILE *fp;
    unsigned char *buffer = (char*)malloc(BUFFERSIZE);
    unsigned char *memory = (char*)calloc(MEMORYSIZE, 1);
    unsigned int address;
    unsigned short byteCount;
    unsigned int maxAddress = 0;
    unsigned int minAddress = MAXADDRESS;
    int endOfFile; //used to test if end of file is reached
    //array of string for holding instructions
    char *instruction[MAXINSTRUCTIONSIZE] = {[0x00] = "halt", [0x10] = "nop", [0x20] = "rrmovl", "cmovle", "cmovl",
                                            "cmove", "cmovne", "cmovge", "cmovg", [0x30] = "irmovl", [0x40] = "rmmovl",
                                            [0x50] = "mrmovl", [0x80] = "call", [0x90] = "ret", [0xA0] = "pushl",
                                            [0xB0] = "popl", [0x60] = "addl", "subl", "andl", "xorl", [0x70] = "jmp",
                                            "jle", "jl", "je", "jne", "jge", "jg"};

    //first read data into memory (stop if error occur)
    if(fp = fopen(argv[1], "rb"))
    {
        //the file is opened successfully;
        fread(buffer, 1, MAGICNUMBERSIZE, fp);
        if(combine2Char(buffer[0], buffer[1]) != (short)MAGICNUMBER)//test if it is a Y86 file
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
                address = combine2Char(buffer[0], buffer[1]);

                if(minAddress >= address) //possibly update the minimum address
                    minAddress = address;

                byteCount = combine2Char(buffer[2], buffer[3]);
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
                    memory[counter+address] = buffer[counter];
                }
                //possibly update the maximum address
                if(maxAddress <= address + counter - 1)
                    maxAddress = address + counter - 1;
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


    //print the data
    for(minAddress; minAddress <= maxAddress; minAddress++)
    {
        printf("%04X %02X %08X %10d %s\n", minAddress, memory[minAddress], *((int*)(memory + minAddress)), *((int*)(memory + minAddress)), instruction[memory[minAddress]] == '\0'? "invalid": instruction[memory[minAddress]]);
    }
    free(memory);
    return 0;

}

/*a function that returns a short by combining 2 characters
    (combine 2 bytes, the first argument being the most significicant byte)*/
unsigned short combine2Char(unsigned char a,unsigned char b)
{
    unsigned short result = b;
    result |= (a << 8);
    return result;
}


