//header file for structures of programme visible state in Y86ISA

#ifndef PROGRAMSTATESTRUCT_H_INCLUDED
#define PROGRAMSTATESTRUCT_H_INCLUDED

typedef struct Reg Reg;
typedef struct CC CC;

//structure for register
struct Reg
{
    char *name;
    int value;
};

//structure for conditional code
struct CC
{
    unsigned int ZF:1;
    unsigned int SF:1;
    unsigned int OF:1;
};

#endif // PROGRAMSTATESTRUCT_H_INCLUDED
