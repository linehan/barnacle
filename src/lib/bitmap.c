#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "bitmap.h"


/*
 * Return an allocated BITMAP large enough for an nrows x ncols
 */
struct matrix_t *new_bitmap(int nrows, int ncols)
{
        struct matrix_t *new;

        new = new_matrix(nrows, BITNSEGS(ncols));

        return (new);
}


