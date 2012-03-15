#include <stdint.h>
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
