#include <stdint.h>
/*
  Compute the Morton number (aka z-code) of (y,x), and store it in z
*/
inline static void mort(uint32_t y, uint32_t x, uint32_t *z)
{
        static const uint32_t B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
        static const uint32_t S[] = {1, 2, 4, 8};

        x = (x | (x << S[3])) & B[3];
        x = (x | (x << S[2])) & B[2];
        x = (x | (x << S[1])) & B[1];
        x = (x | (x << S[0])) & B[0];
        
        y = (y | (y << S[3])) & B[3];
        y = (y | (y << S[2])) & B[2];
        y = (y | (y << S[1])) & B[1];
        y = (y | (y << S[0])) & B[0];

        *z = x | (y << 1);
}

/*
  Determines the integer value of the highest bit set (i.e. equal to 1) in
  the 32-bit integer v. This turns out to be equivalent to computing the
  integer log base 2 of the integer v.

  Bit Twiddling Hacks description:

       "Eric Cole devised this January 8, 2006 after reading about the 
        entry below to round up to a power of 2 and the method below for 
        computing the number of trailing bits with a multiply and lookup 
        using a DeBruijn sequence. On December 10, 2009, Mark Dickinson 
        shaved off a couple operations by requiring v be rounded up to 
        one less than the next power of 2 rather than the power of 2."
*/
inline static int highest_bit_set(uint32_t v)
{
        static const int MultiplyDeBruijnBitPosition[32] = 
        {
          0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
          8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
        };

        v |= v >> 1; // first round down to one less than a power of 2 
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;

        return (MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27]);
}
