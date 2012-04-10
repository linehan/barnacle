
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

/*
 * Method to free a BITMAP and all the data in its members.
 */
void dismember_bitmap(void *self)
{
        BITMAP *condemned = (BITMAP *)self;

        free(condemned->bm);
        free(condemned->bs); 
        free(condemned);        /* bon voyage! */
}


/*
 * Return an allocated BITMAP large enough for an nrows x ncols
 */
BITMAP *new_bitmap(int nrows, int ncols)
{
        int k;

        BITMAP *new = malloc(sizeof(BITMAP));

        new->rows = nrows;
        new->cols = ncols;
        new->die  = &dismember_bitmap;

        /* Allocate the 1D array to contain all values in contiguous memory */
        new->bs = calloc(nrows * BITNSEGS(ncols), sizeof(uint32_t));

        /* Allocate the row pointers */
        new->bm = calloc(nrows, sizeof(uint32_t *));

        for (k=0; k<nrows; k++)
                new->bm[k] = new->bs + (k * BITNSEGS(ncols));

        return (new);
}


