/* 
 * File:   lzw.h
 * Author: Alexander Schurman
 *
 * Created on September 20, 2012
 * 
 * Provides functions to encode and decode stdin with the LZW compression
 * algorithm
 */

#include <stdbool.h>

#ifndef LZW_H
#define LZW_H

/* encodes stdin into stdout.
 * maxBits is the maximum number of bits allowed per code. It must be in the
 *     range [8, 24]
 * window is the window size for pruning. If zero, no pruning will happen.
 * eFlag indicates if encode was passed the -e argument. */
void encode(unsigned int maxBits, unsigned int window, bool eFlag);

/* decodes stdin into stdout. Returns true if successful, false if stdin is an
 * invalid encoded stream */
bool decode();

#endif
