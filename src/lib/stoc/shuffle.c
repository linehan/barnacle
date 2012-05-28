/* 
 * shuffle.c -- helper functions for the Perlin generators
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
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "stoc.h"

#define BYTE(X) ((unsigned char *)(X)) 


/* 
 * This function generates a random permutation of a finite set, using the 
 * "inside-out" variation of the Fisher-Yates shuffle algorithm, which does
 * not require the shuffle be in-place. 
 *
 * The function is written so that it will accomodate any set of items with 
 * integral type; of course, both the source and shuffle buffer must be of 
 * the same type. 
 *
 * @source: pointer to an allocated array of integral type
 * @shuffle: pointer to an allocated array of integral type
 * @nitems: the number of items in the set
 * @size: the size of each item in the set
 *
 */
void shuf(void *source, void *shuffle, size_t nitems, size_t size)
{
        assert(source != NULL && shuffle != NULL);
        size_t i;
        size_t j;
        
        /* i.e. shuffle[0] = source[0] */
        memcpy(shuffle, source, size);

        for (i=1; i<(nitems-1); i++)
        {
                j = roll_fair(i);
                /* i.e. shuffle[i] = shuffle[j] */
                memcpy(BYTE(shuffle)+i*size, BYTE(shuffle)+j*size, size);
                /* i.e. shuffle[j] = source[i] */
                memcpy(BYTE(shuffle)+j*size, BYTE(source)+i*size, size);
        }
}


/* Implement the inside-out version of the Fisher-Yates shuffle algorithm,
 *  * which generates a random permutation of a finite set. Here, we use the
 *   * algorithm to shuffle the permutation table. */
void fy_shuffle(int *source, int *shuffle, int n)
{
        int i, j;
        shuffle[0] = source[0];
        for (i=1; i<(n-1); i++) {
                j = roll_fair(i);
                shuffle[i] = shuffle[j];
                shuffle[j] = source[i];
        }
}

