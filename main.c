/* 
 * File:   main.c
 * Author: Alexander Schurman (alexander.schurman@yale.edu)
 *
 * Created on September 18, 2012
 * 
 * Contains main function, which parses the command line arguments and calls
 * the appropriate function from lzw.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lzw.h"

// the returns codes from main
typedef enum
{
    SUCCESS = 0,
    INVALID_ARGS, // encode or decode was passed invalid args
    FAILED_DECODE // decode failed because stdin isn't a valid encoded file
} RETURN_CODE;

#define INVALID (-1)

typedef enum
{
    ENCODE,
    DECODE
} MODE;

// enumerates the flag types that can be passed to encode
typedef enum
{
    M, // -m flag
    P, // -p flag
    E, // -e flag
} FLAG;

/* Called when lzw is passed an invalid set of arguments. Prints a message to
 * stderr */
void argsError()
{
    fprintf(stderr, "Invalid Arguments: encode [-m MAXBITS] [-p WINDOW] [-e]"
                    " or decode with no arguments\n");
}

/* Identifies the given arg as "encode" or "decode". Returns INVALID if the
 * arg is neither "encode" nor "decode" */
MODE encodeOrDecode(char* arg)
{
    char* lastSlash = strrchr(arg, '/');
    if(lastSlash)
    {
        arg = lastSlash + 1;
    }
    
    if(strcmp(arg, "encode") == 0)
    {
        return ENCODE;
    }
    else if(strcmp(arg, "decode") == 0)
    {
        return DECODE;
    }
    else
    {
        return INVALID;
    }
}

/* Checks an encode argument, arg and returns what type it is. Returns INVALID
 * if the flag is not one of the acceptable types. */
FLAG checkFlag(char* arg)
{
    if(strcmp(arg, "-m") == 0)
    {
        return M;
    }
    else if(strcmp(arg, "-p") == 0)
    {
        return P;
    }
    else if(strcmp(arg, "-e") == 0)
    {
        return E;
    }
    else
    {
        return INVALID;
    }
}

/* Checks the validity of an argument following -m or -p and converts it to a
 * long. Returns the converted long or INVALID if arg is invalid. */
long checkNumArg(char* arg)
{
    char* charAfterNum;
    long num = strtol(arg, &charAfterNum, 10);
    
    if(*charAfterNum == '\0')
    {
        return num;
    }
    else
    {
        return INVALID;
    }
}

/* Processes the command line arguments and calls the appropriate function from
 * lzw.h */
int main(int argc, char** argv)
{
    MODE mode; /* indicates whether lzw is called as encode or decode */
    
    // check argv[0] to see if it's encode or decode
    if(argc == 0 || (mode = encodeOrDecode(argv[0])) == INVALID)
    {
        argsError();
        return INVALID_ARGS;
    }
    else if(mode == DECODE)
    {
        if(argc > 1) // decode cannot have additional args
        {
            argsError();
            return INVALID_ARGS;
        }
        else
        {
            if(!decode())
            {
                fprintf(stderr, "Error on decode; invalid encoded stream\n");
                return FAILED_DECODE;
            }
        }
    }
    else // mode == ENCODE
    {
        long maxBits = 0; // value of -m argument, or 0 if there's no -m
        long window = 0; // value of -p argument, or 0 if there's no -p
        bool eFlag = false; // true if -e flag has been seen
        
        // iterate over args
        for(unsigned int i = 1; i < argc; i++)
        {
            FLAG argType = checkFlag(argv[i]);
            
            switch(argType)
            {
                case M:
                    i++;
                    if(i >= argc || // there is no following number arg
                       (maxBits = checkNumArg(argv[i])) <= 0)
                    {
                        argsError();
                        return 1;
                    }
                    else
                    {
                        // maxBits is a positive int; now set to correct value
                        // if out of range
                        if(maxBits <= 8 || maxBits > 24)
                        {
                            maxBits = 12;
                        }
                    }
                    break;
                    
                case P:
                    i++;
                    if(i >= argc || // there is no following number arg
                       (window = checkNumArg(argv[i])) <= 0)
                    {
                        argsError();
                        return 1;
                    }
                    break;
                    
                case E:
                    eFlag = true;
                    break;
                    
                default:
                    argsError();
                    return 1;
            }
        }
        
        if(!maxBits) // if maxBits wasn't set, default to 12
        {
            maxBits = 12;
        }
        
        encode(maxBits, window, eFlag);
    }

    return SUCCESS;
}

