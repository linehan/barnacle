#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H
#include <stdlib.h>
#include <assert.h>
#include "../com/arawak.h"

#define NUM_MOORE_NEIGHBORS 8
#define NUM_VONEU_NEIGHBORS 4

/* Abstract data types
``````````````````````````````````````````````````````````````````````````````*/
struct seed_t {
        uint32_t *cur;
        uint32_t *n;
        uint32_t *s;
        uint32_t *e;
        uint32_t *w;
        uint32_t *nw;
        uint32_t *ne;
        uint32_t *sw;
        uint32_t *se;
};

struct mx_index {
        int rows;
        int cols;
        int len;

        int row;
        int col;
        int adr;
};


struct matrix_t {
        uint32_t  *ar;  /* Linear array of values */
        uint32_t **mx;  /* The matrix, an array of row pointers into ar */
        struct mx_index itr;
        void (*die)(void *self); /* Destructor to free all memory */
};



static uint32_t DOPE = ~0;


/* Public function prototypes 
``````````````````````````````````````````````````````````````````````````````*/
struct matrix_t *new_matrix(int nrows, int ncols);



/****************************************************************************** 
 * Accessor Functions
 *
 *      get - returns uint32_t *
 *      val - returns uint32_t
 *      put - inserts uint32_t
 *      adr - compute array address
 *
 * There are two ways to access a matrix. The first is the more intuitive,
 * passing two Cartesian parameters y and x to dereference a 2D array, or
 * matrix, in the traditional sense.
 *
 * The other is to use the generalized array formula:
 *
 *      arr[n] = arr + n * (size-of-type-pointed-into)
 *
 * To compute the appropriate address 'yx' for 2 dimensions like this: 
 *
 *      arr[yx] = arr + (x + (y * cols-in-row)) * (size-of-type-pointed-into)
 *
 * Both have their uses. In these functions, all matrix accessor functions
 * begin with 'mx_', and all array accessors begin with 'mxar_'.
 *
 ******************************************************************************/

/*
 * mx_get -- return a pointer to the word at [y][x]
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_get(struct matrix_t *matrix, int y, int x)
{
        return &matrix->mx[y][x];
}

/*
 * mx_val -- return the value of the word at [y][x]
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t mx_val(struct matrix_t *matrix, int y, int x)
{
        return matrix->mx[y][x];
}

/*
 * mx_set -- set the word at [y][x] to new value 
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 * @val: new value of matrix->mx[y][x]
 */
static inline
void mx_set(struct matrix_t *matrix, int y, int x, uint32_t val)
{
        matrix->mx[y][x] = val;
}

/*
 * mx_adr -- return the linear address value for [y][x]
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
int mx_adr_yx(struct matrix_t *matrix, int y, int x)
{
        return x + (y * matrix->itr.cols);
}

/*
 * mxar_get -- return a pointer to the word at adr
 * @matrix: pointer to a struct matrix_t 
 * @adr: integer address of word
 */
static inline
uint32_t *mx_arget(struct matrix_t *matrix, int adr)
{
        return &matrix->ar[adr];
}

/*
 * mxar_val -- return the value of the word at ar[adr]
 * @matrix: pointer to a struct matrix_t 
 * @adr: integer address of value
 */
static inline
uint32_t mxar_val(struct matrix_t *matrix, int adr)
{
        return matrix->ar[adr];
}

/*
 * mxar_put -- set the word at ar[adr] to new value 
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 * @val: new value of matrix->ar[adr]
 */
static inline
void mxar_put(struct matrix_t *matrix, int adr, uint32_t val)
{
        matrix->ar[adr] = val;
}

/*
 * mx_del -- free a matrix
 * @matrix: pointer to a struct matrix_t 
 */
static inline
void mx_del(struct matrix_t *matrix)
{
        matrix->die(matrix);
}


/****************************************************************************** 
 * Connectedness functions
 *
 * For efficiency, these functions perform the minimal bounds checking, and
 * assume that the caller has already determined that the point y,x lies
 * within the bounds of the matrix.
 ******************************************************************************/
#define safe_address_w(mx,x) (x > 0) ? 1 : 0
#define safe_address_e(mx,x) (x < (mx)->itr.cols - 1) ? 1 : 0
#define safe_address_n(mx,y) (y > 0) ? 1 : 0 
#define safe_address_s(mx,y) (y < (mx)->itr.rows - 1) ? 1 : 0

#define safe_address_nw(mx,y,x) safe_address_n(mx, y) && safe_address_w(mx, x)
#define safe_address_ne(mx,y,x) safe_address_n(mx, y) && safe_address_e(mx, x)
#define safe_address_sw(mx,y,x) safe_address_s(mx, y) && safe_address_w(mx, x)
#define safe_address_se(mx,y,x) safe_address_s(mx, y) && safe_address_e(mx, x)


enum neighbors {CUR,_N_,_S_,_E_,_W_,N_W,N_E,S_W,S_E};

/*
 * mx_w -- returns ptr to the value stored to the west of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 */
static inline
uint32_t *mx_w(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_w(matrix, x))) 
                return &matrix->mx[y][x-1];
        else
                return &DOPE;
}

/*
 * mx_e -- returns ptr to the value stored to the east of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_e(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_e(matrix, x))) 
                return &matrix->mx[y][x+1]; 
        else
                return &DOPE;
}

/*
 * mx_n -- returns ptr to the value stored to the north of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_n(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_n(matrix, y))) 
                return &matrix->mx[y-1][x];
        else 
                return &DOPE;
}

/*
 * mx_s -- returns ptr to the value stored to the south of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_s(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_s(matrix, y)))
                return &matrix->mx[y+1][x];
        else 
                return &DOPE;
}

/*
 * mx_nw -- returns ptr to the value stored to the northwest of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_nw(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_nw(matrix, y, x)))
                return &matrix->mx[y-1][x-1];
        else
                return &DOPE;
}

/*
 * mx_ne -- returns ptr to the value stored to the east of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_ne(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_ne(matrix, y, x))) 
                return &matrix->mx[y-1][x+1]; 
        else
                return &DOPE;
}

/*
 * mx_sw -- returns ptr to the value stored to the north of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_sw(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_sw(matrix, y, x))) 
                return &matrix->mx[y+1][x-1];
        else 
                return &DOPE;
}

/*
 * mx_se -- returns ptr to the value stored to the south of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 */
static inline
uint32_t *mx_se(struct matrix_t *matrix, int y, int x)
{
        if (likely(safe_address_se(matrix, y, x))) 
                return &matrix->mx[y+1][x+1];
        else 
                return &DOPE;
}

/*
 * mx_seed -- fill a seed of the moore neighborhood
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 * @x: x coordinate index
 * @seed: pointer to a struct seed_t
 */
static inline
void mx_seed(struct matrix_t *matrix, int y, int x, struct seed_t *seed)
{
        seed->cur = mx_get(matrix, y, x);
        seed->n   = mx_n(matrix, y, x);
        seed->s   = mx_s(matrix, y, x);
        seed->e   = mx_e(matrix, y, x);
        seed->w   = mx_w(matrix, y, x);
        seed->nw  = mx_nw(matrix, y, x);
        seed->ne  = mx_ne(matrix, y, x);
        seed->sw  = mx_sw(matrix, y, x);
        seed->se  = mx_se(matrix, y, x);
}



/* Iteration Accessors
``````````````````````````````````````````````````````````````````````````````*/
/**
 * mx_row -- returns the current row index of the matrix
 */
static inline int mx_row(struct matrix_t *mx)
{
        return (mx->itr.row);
}

/**
 * mx_col -- returns the current column index of the matrix
 */
static inline int mx_col(struct matrix_t *mx)
{
        return (mx->itr.col);
}

/**
 * mx_adr -- returns the current vector address of the matrix 
 */
static inline int mx_adr(struct matrix_t *mx)
{
        return (mx->itr.adr);
}

/**
 * mx_rows -- returns the number of rows in a matrix 
 */
static inline int mx_rows(struct matrix_t *mx)
{
        return (mx->itr.rows);
}

/**
 * mx_cols -- returns the number of rows in a matrix 
 */
static inline int mx_cols(struct matrix_t *mx)
{
        return (mx->itr.cols);
}

/**
 * mx_len -- returns the number of rows in a matrix 
 */
static inline int mx_len(struct matrix_t *mx)
{
        return (mx->itr.len);
}

/**
 * mx_cell -- returns the current cell of the matrix
 */
static inline uint32_t *mx_cell(struct matrix_t *mx)
{
        return mx_arget(mx, mx_adr(mx)); 
}


/* Foreach loops 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * mx_itr_start -- the initial condition of the foreach loop
 */
static inline void mx_itr_start(struct matrix_t *matrix)
{
        matrix->itr.adr = 0;
        matrix->itr.row = 0;
        matrix->itr.col = 0;
}

/**
 * mx_itr_until -- the termination condition of the foreach loop
 */
static inline bool mx_itr_until(struct matrix_t *mx)
{
        return (mx_adr(mx) < mx_len(mx)) ? true : false; 
}

/**
 * mx_itr_next -- advances the len, col, and row indices of the foreach loop
 */
static inline void mx_itr_next(struct matrix_t *matrix)
{
        matrix->itr.adr++;

        matrix->itr.col = matrix->itr.adr % matrix->itr.cols;
        matrix->itr.row = matrix->itr.adr / matrix->itr.cols;

        assert(matrix->itr.col < matrix->itr.cols || "Matrix column overflow");
        assert(matrix->itr.row < matrix->itr.rows || "Matrix row overflow");
}

/**
 * mx_itr_seed -- fill a seed on traversal
 * @matrix: pointer to the struct matrix_t
 */
static inline
void mx_itr_seed(struct seed_t *seed, struct matrix_t *matrix)
{
        mx_seed(matrix, mx_row(matrix), mx_col(matrix), seed);
}


/**
 * mx_foreach -- traverses a matrix
 * @cell: pointer to a uint32_t
 * @mx: pointer to a struct matrix_t
 */
#define mx_foreach(cell, mx)                      \
        for (mx_itr_start(mx);                    \
             mx_itr_until(mx);                    \
             mx_itr_next(mx), cell = mx_cell(mx))

/**
 * mx_foreach_seed -- traverses a matrix and generate seed
 * @mx: pointer to a struct matrix_t
 */
#define mx_foreach_seed(seed, mx)                     \
        for (mx_itr_start(mx), mx_itr_seed(seed,mx);  \
             mx_itr_until(mx);                        \
             mx_itr_next(mx), mx_itr_seed(seed,mx))

/**
 * mx_foreach_yx -- traverses a matrix; provides row and column indices
 * @cell: pointer to a uint32_t
 * @y: row index
 * @x: column index
 * @mx: pointer to a struct matrix_t
 */
#define mx_foreach_yx(cell, y, x, mx)                                       \
        for (mx_itr_start(mx);                                              \
             mx_itr_until(mx);                                              \
             mx_itr_next(mx), cell=mx_cell(mx), y=mx_row(mx), x=mx_col(mx))

/**
 * mx_foreach_seed_yx -- traverses a matrix; provides row and column indices
 * @cell: pointer to a uint32_t
 * @y: row index
 * @x: column index
 * @mx: pointer to a struct matrix_t
 */
#define mx_foreach_seed_yx(seed, y, x, mx)                                     \
        for (mx_itr_start(mx), mx_itr_seed(seed,mx), y=mx_row(mx),x=mx_col(mx);\
             mx_itr_until(mx);                                                 \
             mx_itr_next(mx), mx_itr_seed(seed,mx), y=mx_row(mx),x=mx_col(mx))


#endif
