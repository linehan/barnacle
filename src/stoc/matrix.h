#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdint.h>

struct matrix_t {
        int   nrows;
        int   ncols;
        double  *ar;               // stores the 2D matrix in linear memory
        double **mx;               // accesses 'ar' with pointers to each row
        void  (*die)(void *self);  // destructor
};


struct matrix_t *new_matrix(int nrows, int ncols);


/* 
 * e.g.
 *      matrix_for_each(i, j, mymatrix) {{
 *              mymatrix[i][j] = foo;
 *      }}
 */
#define matrix_for_each(r, c, mymx)                             \
        for (r=0; r<mymx->nrows; r++)                           \
        for (c=0; c<mymx->ncols; mymx->mx[r][c]=(r + c), c++)

#endif
