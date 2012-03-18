#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

// Returns true if word is even, returns false if word is odd.
bool par(uint32_t v)
{
        static const bool ParityTable256[256] = {
        #define P2(n) n, n^1, n^1, n
        #define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
        #define P6(n) P4(n), P4(n^1), P4(n^1), P4(n) P6(0), P6(1), P6(1), P6(0)
        };

        v ^= v >> 16;
        v ^= v >> 8;

        return (ParityTable256[v & 0xff]);

/******************************************************************************
 * Bruce Rawles found a typo in an 
 * instance of the table variable's name on September 27, 2005, and he 
 * received a $10 bug bounty. 
 *
 * On October 9, 2006, Fabrice Bellard suggested the 32-bit variations above, 
 * which require only one table lookup; the previous version had four lookups 
 * (one per byte) and were slower. 
 *
 * On July 14, 2009 Hallvard Furuseth suggested the macro compacted table.    
 ******************************************************************************/
}


// The best method for counting bits in a 32-bit integer v is the following:
uint32_t ones(uint32_t v)
{
        v = v - ((v>>1) & 0x55555555);                    
        v = (v & 0x33333333) + ((v>>2) & 0x33333333);  

        return (((v + (v>>4) & 0xF0F0F0F) * 0x1010101) >> 24);
/******************************************************************************
 * The best bit counting method takes only 12 operations, which is the same 
 * as the lookup-table method, but avoids the memory and potential cache 
 * misses of a table. It is a hybrid between the purely parallel method above 
 * and the earlier methods using multiplies (in the section on counting bits 
 * with 64-bit instructions), though it doesn't use 64-bit instructions. The 
 * counts of bits set in the bytes is done in parallel, and the sum total of 
 * the bits set in the bytes is computed by multiplying by 0x1010101 and 
 * shifting right 24 bits. 
 ******************************************************************************/
}

// Count leading zeroes in a word
uint32_t lzc(uint32_t v)
{
        #define WORDBITS 32

        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        return(WORDBITS - ones(v));
}

/******************************************************************************
 * The following finds the the rank of a bit, meaning it returns the sum of     *
 * bits that are set to 1 from the most-signficant bit down to the bit at the   *
 * given position.                                                              *
 *----------------------------------------------------------------------------
 * Juha Järvi sent this to me on November 21, 2009 as an inverse operation to 
 * the computing the bit position with the given rank, which follows.         
 ******************************************************************************/
uint64_t cbs(uint32_t v, unsigned int pos)
{
        //uint64_t v;       // Compute rank (bits set) in v from the MSB to pos.
        //unsigned int pos; Bit position to count bits upto.
        uint64_t r;       // Resulting rank of bit at pos goes here.

        // Shift out bits after given position.
        r = v >> (sizeof(v) * CHAR_BIT - pos);
        // Count set bits in parallel.
        // r = (r & 0x5555...) + ((r >> 1) & 0x5555...);
        r = r - ((r >> 1) & ~0UL/3);
        // r = (r & 0x3333...) + ((r >> 2) & 0x3333...);
        r = (r & ~0UL/5) + ((r >> 2) & ~0UL/5);
        // r = (r & 0x0f0f...) + ((r >> 4) & 0x0f0f...);
        r = (r + (r >> 4)) & ~0UL/17;
        // r = r % 255;
        r = (r * (~0UL/255)) >> ((sizeof(v) - 1) * CHAR_BIT);

        return (r);
}


uint32_t mask(int n)
{
        return (~0 << n) >> n;
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
