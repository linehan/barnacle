#pragma once
#ifndef __BITS_H
#define __BITS_H


#include <stdint.h>


#define WORDBITS 32 /* The number of bits in a byte */



typedef struct bitmap_t {
        int wordbits;
        int rows;
        int cols;
        uint32_t  *bs;            /* a bitstring */
        uint32_t **bm;            /* a bit matrix (accesses 'bs') */
        void  (*die)(void *self); /* destructor */
} BITMAP;

/* Accessors */
#define BM_bm(a) ((a)->bm)
#define BM_bs(a) ((a)->bs)
#define BM_rows(a) ((a)->rows)
#define BM_cols(a) ((a)->cols)

BITMAP *new_bitmap(int nrows, int ncols);




/* Primitive operations */
#define BITMASK(b)      (1 << ((b) % WORDBITS))
#define BITSEG(b)       ((b) / WORDBITS)
#define BITNSEGS(nb)    ((nb + WORDBITS - 1) / WORDBITS)

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

