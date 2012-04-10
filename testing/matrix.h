#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdint.h>

struct matrix_t {
        int   nrows;
        int   ncols;
        int   cur_row;
        int   cur_col;
        double  *ar;               // stores the 2D matrix in linear memory
        double **mx;               // accesses 'ar' with pointers to each row
        void  (*die)(void *self);  // destructor
};


struct matrix_t *new_matrix(int nrows, int ncols);

#define rows_in_matrix(mymx) mymx->nrows 
#define cols_in_matrix(mymx) mymx->ncols

#define matrix_row(mymx) mymx->cur_row
#define matrix_col(mymx) mymx->cur_col

#define matrix_element(m, y, x) \
        m->mx[y][x]

#define matrix_current_element(mymx) \
        mymx->mx[(matrix_row(mymx))][(matrix_col(mymx))]


#define matrix_for_each_row(mymx)  \
        for (matrix_row(mymx)=0; \
             matrix_row(mymx)<rows_in_matrix(mymx); \
             matrix_row(mymx)++)


#define matrix_for_each_col(mymx)  \
        for ((matrix_col(mymx))=0; \
             (matrix_col(mymx))<(cols_in_matrix(mymx)); \
             (matrix_current_element(mymx))=((matrix_row(mymx))+(matrix_col(mymx))), (matrix_col(mymx))++)

/* 
 * e.g.
 *      matrix_for_each(i, j, mymatrix) {{
 *              mymatrix[i][j] = foo;
 *      }}
 */
#define matrix_for_each(mymx)     \
        matrix_for_each_row(mymx) {  \
        matrix_for_each_col(mymx)








#endif
