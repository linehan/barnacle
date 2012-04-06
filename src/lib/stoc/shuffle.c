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
        
        /*i = nitems;*/

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

