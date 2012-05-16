#include <stdlib.h>
#include "matrix.h"


void delete_matrix(void *self)
{
        struct matrix_t *todel = (struct matrix_t *)self;
        /* 
         * We don't have to loop in order to free everything, 
         * because 'mx' is an array of pointers to the same 
         * memory laid out in 'ar'
         */
        free(todel->mx);
        free(todel->ar); 
        free(todel);     /* todel-oo! */
}


/*
 * Return an allocated struct matrix_t large enough for an nrows x ncols
 * matrix.
 */
struct matrix_t *new_matrix(int nrows, int ncols)
{
        struct matrix_t *new;
        int k;

        /* Allocate the structure */
        if (new = calloc(1, sizeof(struct matrix_t)), new == NULL) {
                puts("\nFailure to allocate room for the struct matrix_t");
                exit(0);
        }

        /* Allocate the 1D array to contain all values in contiguous memory */
        if (new->ar = calloc(nrows * ncols, sizeof(uint32_t)), new == NULL) {
                puts("\nFailure to allocate memory for the matrix array");
                exit(0);
        }
        /* Allocate the array of row pointers, one for each row of the matrix */
        if (new->mx = calloc(nrows, sizeof(uint32_t *)), new->mx == NULL) {
                puts("\nFailure to allocate memory for matrix pointers.");
                exit(0);
        }
        /*
         * Calculate the offset of each "row" in the 1D array, and assign
         * the address of the first item in this row to the corresponding
         * row pointer. We can now access the matrix as rptr[row][col].
         */
        for (k=0; k<nrows; k++)
                new->mx[k] = new->ar + (k * ncols);

        /* Assign helper values to members in the structure */
        new->itr.rows = nrows;
        new->itr.cols = ncols;
        new->itr.len = (nrows * ncols);
        new->itr.row = 0;
        new->itr.col = 0;
        new->itr.adr = 0;
        new->die = &delete_matrix;

        return (new);
}




