# LZW

An implementation in C of the Lempel-Ziv-Welch compression algorithm for Yale
University's CS 323 course. See [Wikipedia](http://en.wikipedia.org/wiki/Lempel–Ziv–Welch)
for an explanation of the LZW compression algorithm.

See my WinLZW repo for a graphical Windows version of LZW created with C# and WPF.

Note that the files code.h and code.c, which handle the packing of bits into
bytes, were written by Professor Stan Eisenstat at Yale University.

## Compiling

To compile LZW, simply invoke the make utility with no arguments. See the
Makefile for more details. Important to note is that this project adheres to the
C99 standard and may not compile under other C standards.

## Running

LZW is invoked as either

`encode [-m MAXBITS] [-p WINDOW] [-e]`

or

`decode`

`encode` compresses the standard input and writes a compressed bit stream to
the standard output. The optional `-m`, `-p`, and `-e` flags are described in
the following section. `decode`, which takes no arguments, decompresses the
standard input and writes it to the standard output.

### Encoding Options

The optional arguments for `encode` include `-m MAXBITS`, `-p WINDOW`, and `-e`
where MAXBITS is a positive integer in the range [8, 24] and WINDOW is a
positive integer less than `LONG_MAX`.

#### Maximum Code Length

The maximum length in bits of codes in the string table can be specified to
`encode` with the `-m MAXBITS` argument. As stated above, MAXBITS must be in
the range [8, 24]. If the `-m` flag is not specified, `encode` defaults to a
maximum code length of 12 bits. Codes are no longer added to the table when it
contains 2^MAXBITS codes, unless the `-p` flag is set.

#### Pruning

The `-p WINDOW` argument indicates that the string table should be pruned after
the string table is full. When pruning, a new string table containing the
single-character strings is created (unless `-e` is specified; see below) and to
it is added the last WINDOW codes written by `encode`.

#### Single Character Escaping

Normally the string table is initialized with the single-character strings. If
the `-e` flag is specified, however, the string table is not initialized with
any codes. Instead, whenever a single-character string is seen for the first
time, `encode` outputs an escape character followed by the 8-bit representation
of the single character. This character is then added to the string table.