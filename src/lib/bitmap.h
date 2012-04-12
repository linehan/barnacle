#pragma once
#ifndef __BITS_H
#define __BITS_H


#include <stdint.h>
#include "matrix.h"


/*
 * A bitmap is a special case of a struct matrix_t. The matrix is allocated
 * as usual, but contains n rows of BITNSEGS(m) cols each, instead of the
 * literal number of columns. In short, it divides the requested column count
 * into "segments" of some size.
 */


#define SEGSIZE 8 /* The number of bits in a word */


struct matrix_t *new_bitmap(int nrows, int ncols);


#define BM_bm(a) (a)->mx
#define BM_rows(a) (a)->rows
#define BM_cols(a) (a)->cols*SEGSIZE
#define BM_size(a) ((size_t)(BM_rows(a) * BM_cols(a)))

/* Primitive operations */
#define BITMASK(b)      (1 << ((b) % SEGSIZE))
#define BITSEG(b)       ((b) / SEGSIZE)
#define BITNSEGS(nb)    ((nb + SEGSIZE - 1) / SEGSIZE)

/* 1D (bitstring) operations */
#define BITSET(a,b)     ((a)[BITSEG(b)] |=  BITMASK(b))
#define BITCLR(a,b)     ((a)[BITSEG(b)] &= ~BITMASK(b))
#define BITTOG(a,b)     ((a)[BITSEG(b)] ^=  BITMASK(b))
#define BITVAL(a,b)     ((a)[BITSEG(b)] & BITMASK(b))
#define BITTEST(a,b)    (BITVAL(a,b)) ? 1 : 0

/* 2D (bitmap) operations */
#define BITMAPSET(a,by,bx)   (BM_bm(a)[by][BITSEG(bx)] |= BITMASK(bx))
#define BITMAPCLR(a,by,bx)   (BM_bm(a)[by][BITSEG(bx)] &= BITMASK(bx))
#define BITMAPTOG(a,by,bx)   (BM_bm(a)[by][BITSEG(bx)] ^= BITMASK(bx))
#define BITMAPVAL(a,by,bx)   (BM_bm(a)[by][BITSEG(bx)] & BITMASK(bx))
#define BITMAPTEST(a,by,bx)  (BITMAPVAL(a,by,bx)) ? 1 : 0


/* Control structures */
#define bitmap_for_each_row(a,i)       for (i=0; i<(BM_rows(a)); i++)
#define bitmap_for_each_col(a,j)       for (j=0; j<(BM_cols(a)); j++)
/* Nested row and column loop (must be terminated with '}}') */
#define bitmap_for_each(a,i,j)         bitmap_for_each_row(a, i) { \
                                       bitmap_for_each_col(a, j)


#endif

