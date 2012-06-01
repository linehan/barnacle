/* 
 * mersenne.c -- "Mersenne twister" pseudorandom number generator
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

/******************************************************************************
 * Implements a pseudorandom number generator using the Mersenne twister 
 * algorithm. 
 *
 * Once initialized, the function mt_random() will return a random unsigned
 * 32-bit integer, which may take any of the values in the interval [0,2^32) 
 *
 * Profiling has indicated this algorithm to be about twice as fast as the 
 * library rand() function. 
 *
 * The original code on which this implementation is based was written by 
 * Michael Brundage and has been placed in the public domain. 
 ******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MT_LEN 624
#define MT_MAX 2147483648 /* Maximum value of mt output */

#define MT_IA           397
#define MT_IB           (MT_LEN - MT_IA)
#define UPPER_MASK      0x80000000
#define LOWER_MASK      0x7FFFFFFF
#define MATRIX_A        0x9908B0DF
#define TWIST(b,i,j)    ((b)[i] & UPPER_MASK) | ((b)[j] & LOWER_MASK)
#define MAGIC(s)        (((s)&1)*MATRIX_A)


unsigned long mt[MT_LEN]; /* stores the state of the generator */
int mt_index;   



/*
 * Seed the Mersenne twister with MT_LEN values from the library rand() 
 * function
 */
void init_mersenne(void) 
{
        int i;
        for (i=0; i<MT_LEN; i++) {
                mt[i] = rand(); /* feed me */
        }
        mt_index = 0;
}




/*
 * Return a pseudorandom unsigned long 
 */
unsigned long mt_random(void) 
{
        unsigned long *b;
        unsigned long s;
        int idx;
        int i;

        b   = mt;
        idx = mt_index;

        if (idx == (MT_LEN * sizeof(unsigned long)))
        {
                idx = 0;
                i   = 0;

                for (; i<MT_IB; i++) 
                {
                        s = TWIST(b, i, i+1);
                        b[i] = b[i + MT_IA] ^ (s >> 1) ^ MAGIC(s);
                }
                for (; i<MT_LEN-1; i++) 
                {
                        s = TWIST(b, i, i+1);
                        b[i] = b[i - MT_IB] ^ (s >> 1) ^ MAGIC(s);
                }
                s = TWIST(b, MT_LEN-1, 0);
                b[MT_LEN-1] = b[MT_IA-1] ^ (s >> 1) ^ MAGIC(s);
        }
        mt_index = idx + sizeof(unsigned long);

        return *(unsigned long *)((unsigned char *)b + idx);
}





/*
 * mt_check - a wrapper around requests for random numbers, to ensure that
 * the Mersenne twister is always primed.
 */
static int MT_PRIMED=0;

void mt_check(void)
{
        if (MT_PRIMED-- <= 0) {
                srand(time(NULL));/* Seed rand() (which seeds the twister) */
                init_mersenne();  /* Start the Mersenne twister */
                MT_PRIMED = 624;
        }
}




/* 
 * Transform the return value of mt_random() to the range [0, 1]. 
 */
double mt_normalize(void)
{
        mt_check();
        return (double)((double)mt_random()/(double)MT_MAX);
}


