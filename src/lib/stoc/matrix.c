/* Program 9.3 from PTRTUT10.HTM   6/13/97 */

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"






/*
 * Method to free a struct matrix_t and all the data in its members.
 */
void dismember_matrix(void *self)
{
        struct matrix_t *condemned = (struct matrix_t *)self;

        free(condemned->mx);
        free(condemned->ar); 
        free(condemned);        /* bon voyage! */
}




/*
 * Return an allocated struct matrix_t large enough for an nrows x ncols
 * matrix.
 */
struct matrix_t *new_matrix(int nrows, int ncols)
{
        struct matrix_t *new;
        int row, col;
        int k;

        /* Allocate the structure */
        if (new = malloc(sizeof(struct matrix_t)), new == NULL)
        {
                puts("\nFailure to allocate room for the struct matrix_t");
                exit(0);
        }
        /* Allocate the 1D array to contain all values in contiguous memory */ 
        if (new->ar = malloc(nrows * ncols * sizeof(double)), new == NULL)
        {
                puts("\nFailure to allocate memory for the matrix array");
                exit(0);
        }
        /* Allocate the array of row pointers, one for each row of the matrix */
        if (new->mx= malloc(nrows * sizeof(double *)), new->mx == NULL)
        {
                puts("\nFailure to allocate memory for matrix pointers.");
                exit(0);
        }
         Calculate the offset of each "row" in the 1D array, and assign
         the address of the first item in this row to the corresponding
         row pointer. We can now access the matrix as rptr[row][col].
        for (k=0; k<nrows; k++)
        {
                new->mx[k] = new->ar + (k * ncols);
        }

        new->nrows = nrows;
        new->ncols = ncols;
        new->die   = &dismember_matrix;

        return (new);
}




