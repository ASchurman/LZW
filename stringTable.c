/* 
 * File:   stringTable.c
 * Author: Alexander Schurman (alexander.schurman@yale.edu)
 *
 * Created on September 19, 2012
 * 
 * Provides implementation for stringTable as defined in stringTable.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stringTable.h"

/*******************************************************************************
********************************* Misc. Functions ******************************
*******************************************************************************/

// the hash function for stringTable->hash
unsigned int hashFunc(unsigned int prefix,
                      unsigned char appendChar,
                      unsigned int hashtableSize)
{
    return (prefix << 8 | appendChar) % hashtableSize;
}

/* returns true if the tableElt's prefix and c fields match prefix and
 * appendChar */
bool tableEltMatch(unsigned int prefix, unsigned int appendChar, tableElt* elt)
{
    return elt->prefix == prefix && elt->k == appendChar;
}

bool stringTableIsFull(stringTable* table)
{
    return table->highestCode == table->arraySize - 1;
}

bool stringTableAdd(stringTable* table,
                    unsigned int prefix,
                    unsigned char appendChar,
                    unsigned int* code)
{
    // check to see if the table already contains this prefix-code pair
    tableElt* elt;
    if((elt = stringTableHashSearch(table, prefix, appendChar)) != NULL)
    {
        if(code) *code = elt->code;
        return false;
    }
    
    if(stringTableIsFull(table))
    {
        if(code) *code = 0;
        return false;   
    }
    
    table->highestCode++;
    
    // the arrayIndex is also the code for the new entry, since table->array
    // is indexed by tableElt code
    unsigned int arrayIndex = table->highestCode;
    
    unsigned int hashIndex = hashFunc(prefix, appendChar, table->hashSize);
    
    table->array[arrayIndex].prefix = prefix;
    table->array[arrayIndex].k = appendChar;
    table->array[arrayIndex].code = arrayIndex;
    
    // find the first empty entry in the hash table
    while(table->hash[hashIndex] != NULL)
    {
        hashIndex = (hashIndex + 1) % table->hashSize;
    }
    
    table->hash[hashIndex] = &(table->array[arrayIndex]);
    
    if(code) *code = arrayIndex;
    return true;
}


/*******************************************************************************
************************ stringTable Creation/Deletion *************************
*******************************************************************************/

// fills the table with the single-char strings if table->eFlag is false
void stringTableInit(stringTable* table)
{
    if(table->eFlag == false)
    {
        for(unsigned int i = 0; i <= 255; i++)
        {
            stringTableAdd(table, EMPTY_PREFIX, i, NULL);
        }   
    }
}

// creates new table based on the number of possible codes and the values from
// the -p and -e args; window is 0 if no -p was passed
stringTable* createTable(unsigned int numCodes,
                         bool eFlag)
{
    stringTable* table = malloc(sizeof(stringTable));
    
    table->highestCode = NUM_SPECIAL_CODES - 1;
    
    table->eFlag = eFlag;
    
    table->arraySize = numCodes;
    table->array = malloc(sizeof(tableElt) * table->arraySize);
    
    table->hashSize = (table->arraySize * 2) + 1;
    table->hash = malloc(sizeof(tableElt*) * table->hashSize);
    
    // initialize table->hash to NULL pointers so we know which hash entries
    // are occupied
    for(unsigned int i = 0; i < table->hashSize; i++)
    {
        table->hash[i] = NULL;
    }
    
    stringTableInit(table);
    
    return table;
}

stringTable* stringTableNew(unsigned int maxBits,
                            bool eFlag)
{
    return createTable((1 << maxBits), eFlag);
}

void stringTableDelete(stringTable* table)
{
    free(table->array);
    free(table->hash);
    free(table);
}


/*******************************************************************************
****************************** Searching ***************************************
*******************************************************************************/

tableElt* stringTableHashSearch(stringTable* table,
                                unsigned int prefix,
                                unsigned char appendChar)
{
    unsigned int hashIndex = hashFunc(prefix, appendChar, table->hashSize);
    
    // increment hashIndex (mod hashSize) until we reach NULL or the desired
    // entry
    while(table->hash[hashIndex] != NULL &&
          !tableEltMatch(prefix, appendChar, table->hash[hashIndex]))
    {
        hashIndex = (hashIndex + 1) % table->hashSize;
    }
    
    if(table->hash[hashIndex] == NULL)
    {
        return NULL;
    }
    else
    {
        return table->hash[hashIndex];
    }
}

tableElt* stringTableCodeSearch(stringTable* table, unsigned int code)
{
    if(code > table->highestCode || code < NUM_SPECIAL_CODES)
    {
        return NULL;
    }
    else
    {
        return &(table->array[code]);
    }
}


/*******************************************************************************
******************************** Pruning ***************************************
*******************************************************************************/

// adds a tableElt and all it's prefixes from an old stringTable to a new
// stringTable. Returns the code of eltToAdd in the new table
unsigned int recursiveAdd(stringTable* newTable,
                          stringTable* oldTable,
                          const tableElt* eltToAdd,
                          pruneInfo* oldPi,
                          pruneInfo* newPi)
{    
    unsigned int newPrefix = EMPTY_PREFIX;
    unsigned int oldPrefix = eltToAdd->prefix;
    
    if(oldPrefix != EMPTY_PREFIX)
    {
        newPrefix = recursiveAdd(newTable,
                                 oldTable,
                                 stringTableCodeSearch(oldTable, oldPrefix),
                                 oldPi,
                                 newPi);
    }
    
    unsigned int newCode;
    stringTableAdd(newTable, newPrefix, eltToAdd->k, &newCode);
    
    // update newPi
    newPi->lastSeen[newCode] = oldPi->lastSeen[eltToAdd->code];
    
    return newCode;
}

stringTable* stringTablePrune(stringTable* table,
                              pruneInfo* pi,
                              unsigned long window,
                              unsigned int* codeToUpdate)
{
    // copy pi into oldPi
    pruneInfo* oldPi = malloc(sizeof(pruneInfo));
    oldPi->counter = pi->counter;
    oldPi->lastSeen = malloc(sizeof(unsigned long) * table->arraySize);
    for(unsigned int i = 0; i < table->arraySize; i++)
    {
        oldPi->lastSeen[i] = pi->lastSeen[i];
    }
    
    // reinitialize pi->lastSeen to zero
    memset(pi->lastSeen, 0, sizeof(unsigned long) * table->arraySize);
    
    stringTable* newTable = createTable(table->arraySize,
                                        table->eFlag);
    
    for(unsigned int i = NUM_SPECIAL_CODES; i <= table->highestCode; i++)
    {
        tableElt* oldElt = &(table->array[i]);
        
        if(oldPi->lastSeen[i] > oldPi->counter - window)
        {
            unsigned int newCode = recursiveAdd(newTable,
                                                table,
                                                oldElt,
                                                oldPi,
                                                pi);
            
            if(oldElt->code == *codeToUpdate)
            {
                *codeToUpdate = newCode;
            }
        }
    }
    
    stringTableDelete(table);
    pruneInfoDelete(oldPi);
    
    return newTable;
}

/*******************************************************************************
******************************** pruneInfo *************************************
*******************************************************************************/

// malloc's a new pruneInfo with size based on maxBits
pruneInfo* pruneInfoNew(unsigned int maxBits)
{
    pruneInfo* pi = malloc(sizeof(pruneInfo));
    pi->lastSeen = malloc(sizeof(unsigned long) * (1 << maxBits));
    pi->counter = 1;
    
    memset(pi->lastSeen, 0, sizeof(unsigned long) * (1 << maxBits));
    
    return pi;
}

// frees the pruneInfo pi
void pruneInfoDelete(pruneInfo* pi)
{
    free(pi->lastSeen);
    free(pi);
}

/* sets pi's lastSeen value for code to the current counter, then increments
 * the counter */
void pruneInfoSawCode(pruneInfo* pi, unsigned int code)
{
    pi->lastSeen[code] = pi->counter;
    (pi->counter)++;
}