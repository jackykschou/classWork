
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The code to read a memory trace and simulate it on the various
   caches. */

#include "global.h"
#include "utils.h"        // utility functions

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* n is a power of two, which one? */
/* 1 -> 0; 2 -> 1; ... */
int which_power(int n)
{
    int i = 0;
    int t = 1;
    while (n > t)
        {
            t += t;
            i += 1;
        }
    return(i);
}

/* generate a mask of n low order bits */
/* if we want a mask of two bits, shift a 1 two
   bits over (100), and then subtract one (011). */
unsigned int mask_of(int n)
{
    return (n <= 0) ? 0 : ((1 << n) - 1); //modified (simplication)
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

//deleted
/*
Boolean ishex(int c)
{
    //modified (using array mapping instead of range checking)
    return ishexArray[c];
}

int hexvalue(int c)
{
    //modified (using array mapping instead of range checking)
    int value = hexvalueArray[c];
    return value ? (c - value) : -1;
}

int decvalue(int c)
{
    //modified (using array mapping instead of range checking)
    int value = decvalueArray[c];
    return value ? (c - value) : -1;
}
*/


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* MALLOC space for a string and copy it */

String remember_string(const String name)
{
    //modified (simplicationa)
    String p;

    if (name == NULL) return(NULL);

    /* get memory to remember file name */
    p = CAST(String , malloc(strlen(name) + 1));
    strcpy(p, name);
    return(p);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int skip_blanks(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && isspace(c)) /* keep reading */;
    return(c);
}


int skip_line(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && (c != '\n')) /* keep reading */;
    c = skip_blanks(file);
    return(c);
}


