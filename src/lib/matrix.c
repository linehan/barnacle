/* 
 * matrix.c -- Bloated matrix objects 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
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

        new = calloc(1, sizeof(struct matrix_t));

        new->mx = malloc(nrows * sizeof(uint32_t *));

        for (k=0; k<nrows; k++)
                new->mx[k] = calloc(ncols, sizeof(uint32_t));

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




