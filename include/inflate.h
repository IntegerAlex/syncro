#ifndef INFLATE_H
#define INFLATE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

// This function will inflate the file at the path and return the inflated data
int zlib_inflate(FILE *source,FILE *dest);

#endif
