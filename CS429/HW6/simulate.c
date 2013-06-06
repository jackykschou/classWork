
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The code to read a memory trace and simulate it on the various
   caches. */

#include "global.h"
#include "utils.h"        // utility functions
#include "cds.h"          // Cache Description Structures
#include "caches.h"       // cache manipulation code
//add define
#define ASCIITABLESIZE 256

//add an mapping array for ishex function
static const Boolean ishexArray[ASCIITABLESIZE] =
{
    ['0'] = TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    ['a'] = TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    ['A'] = TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

//add an mapping array for ishexvalue function
static const int hexvalueArray[ASCIITABLESIZE] =
{
  ['0'] = '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
  ['a'] = 'a' - 10, 'a' - 10, 'a' - 10, 'a' - 10, 'a' - 10, 'a' - 10,
  ['A'] = 'A' - 10, 'A' - 10, 'A' - 10, 'A' - 10, 'A' - 10, 'A' - 10
};

//add an mapping array for decvalue function
static const int decvalueArray[ASCIITABLESIZE] =
{
    ['0'] = '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'
};

struct memory_reference
{
    enum memory_access_type  type;
    memory_address           address;
    unsigned int             length;
};
typedef struct memory_reference memory_reference;

static int trace_line_number;


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* read on input line */

void read_reference(FILE *trace_file, memory_reference *reference)
{
    int c;
    /* we have the first character; it defined the
       memory access type.  Skip any blanks, get the
       hexadecimal address, skip the comma and get the length */

    /* skip any leading blanks */
    c = skip_blanks(trace_file);

    memory_address a = 0;
    while (ishexArray[c]) //modified (use array indexing instead of calling function)
        {
            a = (a << 4) | (hexvalueArray[c] ? (c - hexvalueArray[c]) : -1);
            c = getc(trace_file);
        }
    if (c != ',')
        {
            fprintf(stderr, "bad trace file input at line %d: %c\n", trace_line_number, c);
            exit(-1);
        }

    /* skip the comma */
    /* and get the length */
    int n = 0;
    c = getc(trace_file);
    while (isdigit(c))
        {
            n = n * 10 + (decvalueArray[c] ? (c - decvalueArray[c]) : -1);
            c = getc(trace_file);
        }

    /* skip to end of line */
    while ((c != '\n') && (c != EOF)) c = getc(trace_file);

    /* define reference fields */
    reference->address = a;
    reference->length = n;
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int Read_trace_file_line(FILE *trace_file, memory_reference *reference)
{
    int c;

    trace_line_number = 0;

    while ((c = getc(trace_file)) != EOF)
        {
            /* start the next line */
            trace_line_number += 1;

            /* skip any leading blanks */
            while (isspace(c) && (c != EOF)) c = getc(trace_file);

            /* what is the character ? */
            switch (c)
                {
                case 'I': /* instruction trace */
                    {
                        reference->type = MAT_FETCH;
                        read_reference(trace_file, reference);
                        return('I');
                    }

                case 'S': /* store */
                    {
                        reference->type = MAT_STORE;
                        read_reference(trace_file, reference);
                        return('S');
                    }

                case 'L': /* load */
                    {
                        reference->type = MAT_LOAD;
                        read_reference(trace_file, reference);
                        return('L');
                    }
                }

            /* apparently not a reference line.  There are a bunch
               of other lines that valgrind puts out.  They start
               with  ====, or --, or such.  Skip the entire line. */
            /* skip to end of line */
            while ((c != '\n') && (c != EOF)) c = getc(trace_file);
        }
    return(EOF);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* LFU counts the number of times something is used.  To prevent
   a large number from just sitting there, we cause it to decay
   over time.  Every "tick" time units, we shift left one bit,
   so that eventually a count will go to zero if it is not continuing
   to be used. */

void Check_For_Decay(CDS *cds)
{
    if (cds->replacement_policy != CRP_LFU) return;

    if ((cds->number_of_memory_reference % cds->LFU_Decay_Interval) == 0)
        {
            int i;

            //modified (add variable to reduce the number of memory references)
            int number_of_cache_entries = cds->number_of_cache_entries;
            cache_line *cLine = cds->c;

            if (debug) fprintf(debug_file, "%s: LFU decay for all LFU counters\n", cds->name);

             //modified (loop parallelism)
            int limit = number_of_cache_entries - 9;
            for (i = 0; i < limit; i+=10)
                {
                    cLine[i].replacement_data = cLine[i].replacement_data >> 1;
                    cLine[i+1].replacement_data = cLine[i+1].replacement_data >> 1;
                    cLine[i+2].replacement_data = cLine[i+2].replacement_data >> 1;
                    cLine[i+3].replacement_data = cLine[i+3].replacement_data >> 1;
                    cLine[i+4].replacement_data = cLine[i+4].replacement_data >> 1;
                    cLine[i+5].replacement_data = cLine[i+5].replacement_data >> 1;
                    cLine[i+6].replacement_data = cLine[i+6].replacement_data >> 1;
                    cLine[i+7].replacement_data = cLine[i+7].replacement_data >> 1;
                    cLine[i+8].replacement_data = cLine[i+8].replacement_data >> 1;
                    cLine[i+9].replacement_data = cLine[i+9].replacement_data >> 1;
                }
            //special case for loop parallelism
            for(;i < number_of_cache_entries; i++)
            {
                cLine[i].replacement_data = cLine[i].replacement_data >> 1;
            }
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int Replacement_policy(CDS *cds, int first_index, int set_size)
{
    int i;
    int victim;
    //modified (add variable to reduce the number of memory references)
    cache_line *cLine = cds->c;

    if (debug) fprintf(debug_file, "%s: look for victim in %d lines starting at %d\n", cds->name,  set_size, first_index);

    //modified (loop parallelism)
    int limit = set_size - 4;
    /* first look to see if any entry is empty */
    for (i = 0; i < limit; i++)
        {
            if (!(cLine[first_index+i].valid))
                {
                    victim = first_index+i;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
            else if (!(cLine[first_index+i+1].valid))
                {
                    victim = first_index+i+1;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
            else if (!(cLine[first_index+i+2].valid))
                {
                    victim = first_index+i+2;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
            else if (!(cLine[first_index+i+3].valid))
                {
                    victim = first_index+i+3;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
            else if (!(cLine[first_index+i+4].valid))
                {
                    victim = first_index+i+4;
                    if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
                    return(victim);
                }
        }

    //special case for loop parallelism
    for(;i < set_size; i++)
    {
        if (!(cLine[first_index+i].valid))
        {
            victim = first_index+i;
            if (debug) fprintf(debug_file, "%s: found empty cache entry at %d\n", cds->name,  victim);
            return(victim);
        }
    }

    victim = first_index; /* default victim */
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  /* replacement data is the order we were brought in: 1, 2, 3, ... */
                    /* choose the smallest */
        case CRP_LRU: //add LRU policy
        case CRP_LFU:  /* replacement data is the number of uses, so
                      choose the smallest */
            {
                int min = cLine[first_index].replacement_data;
                if (debug) fprintf(debug_file, "%s: replacement data: [%d]: %d", cds->name, victim, min);
                for (i = 1; i < set_size; i++)
                    {
                        if (debug) fprintf(debug_file, ", [%d]: %d", first_index+i, cLine[first_index+i].replacement_data);
                        if (cLine[first_index+i].replacement_data < min)
                            {
                                victim = first_index+i;
                                min = cLine[victim].replacement_data;
                            }
                    }
            }
            break;

        case CRP_RANDOM:
            victim = first_index + (random() % set_size);
            break;
        }

    if (debug) fprintf(debug_file, "%s: found victim in entry %d\n", cds->name,  victim);
    return(victim);
}


void Set_Replacement_Policy_Data(CDS *cds, int index)
{
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  // replacement data is the order we were brought in: 1, 2, 3, ...
            cds->c[index].replacement_data = cds->number_of_memory_reference;
            break;

        case CRP_LFU:  // replacement data is a count; starts at zero
            cds->c[index].replacement_data = 0;
            Check_For_Decay(cds);
            break;

        case CRP_LRU: //add LRU
            cds->c[index].replacement_data = cds->number_of_memory_reference;
            break;

        case CRP_RANDOM:
            break;
        }
}

void Update_Replacement_Policy_Data(CDS *cds, int index)
{
    switch (cds->replacement_policy)
        {
        case CRP_FIFO:  //replacement data is the order we were brought in: 1, 2, 3, ...
            break;

        case CRP_LFU:  // replacement data is the count of the number of uses
            cds->c[index].replacement_data += 1;
            Check_For_Decay(cds);
            break;
        case CRP_LRU:
            cds->c[index].replacement_data = cds->number_of_memory_reference;
            break;
        case CRP_RANDOM:
            break;
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/*
Modified, there are different versions of Simulate_Reference_to_Cache_Line based on the number of ways of the caches
to provide possible faster loop parallelism.
*/

void Simulate_Reference_to_Cache_Line_Full(CDS *cds, memory_reference *reference) //fully associative cache version
{
    //modified (add variable to reduce the number of memory references)
    cache_line *cLine = cds->c;

    if (debug) fprintf(debug_file, "%s: Reference 0x%08X of length %d\n",
                       cds->name, reference->address, reference->length);

    /* find cache line for this reference */
    /* find number of low-order bits to mask off to find beginning cache
       line address */

    //modified (do not use function call, inline the body of the function)
    memory_address cache_address = reference->address & ~(cds->low_order_mask);

    //modified (use new fields in cds for simplication (reduce redundant calculations))
    int cache_entry_index = ((cache_address >> cds->number_of_low_order_bits) & cds->sets_bits_mask) * cds->number_of_ways;

    /* index into cache table and search the number of ways to
       try to find cache line. */
    int i;

    //modified (loop parallelism)
    int limit = cds->number_of_ways - 30 + cache_entry_index;
    for (i = cache_entry_index; i < limit; i += 31)
    {
//*****************************************************************************************************************
        if (cLine[i].valid && !(cache_address ^ cLine[i].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i);
                return;
            }
        else if (cLine[i+1].valid && !(cache_address ^ cLine[i+1].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+1);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+1].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+1);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 1);
                return;
            }
        else if (cLine[i+2].valid && !(cache_address ^ cLine[i+2].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+2);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+2].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+2);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 2);
                return;
            }
        else if (cLine[i+3].valid && !(cache_address ^ cLine[i+3].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+3);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+3].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+3);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+4].valid && !(cache_address ^ cLine[i+4].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+4);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+4].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+4);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+5].valid && !(cache_address ^ cLine[i+5].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+5);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+5].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+5);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+6].valid && !(cache_address ^ cLine[i+6].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+6);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+6].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+6);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+7].valid && !(cache_address ^ cLine[i+7].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+7);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+7].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+7);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+8].valid && !(cache_address ^ cLine[i+8].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+8);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+8].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+8);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+9].valid && !(cache_address ^ cLine[i+9].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+9);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+9].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+9);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+10].valid && !(cache_address ^ cLine[i+10].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+10);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+10].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+10);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+11].valid && !(cache_address ^ cLine[i+11].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+11);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+11].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+11);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+12].valid && !(cache_address ^ cLine[i+12].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+12);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+12].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+12);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+13].valid && !(cache_address ^ cLine[i+13].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+13);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+13].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+13);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+14].valid && !(cache_address ^ cLine[i+14].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+14);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+14].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+14);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+15].valid && !(cache_address ^ cLine[i+15].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+15);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+15].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+15);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+16].valid && !(cache_address ^ cLine[i+16].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+16);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+16].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+16);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+17].valid && !(cache_address ^ cLine[i+17].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+17);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+17].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+17);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+18].valid && !(cache_address ^ cLine[i+18].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+18);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+18].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+18);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+19].valid && !(cache_address ^ cLine[i+19].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+19);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+19].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+19);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+20].valid && !(cache_address ^ cLine[i+20].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+20);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+20].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+20);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+21].valid && !(cache_address ^ cLine[i+21].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+21);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+21].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+21);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+22].valid && !(cache_address ^ cLine[i+22].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+22);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+22].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+22);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+23].valid && !(cache_address ^ cLine[i+23].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+23);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+23].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+23);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+24].valid && !(cache_address ^ cLine[i+24].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+24);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+24].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+24);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+25].valid && !(cache_address ^ cLine[i+25].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+25);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+25].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+25);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+26].valid && !(cache_address ^ cLine[i+26].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+26);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+26].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+26);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+27].valid && !(cache_address ^ cLine[i+27].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+27);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+27].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+27);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+28].valid && !(cache_address ^ cLine[i+28].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+28);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+28].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+28);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+29].valid && !(cache_address ^ cLine[i+29].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+29);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+29].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+29);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
            else if (cLine[i+30].valid && !(cache_address ^ cLine[i+30].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+30);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+30].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+30);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }

//*****************************************************************************************************************
    }

    //special case for loop
    for(limit += 30;i < limit;i++)
    {
        if (cLine[i].valid && !(cache_address ^ cLine[i].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i);
                return;
            }
    }

    /* Did not find it. */
    cds->number_cache_misses += 1;

    /* Choose a victim from the set */
    i = Replacement_policy(cds, cache_entry_index, cds->number_of_ways); //modified
    if (debug) fprintf(debug_file, "%s: Pick victim %d to replace\n", cds->name,  i);

    /* evict victim */

    /* if victim is dirty, must write back to memory first */
    if (cLine[i].dirty)
        {
            if (debug) fprintf(debug_file, "%s: Write victim 0x%08X to memory\n", cds->name,  cLine[i].tag);
            cds->number_memory_writes += 1;
            cLine[i].dirty = FALSE;
        }

    /* fill in victim cache line for this new line */
    cLine[i].valid = TRUE;
    cLine[i].tag = cache_address;
    cLine[i].dirty = !(reference->type ^ MAT_STORE);
    if (debug) fprintf(debug_file, "%s: Read cache line 0x%08X into entry %d\n", cds->name,  cLine[i].tag, i);

    //Set_Replacement_Policy_Data(cds, victim);
    //fp_Set_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index, victim-cache_entry_index);
    Set_Replacement_Policy_Data(cds,  i);

    /* read cache line from memory into cache table */
    cds->number_memory_reads += 1;

    if (!cds->write_back && !(reference->type ^ MAT_STORE))
        {
            /* If it's not write-back, then it is write-thru.
               For write-thru, if it's a write, we write to memory. */
            cds->number_memory_writes += 1;
        }

}

void Simulate_Reference_to_Cache_Line_Direct(CDS *cds, memory_reference *reference) //Directed-mapped cache version
{
    //modified (add variable to reduce the number of memory references)
    cache_line *cLine = cds->c;

    if (debug) fprintf(debug_file, "%s: Reference 0x%08X of length %d\n",
                       cds->name, reference->address, reference->length);

    /* find cache line for this reference */
    /* find number of low-order bits to mask off to find beginning cache
       line address */

    //modified (do not use function call, inline the body of the function)
    memory_address cache_address = reference->address & ~(cds->low_order_mask);

    //modified (use new fields in cds for simplication (reduce redundant calculations))
    int cache_entry_index = ((cache_address >> cds->number_of_low_order_bits) & cds->sets_bits_mask) * cds->number_of_ways;

    /* index into cache table and search the number of ways to
       try to find cache line. */
    if (cLine[cache_entry_index].valid && !(cache_address ^ cLine[cache_entry_index].tag))
        {
            /* found it -- record cache hit and exit */
            if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                               reference->address, cache_entry_index);
            cds->number_cache_hits += 1;

            /* update reference specific info */
            if (reference->type == MAT_STORE)
                cLine[cache_entry_index].dirty = TRUE;
            //modified (use function pointer array instead of functino that use switch statement)
            Update_Replacement_Policy_Data(cds, cache_entry_index);
            //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i);
            return;
        }

    /* Did not find it. */
    cds->number_cache_misses += 1;

    /* Choose a victim from the set */
    int victim = Replacement_policy(cds, cache_entry_index, cds->number_of_ways); //modified
    if (debug) fprintf(debug_file, "%s: Pick victim %d to replace\n", cds->name,  victim);

    /* evict victim */

    /* if victim is dirty, must write back to memory first */
    if (cLine[victim].dirty)
        {
            if (debug) fprintf(debug_file, "%s: Write victim 0x%08X to memory\n", cds->name,  cLine[victim].tag);
            cds->number_memory_writes += 1;
            cLine[victim].dirty = FALSE;
        }

    /* fill in victim cache line for this new line */
    cLine[victim].valid = TRUE;
    cLine[victim].tag = cache_address;
    cLine[victim].dirty = !(reference->type ^ MAT_STORE);
    if (debug) fprintf(debug_file, "%s: Read cache line 0x%08X into entry %d\n", cds->name,  cLine[victim].tag, victim);

    //Set_Replacement_Policy_Data(cds, victim);
    //fp_Set_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index, victim-cache_entry_index);
    Set_Replacement_Policy_Data(cds,  victim);

    /* read cache line from memory into cache table */
    cds->number_memory_reads += 1;

    if (!cds->write_back && !(reference->type ^ MAT_STORE))
        {
            /* If it's not write-back, then it is write-thru.
               For write-thru, if it's a write, we write to memory. */
            cds->number_memory_writes += 1;
        }

}

void Simulate_Reference_to_Cache_Line_Other(CDS *cds, memory_reference *reference) //For cache that are not fully associative cache or directed-mapped cache
{
    //modified (add variable to reduce the number of memory references)
    cache_line *cLine = cds->c;

    if (debug) fprintf(debug_file, "%s: Reference 0x%08X of length %d\n",
                       cds->name, reference->address, reference->length);

    /* find cache line for this reference */
    /* find number of low-order bits to mask off to find beginning cache
       line address */

    //modified (do not use function call, inline the body of the function)
    memory_address cache_address = reference->address & ~(cds->low_order_mask);

    //modified (use new fields in cds for simplication (reduce redundant calculations))
    int cache_entry_index = ((cache_address >> cds->number_of_low_order_bits) & cds->sets_bits_mask) * cds->number_of_ways;

    /* index into cache table and search the number of ways to
       try to find cache line. */
    int i;

    //modified (loop parallelism)
    int limit = cds->number_of_ways - 3 + cache_entry_index;
    for (i = cache_entry_index; i < limit; i += 4)
    {
        if (cLine[i].valid && !(cache_address ^ cLine[i].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i);
                return;
            }
        else if (cLine[i+1].valid && !(cache_address ^ cLine[i+1].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+1);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+1].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+1);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 1);
                return;
            }
        else if (cLine[i+2].valid && !(cache_address ^ cLine[i+2].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+2);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+2].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+2);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 2);
                return;
            }
        else if (cLine[i+3].valid && !(cache_address ^ cLine[i+3].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i+3);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i+3].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i+3);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i + 3);
                return;
            }
    }

    //special case for loop
    for(limit+=3;i < limit;i++)
    {
        if (cLine[i].valid && !(cache_address ^ cLine[i].tag))
            {
                /* found it -- record cache hit and exit */
                if (debug) fprintf(debug_file, "%s: Found address 0x%08X in cache line %d\n", cds->name,
                                   reference->address, i);
                cds->number_cache_hits += 1;

                /* update reference specific info */
                if (reference->type == MAT_STORE)
                    cLine[i].dirty = TRUE;
                //modified (use function pointer array instead of functino that use switch statement)
                Update_Replacement_Policy_Data(cds, i);
                //fp_Update_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index , i);
                return;
            }
    }

    /* Did not find it. */
    cds->number_cache_misses += 1;

    /* Choose a victim from the set */
    i = Replacement_policy(cds, cache_entry_index, cds->number_of_ways); //modified
    if (debug) fprintf(debug_file, "%s: Pick victim %d to replace\n", cds->name,  i);

    /* evict victim */

    /* if victim is dirty, must write back to memory first */
    if (cLine[i].dirty)
        {
            if (debug) fprintf(debug_file, "%s: Write victim 0x%08X to memory\n", cds->name,  cLine[i].tag);
            cds->number_memory_writes += 1;
            cLine[i].dirty = FALSE;
        }

    /* fill in victim cache line for this new line */
    cLine[i].valid = TRUE;
    cLine[i].tag = cache_address;
    cLine[i].dirty = !(reference->type ^ MAT_STORE);
    if (debug) fprintf(debug_file, "%s: Read cache line 0x%08X into entry %d\n", cds->name,  cLine[i].tag, i);

    //Set_Replacement_Policy_Data(cds, victim);
    //fp_Set_Replacement_Policy_Data[cds->replacement_policy](cds, cache_entry_index, victim-cache_entry_index);
    Set_Replacement_Policy_Data(cds,  i);

    /* read cache line from memory into cache table */
    cds->number_memory_reads += 1;

    if (!cds->write_back && !(reference->type ^ MAT_STORE))
        {
            /* If it's not write-back, then it is write-thru.
               For write-thru, if it's a write, we write to memory. */
            cds->number_memory_writes += 1;
        }

}

//function pointer array to hold different versions of Simulate_Reference_to_Cache_Line
static void (*fp_Simulate_Reference_to_Cache_Line[TYPEOFCACHESIZE])(CDS*, memory_reference*) = {
    [FULL] = &Simulate_Reference_to_Cache_Line_Full, [DIRECT] = &Simulate_Reference_to_Cache_Line_Direct,
    [OTHER] = &Simulate_Reference_to_Cache_Line_Other
};

void Simulate_Reference_to_Memory(CDS *cds, memory_reference *reference, void (*fp)(CDS*, memory_reference*)) //modified (add function pointer arg for different type of caches)
{

    cds->number_of_memory_reference += 1;
    cds->number_of_type[reference->type] += 1;

    /* check if the entire reference fits into just one cache line */
    if ((reference->address & ~(cds->low_order_mask)) == ((reference->address + reference->length - 1) & ~(cds->low_order_mask)))
        {
            (*fp)(cds, reference);
        }
    else
        {
            /* reference spans two cache lines.  Convert it to two
               references: the first cache line, and the second cache line */

            //modified(use structure initialization instead of assigning fields one by one)
            memory_reference reference2 = {reference->type, reference->address + reference->length -1, reference->address + reference->length - reference2.address};
            memory_reference reference1 = {reference->type, reference->address, reference->length - reference2.length};
            /* easiest to compute the second part first */

            /* but we do the references first, then second */
            (*fp)(cds, &reference1);
            (*fp)(cds, &reference2);
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* read each input line, and then simulate that reference on each
   cache. */

void Simulate_Caches(String trace_file_name)
{
    FILE *trace_file;
    memory_reference reference;

    /* open input file */
    trace_file = fopen(trace_file_name, "r");
    if (trace_file == NULL)
        {
            fprintf (stderr,"Cannot open trace file %s\n", trace_file_name);
        }

    Init_caches_for_trace();

    while (Read_trace_file_line(trace_file, &reference) != EOF)
        {
            CDS *cds = CDS_root;
            while (cds != NULL)
                {
                    Simulate_Reference_to_Memory(cds, &reference, fp_Simulate_Reference_to_Cache_Line[cds->type]);
                    cds = cds->next;
                }
        }
    fclose(trace_file);
}
