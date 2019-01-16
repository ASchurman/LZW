/* 
 * File:   stack.c
 * Author: Alexander Schurman (alexander.schurman@yale.edu)
 *
 * Created on September 22, 2012
 * 
 * Implementation for a stack of characters
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

#define INIT_STACK_SIZE (20)
#define STACK_GROWTH_FACTOR (2)

stack* stackNew()
{
    stack* st = malloc(sizeof(stack));
    st->dataLen = 0;
    st->dataSize = INIT_STACK_SIZE;
    st->data = malloc(sizeof(unsigned char) * INIT_STACK_SIZE);
    
    return st;
}

void stackDelete(stack* st)
{
    free(st->data);
    free(st);
}

void stackPush(stack* st, unsigned char c)
{
    // if stack is full, grow it
    if(st->dataLen == st->dataSize)
    {
        st->dataSize *= STACK_GROWTH_FACTOR;
        st->data = realloc(st->data, sizeof(unsigned char) * st->dataSize);
    }
    
    st->dataLen++;
    
    st->data[st->dataLen - 1] = c;
}

bool stackPeek(stack* st, unsigned char* output)
{
    if(st->dataLen == 0)
    {
        output = NULL;
        return false;
    }
    else
    {
        *output = st->data[st->dataLen - 1];
        return true;
    }
}

bool stackPop(stack* st, unsigned char* output)
{
    if(stackPeek(st, output))
    {
        st->dataLen--;
        return true;
    }
    else
    {
        return false;
    }
}