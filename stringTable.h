/* 
 * File:   stringTable.h
 * Author: Alexander Schurman
 *
 * Created on September 19, 2012
 * 
 * Interface for the string table for LZW encoding and decoding.
 * The string table is searchable by either prefix-char pairs (prefix being
 * the code for the prefix) or codes.
 */

#include <stdbool.h>

#ifndef STRINGTABLE_H
#define STRINGTABLE_H

// enum for special codes
enum
{
    ESCAPE_CODE, // for -e; the next 8 bits represent an ASCII char
    GROW_NBITS_CODE, // increments the number of bits per code
    PRUNE_CODE, // indicates that the string table has been pruned
    STOP_CODE, // indicates that the encoded file has ended
    NUM_SPECIAL_CODES // the number of special codes in this enum
};

#define EMPTY_PREFIX (0)

/*******************************************************************************
 ***************************** Struct Definitions ******************************
 ******************************************************************************/

// an entry in the string table
typedef struct
{
    unsigned int prefix; // code for the prefix to this string table element
    unsigned char k; // character appended to the prefix string
    unsigned int code; // the code for this string table element
} tableElt;

typedef struct
{
    tableElt* array; // array indexed by tableElt codes for O(1) access by code
    tableElt** hash; // a hash table that holds pointers to elements of array;
                     // allows near O(1) access by prefix-char pairs
    
    unsigned int arraySize; // the malloc'd size of array; also the max number
                            // of tableElts that can be stored
    unsigned int hashSize; // the malloc'd size of hash
    
    unsigned int highestCode; // the current number of tableElts stored
    
    bool eFlag; // true if -e was passed to encode
} stringTable;

/* Used for pruning. Contains when each code was last seen; lastSeen[n] is equal
 * to the counter value when code n was last output by encode or input by
 * decode */
typedef struct
{
    unsigned long* lastSeen;
    unsigned long counter;
} pruneInfo;


/*******************************************************************************
 ***************************** stringTable Functions ***************************
 ******************************************************************************/

/* returns a malloc'd stringTable. maxBits is the -m arg */
stringTable* stringTableNew(unsigned int maxBits, bool eFlag);

// frees the malloc'd stringTable
void stringTableDelete(stringTable* table);

/* Adds an entry to the string table. Returns true if successful.
 * prefix and c are the prefix code and appended character of the entry.
 * code is a pointer to an unsigned int into which stringTableAdd puts the code
 * for the new table entry (or 0 if unsuccessful). NULL can be passed as code
 * safely if you don't care about the code. */
bool stringTableAdd(stringTable* table,
                    unsigned int prefix,
                    unsigned char appendChar,
                    unsigned int* code);

/* Finds a string table entry by prefix-code and appended char. Returns a ptr
 * to the table entry (so DON'T CHANGE IT), or NULL if the entry can't be
 * found */
tableElt* stringTableHashSearch(stringTable* table,
                                unsigned int prefix,
                                unsigned char appendChar);

/* Finds a string table entry by code. Returns a ptr to the table entry (so
 * DON'T CHANGE IT), or NULL if the entry can't be found */
tableElt* stringTableCodeSearch(stringTable* table, unsigned int code);

// returns true if table is full and ready to be pruned
bool stringTableIsFull(stringTable* table);

/* prunes the table by deleting the old table and returning a new one. Modifies
 * codeToUpdate from the old table to the new table (if passed 50, and 50
 * becomes 3 in the pruned table, writes 3 to codeToUpdate).
 * Also updates the codes in the pruneInfo. */
stringTable* stringTablePrune(stringTable* table,
                              pruneInfo* pi,
                              unsigned long window,
                              unsigned int* codeToUpdate);


/*******************************************************************************
 ***************************** pruneInfo Functions *****************************
 ******************************************************************************/

// malloc's a new pruneInfo with size based on maxBits
pruneInfo* pruneInfoNew(unsigned int maxBits);

// frees the pruneInfo pi
void pruneInfoDelete(pruneInfo* pi);

/* sets pi's lastSeen value for code to the current counter, then increments
 * the counter */
void pruneInfoSawCode(pruneInfo* pi, unsigned int code);

#endif
