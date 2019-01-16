/* 
 * File:   stack.h
 * Author: Alexander Schurman
 *
 * Created on September 22, 2012
 * 
 * Interface for a stack of characters
 */

#include <stdbool.h>

#ifndef STACK_H
#define STACK_H

typedef struct
{
    unsigned char* data; // the data held in the stack
    unsigned int dataSize; // malloc'd size of data
    unsigned int dataLen; // the number of elements in data
} stack;

// returns a malloc'd stack
stack* stackNew();

// frees a malloc'd stack
void stackDelete(stack* st);

// pushes c onto st
void stackPush(stack* st, unsigned char c);

/* pops a value off of st and writes its value to output. Returns true if
 * successful, false if the stack was empty */
bool stackPop(stack* st, unsigned char* output);

/* peeks at the top value of st, placing the value in output, without popping
 * it off. Returns true if successful, false if st was empty. */
bool stackPeek(stack* st, unsigned char* output);

#endif
