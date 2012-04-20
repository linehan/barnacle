#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H
#include <stdlib.h>
#include "../com/arawak.h"

#define NUM_MOORE_NEIGHBORS 8
#define NUM_VONEU_NEIGHBORS 4

/* Abstract data types
``````````````````````````````````````````````````````````````````````````````*/
struct matrix_t {
        uint32_t rows;  /* Number of rows */
        uint32_t cols;  /* Number of columns */
        uint32_t len;   /* Length of the ar array */
        uint32_t  *ar;  /* Linear array of values */
        uint32_t **mx;  /* The matrix, an array of row pointers into ar */
        void (*die)(void *self); /* Destructor to free all memory */
};

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

static uint32_t DUMMY;

/* Public function prototypes 
``````````````````````````````````````````````````````````````````````````````*/
struct matrix_t *new_matrix(uint32_t nrows, uint32_t ncols);



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
 * mx_put -- set the word at [y][x] to new value 
 * @matrix: pointer to a struct matrix_t 
 * @y: y coordinate index
 * @x: x coordinate index
 * @val: new value of matrix->mx[y][x]
 */
static inline
void mx_put(struct matrix_t *matrix, int y, int x, uint32_t val)
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
int mx_adr(struct matrix_t *matrix, int y, int x)
{
        return x + (y * matrix->cols);
}

/*
 * mxar_get -- return a pointer to the word at adr
 * @matrix: pointer to a struct matrix_t 
 * @adr: integer address of word
 */
static inline
uint32_t *mxar_get(struct matrix_t *matrix, int adr)
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
#define safe_address_e(mx,x) (x < (mx)->cols - 1) ? 1 : 0
#define safe_address_n(mx,y) (y > 0) ? 1 : 0 
#define safe_address_s(mx,y) (y < (mx)->rows - 1) ? 1 : 0

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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
                return &DUMMY;
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
        seed->n = mx_n(matrix, y, x);
        seed->s = mx_s(matrix, y, x);
        seed->e = mx_e(matrix, y, x);
        seed->w = mx_w(matrix, y, x);
        seed->nw = mx_nw(matrix, y, x);
        seed->ne = mx_ne(matrix, y, x);
        seed->sw = mx_sw(matrix, y, x);
        seed->se = mx_se(matrix, y, x);
}


#endif
