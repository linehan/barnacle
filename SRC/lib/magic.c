/*


      (\.   \      ,/)        Magical algorithms -- magic.c
       \(   |\     )/
       //\  | \   /\\         Commonly useful, occasionally priceless
      (/ /\_#oo#_/\ \)        algorithms for "bit-twiddling"; not as
       \/\  ####  /\/         indecipherable as they seem at first, and 
            `##'              downright joyful after close study. 
 

*/



#include "magic.h"



/*
 * par32 -- Compute parity of 32-bit int in 8 operations
 * @v: unsigned 32-bit integer value
 * re: 1 if even, 0 if odd
 */
uint32_t par32(uint32_t v)
{
        v ^= v >> 1;
        v ^= v >> 2;
        v = (v & 0x11111111U) * 0x11111111U;

        return (v >> 28) & 1;
} 




/*
 * par64 -- Compute parity of 64-bit int in 8 operations
 * @v: unsigned 64-bit integer value
 * re: 1 if even, 0 if odd
 */
uint64_t par64(uint64_t v)
{
        v ^= v >> 1;
        v ^= v >> 2;
        v = (v & 0x1111111111111111UL) * 0x1111111111111111UL;

        return (v >> 60) & 1;
}




/*
 * ones32 -- Compute the number of set bits (ones) in a 32-bit integer v
 * @v: unsigned 32-bit integer value
 *
 *
 * Henry Gordon Dietz
 * The Aggregate Magic Algorithms
 * University of Kentucky
 * Aggregate.Org online technical report
 * http://aggregate.org/MAGIC/
 *
 * The population count of a binary integer value x is the number of one 
 * bits in the value. Although many machines have single instructions for 
 * this, the single instructions are usually microcoded loops that test a 
 * bit per cycle; a log-time algorithm coded in C is often faster. 
 *
 * The following code uses a variable-precision SWAR algorithm to perform 
 * a tree reduction adding the bits in a 32-bit value:
 *
 * It is worthwhile noting that the SWAR population count algorithm given 
 * above can be improved upon for the case of counting the population of 
 * multi-word bit sets. How? The last few steps in the reduction are using 
 * only a portion of the SWAR width to produce their results; thus, it 
 * would be possible to combine these steps across multiple words being 
 * reduced.
 *
 * One additional note: the AMD Athlon optimization guidelines suggest a 
 * very similar algorithm that replaces the last three lines with 
 *
 *      return((x * 0x01010101) >> 24);
 *
 * For the Athlon (which has a very fast integer multiply), I would have 
 * expected AMD's code to be faster... but it is actually 6% slower 
 * according to my benchmarks using a 1.2GHz Athlon (a Thunderbird). Why? 
 * Well, it so happens that GCC doesn't use a multiply instruction - it 
 * writes out the equivalent shift and add sequence! 
 *
 */
uint32_t ones32(register uint32_t x)
{
        /* 32-bit recursive reduction using SWAR...
	 * but first step is mapping 2-bit values
	 * into sum of 2 1-bit values in sneaky way
	 */
        x -= ((x >> 1) & 0x55555555);
        x  = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x  = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return(x & 0x0000003f);
}




/* 
 * lzc -- Return the number of leading zeroes in a 32-bit value v 
 *  @v: unsigned 32-bit integer value
 */
uint32_t lzc(uint32_t v)
{
        #define WORDBITS 32

        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);

        return (WORDBITS - ones32(v));
}





const char *dispel(uint32_t magic)
{
        #define WORDBITS 32
        static const uint32_t maskmsb = 0x80000000;
        static char buf[33];
        int i;

        for (i=0; i<WORDBITS; i++) {
                if ((magic & maskmsb) == 0) buf[i] = '-';
                else                        buf[i] = '#';
                magic <<= 1;
        }
        buf[i] = '\0';

        return (&buf);
};


int disenchant(uint32_t *buf)
{
        static uint32_t read = 0x80000000;
        static uint32_t cpy;

        if (buf == NULL) cpy <<= 1;
        else             cpy = *buf;
        
        if (cpy == 0U) return -1;
        else
                return ((read & cpy) != 0) ? 1 : 0;
}


int *bitind(uint32_t v, int n)
{
        #define WORDBITS 32
        static uint32_t msb = 0x80000000;
        int *buf;
        int ind;
        int i;

        buf = calloc(n, sizeof(int));
        ind = 0;

        for (i=0; i<WORDBITS; i++) {
                if ((v & msb) == 0) {
                        buf[ind] = i;
                        ind++;
                        if (ind == n) break;
                }
                v <<= 1;
        }
        return (buf);
}
