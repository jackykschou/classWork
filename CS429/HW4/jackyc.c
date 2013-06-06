/* UT EID : kc32547 */
//this program read a formatted input and perform a topological sort on it the print the sorted result
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define NEWLINECHARACTER 0x0A
#define SPACECHARACTER 0x20
#define RETURN 0x0D
#define TABCHARACTER 0x09
#define ORIGINALMAXTABLESIZE 20 //the number of symbols that the table can hold at the start (original size of table)
#define MAXLINEBYTESIZE 250 //maximum size of characters in a line of input
#define MAXSYMBOLBYTESIZE 250 //maximum size of a symbol (the string)
#define NULLC '\0'

typedef struct followerNode followerNode;
typedef struct symbol symbol;
typedef struct table table;

//a linked-list liked follower list
struct followerNode
{
    followerNode *next; //the next follower
    int index; //the index (hash code) of the follower
};

//structure for a symbol
struct symbol
{
    char *string; //how the string of the symbol
    int precederNum; //number of symbols that precede this symbol
    followerNode follower; //a list of index (index in ASCII table) of symbols that follows this symbol. This is a root which index is always 0
};

//structure for a table
struct table
{
    int capacity; //the maximum size of the table
    int size; //the number of symbol exists in the table
    symbol *tableHead; //pointer to the head of the table
};

//function prototypes
void issueMemoryError();
int getHash(char*, int);
int linearPorbing(int , table*);
table *expandTable(table*);
void updateTable(char*, char*, table*);
int removeFromTable(table*, char*);
int fgetline(FILE*, char*);
int getSymbol(char*, char*);
void freeFollowers(followerNode*);

int main(int argc, char *argv[])
{
    FILE *fp; //file pointer

    if(fp = fopen(argv[1], "r"))
    {
        //these 2 variables are used to get the symbols (string)
        char *s1 = 0;
        char *s2 = 0;
        char *dummy = 0; //a dummy string for checking if the line has more than 2 symbols
        int numSymbolLine = 0; //to keep tract the number of symbols in each line of the input file to indicate possible error
        int lineNum = 1; //keep track of line number in case there is an error
        char *line= 0;
        table *tPtr=0;

        //allocate memory for different variables, print error if fail
        s1 = (char*)malloc(MAXSYMBOLBYTESIZE);
        s2 = (char*)malloc(MAXSYMBOLBYTESIZE);
        dummy = (char*)malloc(MAXSYMBOLBYTESIZE);
        line = (char*)malloc(MAXLINEBYTESIZE);
        tPtr = (table*)calloc(1, sizeof(table));
        if(!(s1 && s2 && dummy && line && tPtr))
            issueMemoryError();

        //initialize the capacity and umber of symbols of the hash table
        tPtr->capacity = ORIGINALMAXTABLESIZE;
        tPtr->size = 0;
        //allocate memory for the hash table for the sorting.
        if(!(tPtr->tableHead = (symbol*)calloc(tPtr->capacity, sizeof(symbol)))) //allocate memory, print error if fail
            issueMemoryError();

        while(fgetline(fp, line) != EOF) //read file line by line
        {
            int i = 0; //use to keep track of the index of the string of the line of the input being read
            while(numSymbolLine != 2) //get the 2 symbols from the line
            {
                if(!numSymbolLine) //get the first symbol
                {
                    i += getSymbol(&line[i], s1);
                    if(!strlen(s1)) //if the first symbol is an empty string, this means the line is empty, skip the line
                        break;
                }
                else if(numSymbolLine == 1) //get the second symbol
                {
                    i += getSymbol(&line[i], s2);
                    if(!strlen(s2)) //if the second symbol is an empty string, this means the there is only one symbol on the line, issue an error
                    {
                        printf("Bad input data at line %d: %s\n", lineNum, line);
                        return 1;
                    }
                    //check for a possible 3 symbol
                    getSymbol(&line[i], dummy);
                    if(strlen(dummy)) //if there more than 3 symbols, issue an error
                    {
                        printf("Bad input data at line %d: %s\n", lineNum, line);
                        return 1;
                    }

                    if((tPtr->size) > ((tPtr->capacity) / 2)) //expand the table if the number of elements are larger than half the capacity of the table
                          tPtr = expandTable(tPtr);

                    //update the hash table
                    updateTable(s1, s2, tPtr);
                }
                numSymbolLine++;
            }
            numSymbolLine &= 0; //reset the number of symbol
            lineNum++; //update line number
        }

        //free the memory allocated that will bot be used again
        free(line);
        free(s1);
        free(s2);
        free(dummy);

        fclose(fp); //close the input file

        //now start the sorting process

        /*at each cycle of the sorting, look through the table constructed to see if there is any symbol left. If no symbol is
        left, the sorting is done. Otherwise, if there is a at least one symbol left in the table, remove the symbol with the smallest
        ASCII index whose the number of preceder is 0 from the table, decrease the number of preceder of all it followers by 1,
        and append that symbol to the output string. If there is at least one symbol in the table but not symbol have zero as number of
        preceder, there is a loop formed and it is an error, and the sorting fails*/

        char *output; //a string to hold the output
        if(!(output = (char*)malloc(MAXSYMBOLBYTESIZE))) //allocate memory, print error if fail
           issueMemoryError();

        while(tPtr->size) //try to remove as long as the table is not empty
        {
            if(removeFromTable(tPtr, output) == -1) //if a loop is formed, issue the error
            {
               printf("\nThere is no possible solution\n");
               return 1;
            }
            printf("%s\n", output);
        }

        //free the memory allocated that will bot be used again
        free(output);
        free(tPtr->tableHead);
        free(tPtr);

        return 0;
    }
    else //fail to open the file
    {
        printf("File does not exist!\n");
        return 1;
    }
}
    //function that print a memory not enough message and exit the program
    void issueMemoryError()
    {
        printf("Not enough memory\n");
        exit(1);
    }

    //hash function for hashing a symbol to produce the hash code for putting it into the table
    int getHash(char *s, int tableSize)
    {
        char c;
        int i = 0;
        int hashCode = 0; //hash code to return
        while(c = s[i++]) //perform the hashing
        {
            hashCode = ((hashCode * 128) + c) % tableSize; //128 is just a random number for hashing
        }
        if(hashCode < 0) //if, the hashCode is negative due to overflow, make the hash code to positive
            hashCode += tableSize;

        return hashCode;
    }

    //function that performs linear porbing and returns the new index of the hash table
    int linearPorbing(int hashCode, table *tPtr)
    {
        //look for an empty position
        while((tPtr->tableHead)[hashCode % (tPtr->capacity)].string)
        {
            hashCode++;
        }
        return hashCode % tPtr->capacity;
    }

    //function that checks if a symbol exists in a given table, return the index of the symbol in the table if exists, returns -1 if not exists
    int tableContains(char *symbol, table *t)
    {
        int hashCode = getHash(symbol, t->capacity);
        int i;
        for(i = 0; i < t->capacity; i++, hashCode++) //look through the entire table, start with the hash code of the symbol
        {
            if(((t->tableHead)[hashCode % t->capacity].string != 0) && (strcmp((t->tableHead)[hashCode % t->capacity].string, symbol) == 0))
            {
                return hashCode % (t->capacity); //symbol found
            }
        }
        return -1; //symbol not in table
    }

    /*function that expands the size of the hash table into double size and rehash the symbols that are already in the table.
    It returns the the new table(pointer)*/
    table* expandTable(table* oldTable)
    {
        //create the new expanded table
        table *newTable;
        if(!(newTable = (table*)calloc(1, sizeof(table)))) //allocate memory, print error if fail
            issueMemoryError();
        newTable->capacity = (oldTable->capacity) * 2;
        newTable->size = oldTable->size;
        if(!(newTable->tableHead = (symbol*)calloc(newTable->capacity, sizeof(symbol)))) //allocate memory, print error if fail
            issueMemoryError();

        int i = 0;
        for(i = 0; i < oldTable->capacity; i++) //rehash the table
        {
            if((oldTable->tableHead)[i].string) //if the position of the table is not empty, perform the rehashing
            {
                //get the new hash code of the symbol
                int newHashCode = getHash((oldTable->tableHead)[i].string, newTable->capacity);

                if(((newTable->tableHead)[newHashCode]).string) //if clustering occur (the position is not empty), perform linear porbing
                    newHashCode = linearPorbing(newHashCode, newTable);

                if(!(((newTable->tableHead)[newHashCode]).string = (char*)calloc(1, strlen((oldTable->tableHead)[i].string) + 1))) //allocate memory, print error if fail
                    issueMemoryError();
                //update the string
                strcpy(((newTable->tableHead)[newHashCode]).string, (oldTable->tableHead)[i].string);

                //update the number of preceder
                ((newTable->tableHead)[newHashCode]).precederNum = (oldTable->tableHead)[i].precederNum;

                //update the follower list
                ((newTable->tableHead)[newHashCode]).follower = (oldTable->tableHead)[i].follower;
            }
        }

        for(i = 0; i < oldTable->capacity; i++) //update the index of the follower, look through the table again
        {
            if(((((oldTable->tableHead)[i]).follower).next))
            {
                //update the index of the follower

                followerNode *oldCurrent = &(((oldTable->tableHead)[i]).follower); //create the "traveller" of the old table
                followerNode *newCurrent = &(((newTable->tableHead)[tableContains((oldTable->tableHead)[i].string, newTable)]).follower); //create the "traveller" of the new table
                while(oldCurrent->next) //travel throught the list
                {
                    if(!((newCurrent->next) = (followerNode*)calloc(1, sizeof(followerNode)))) //allocate memory, print error if fail
                        issueMemoryError();
                    int newIndex = tableContains((oldTable->tableHead)[oldCurrent->index].string, newTable); //get the hash value of the follower
                    newCurrent->index = newIndex; //put new index in the follower of the new table
                    oldCurrent = oldCurrent->next;
                    newCurrent = newCurrent->next;
                }
                freeFollowers(&(((oldTable->tableHead)[i]).follower)); //free the memory allocated for list of followers
            }
        }

        //release the memory allocated for the old table
        for(i = 0; i < oldTable->capacity; i++)
        {
            if((oldTable->tableHead)[i].string);
                free((oldTable->tableHead)[i].string);
        }
        free(oldTable);
        free(oldTable->tableHead);
        return newTable; //return the new table
    }

    //function that update the table for sorting given a pair of character
    void updateTable(char *from, char *to, table* t)
    {
        int fromHashCode; //hash code for the first symbol
        int toHashCode; //hash code for the second symbol

        //add the symbols to the table if they do not exist
        if((fromHashCode = tableContains(from, t)) == -1)
        {
            t->size++;
            fromHashCode = getHash(from, t->capacity); //get the new hash code for the first symbol
            //if clustering occur (the position is not empty), perform linear porbing
            if(((t->tableHead)[fromHashCode]).string)
                fromHashCode = linearPorbing(fromHashCode, t);

            if(!((t->tableHead)[fromHashCode].string = (char*)calloc(1, strlen(from) + 1))) //allocate memory, print error if fail
                issueMemoryError();
            strcpy((t->tableHead)[fromHashCode].string, from);
        }

        if((toHashCode = tableContains(to, t)) == -1)
        {
            t->size++;
            toHashCode = getHash(to, t->capacity); //get the new hash code for the second symbol
            //if clustering occur (the position is not empty), perform linear porbing
            if(((t->tableHead)[toHashCode]).string)
                toHashCode = linearPorbing(toHashCode, t);
            if(!((t->tableHead)[toHashCode].string = (char*)calloc(1, strlen(to) + 1))) //allocate memory, print error if fail
                issueMemoryError();
            strcpy((t->tableHead)[toHashCode].string, to);
        }

        //add a new follower to the first symbol (the from char)

        //pointer to followerNode to "travel" to the end the list of followers for appending the new follower
        followerNode *current = &(((t->tableHead)[fromHashCode].follower));
        while(current->next)
        {
            current = current->next;
        }
        if(!(current->next = (followerNode*)calloc(1, sizeof(followerNode)))) //allocate new memory for the new index of follower of the list of pointer
            issueMemoryError();
        current->index = toHashCode; //add the new follower's hashCode as index of the hash table

        //increase the number of preceders of the second symbol (the to char)
        (t->tableHead)[toHashCode].precederNum++;
    }

    /*function that remove the smallest symbol whose the number of preceder is 0 from a given table
      copy the symbol to the argument if success, returns -1 if error occurs (a loop is formed), return a non-positive number if success */
    int removeFromTable(table *t, char *s)
    {
        int symbolIndex = -1; //hold the possible of the index of the symbol to be removed
        //iterate throught the entire table in ascending order
        int i;
        for(i = 0; i < t->capacity; i++)
        {
            if((t->tableHead)[i].string && !((t->tableHead)[i].precederNum)) //test if there is a symbol in the table who has no preceder
            {
                if(symbolIndex == -1) //if this is the first symbol found in table, get it anyway
                {
                    strcpy(s, (t->tableHead)[i].string);
                    t->size--;
                    symbolIndex = i;
                }
                else if(strcmp((t->tableHead)[i].string, s) < 0) //check if new symbol found is smaller
                {
                    strcpy(s, (t->tableHead)[i].string);
                    symbolIndex = i;
                }
            }
        }

        if(symbolIndex != -1)
        {
            free((t->tableHead)[symbolIndex].string); //release the memory allocated for the symbol string
            (t->tableHead)[symbolIndex].string = 0; //set the string to null pointer (the char pointer)

            //decreases the number of preceder of all followers of the symbol being removed by 1 if the symbol has follower
            if(((((t->tableHead)[symbolIndex]).follower).next))
            {
                followerNode *current = &((t->tableHead)[symbolIndex].follower); //set the "traveller of the list" to the head of the follower list

                while(current->next)
                {
                    (t->tableHead)[current->index].precederNum--;
                    current = current->next;
                }
                freeFollowers(&((t->tableHead)[symbolIndex].follower)); //free the memory allocated for list of followers
            }
        }

        return symbolIndex; //return -1 if loop is formed
    }

    //function that read a line of text from a file as a string, return EOF if end of file reached, return 1 if a line is successfully read
    int fgetline(FILE *fp, char* s)
    {
        if((*s = fgetc(fp)) == EOF) //where there is no more data to read
            return EOF;

        ungetc(*s, fp); //put back that 'special check' character

        while(((*s = fgetc(fp)) != EOF) && (*s != NEWLINECHARACTER)) //when *s is equal to EOF, this means it is the last line to read
        {
            s++;
        }
        *s = NULLC; //append the null character at the end to make it a string
        return 1;
    }

    //function that gets one symbol from the line, it returns the number of characters it read
    int getSymbol(char *line, char *symbol)
    {

        int i = 0;
        while((line[i] == TABCHARACTER) || (line[i] == SPACECHARACTER) || (line[i] == RETURN)) //skips all space characters in the front
        {
            i++;
        }
        int j = i;
        while(line[j] && (line[j] != TABCHARACTER) && (line[j] != SPACECHARACTER) && (line[j] != RETURN)) //read building the string until a space or the end of line is reached
        {
            symbol[j-i] = line[j];
            j++;
        }
        symbol[j-i] = NULLC; //append a null character to make it a string
        return strlen(symbol) + i; //return the total number of characters read(number of spaces + number of characters of the string)
    }

    //function that free a list of followers
    void freeFollowers(followerNode* listNode)
    {
        if(listNode->next) //recursive case: travel to the next follower
        {
            freeFollowers(listNode->next);
            free(listNode->next);
        }
        else //base case
            return;

    }

