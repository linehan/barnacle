#pragma once
#ifndef __MATRIX_H
#define __MATRIX_H
#include <stdlib.h>
#include <assert.h>
#include "../com/arawak.h"
#include "../eng/error.h"

#define NUM_MOORE_NEIGHBORS 8
#define NUM_VONEU_NEIGHBORS 4

/* Abstract data types
``````````````````````````````````````````````````````````````````````````````*/
struct seed_t {
        uint32_t *nbr[8];
        uint32_t *cur;
        uint32_t *n;
        uint32_t *s;
        uint32_t *e;
        uint32_t *w;
        uint32_t *nw;
        uint32_t *ne;
        uint32_t *sw;
        uint32_t *se;
        int y;
        int x;
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



static uint32_t DOPE = 0x00000000UL;


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
        return &(matrix->mx[y][x]);
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
uint32_t mxar_val(struct matrix_t *matrix, size_t adr)
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
/* Range-check y coordinates */
static inline bool safe_y(struct matrix_t *matrix, int y)
{
        return ((y >= 0) && (y < matrix->itr.rows)) ? true : false;
}
/* Range-check x coordinates */
static inline bool safe_x(struct matrix_t *matrix, int x)
{
        return ((x >= 0) && (x < matrix->itr.cols)) ? true : false;
}
/* Range-check to the west */
static inline bool safe_w(struct matrix_t *mx, int y, int x)
{
        if ((safe_y(mx, y)) && (safe_x(mx, x)) && (safe_x(mx, (x-1))))
                return true;
        else
                return false;
}
/* Range-check to the east */
static inline bool safe_e(struct matrix_t *mx, int y, int x)
{
        if ((safe_y(mx, y)) && (safe_x(mx, x)) && (safe_x(mx, (x+1))))
                return true;
        else
                return false;
}
/* Range-check to the north */
static inline bool safe_n(struct matrix_t *mx, int y, int x)
{
        if ((safe_y(mx, y)) && (safe_x(mx, x)) && (safe_y(mx, (y-1))))
                return true;
        else
                return false;
}
/* Range-check to the south */
static inline bool safe_s(struct matrix_t *mx, int y, int x)
{
        if ((safe_y(mx, y)) && (safe_x(mx, x)) && (safe_y(mx, (y+1))))
                return true;
        else
                return false;
}
/* Range-check to the north-west */
static inline bool safe_nw(struct matrix_t *mx, int y, int x)
{
        if ((safe_n(mx, y, x)) && (safe_w(mx, y, x)))
                return true;
        else
                return false;
}
/* Range-check to the north-east */
static inline bool safe_ne(struct matrix_t *mx, int y, int x)
{
        if ((safe_n(mx, y, x)) && (safe_e(mx, y, x)))
                return true;
        else
                return false;
}
/* Range-check to the south-west */
static inline bool safe_sw(struct matrix_t *mx, int y, int x)
{
        if ((safe_s(mx, y, x)) && (safe_w(mx, y, x)))
                return true;
        else
                return false;
}
/* Range-check to the south-east */
static inline bool safe_se(struct matrix_t *mx, int y, int x)
{
        if ((safe_s(mx, y, x)) && (safe_e(mx, y, x)))
                return true;
        else
                return false;
}




/*
 * mx_w -- returns ptr to the value stored to the west of address adr
 * @matrix: pointer to the struct matrix_t
 * @y: y coordinate index
 */
static inline
uint32_t *mx_w(struct matrix_t *matrix, int y, int x)
{
        if (safe_w(matrix, y, x)) 
                return &matrix->mx[y][(x-1)];
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
        if (safe_e(matrix, y, x)) 
                return &matrix->mx[y][(x+1)]; 
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
        if (safe_n(matrix, y, x))
                return &matrix->mx[(y-1)][x];
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
        if (safe_s(matrix, y, x))
                return &matrix->mx[(y+1)][x];
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
        if (safe_nw(matrix, y, x))
                return &matrix->mx[(y-1)][(x-1)];
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
        if (safe_ne(matrix, y, x))
                return &matrix->mx[(y-1)][(x+1)]; 
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
        if (safe_sw(matrix, y, x))
                return &matrix->mx[(y+1)][(x-1)];
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
        if (safe_se(matrix, y, x))
                return &matrix->mx[(y+1)][(x+1)];
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
        if (!(safe_y(matrix, y) && safe_x(matrix, x)))
                abort_report("Bad indices %d,%d in %d rows and %d cols",
                             y, x, matrix->itr.rows, matrix->itr.cols);

        seed->cur = mx_get(matrix, y, x);
        seed->nbr[0] = seed->n  = mx_n(matrix, y, x);
        seed->nbr[1] = seed->s  = mx_s(matrix, y, x);
        seed->nbr[2] = seed->e  = mx_e(matrix, y, x);
        seed->nbr[3] = seed->w  = mx_w(matrix, y, x);
        seed->nbr[4] = seed->nw = mx_nw(matrix, y, x);
        seed->nbr[5] = seed->ne = mx_ne(matrix, y, x);
        seed->nbr[6] = seed->sw = mx_sw(matrix, y, x);
        seed->nbr[7] = seed->se = mx_se(matrix, y, x);
        seed->y = y;
        seed->x = x;
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
        return mx_get(mx, mx->itr.row, mx->itr.col);
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
        if (mx_row(mx) >= (mx->itr.rows - 1)
        && (mx_col(mx) >= (mx->itr.cols - 1)))
                return false;
        else
                return true;
}

/**
 * mx_itr_next -- advances the len, col, and row indices of the foreach loop
 */
static inline void mx_itr_next(struct matrix_t *mx)
{
        mx->itr.col = (mx->itr.col+1) % mx->itr.cols;

        if (mx->itr.col == 0)
                mx->itr.row++;

        assert(mx->itr.col < mx->itr.cols || "Matrix column overflow");
        assert(mx->itr.row < mx->itr.rows || "Matrix row overflow");
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

#endif
