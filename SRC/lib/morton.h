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

#define MORT(y, x, z) \
        mort((uint32_t)y, (uint32_t)x, z) 

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


// Inverse of Part1By1 - "delete" all odd-indexed bits
inline static uint32_t trom(uint32_t x)
{
  x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
  x = (x ^ (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
  x = (x ^ (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
  x = (x ^ (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
  x = (x ^ (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
  return x;
}

inline static uint32_t trom_x(uint32_t mort)
{
  return trom(mort>> 0);
}

inline static uint32_t trom_y(uint32_t mort)
{
  return trom(mort >> 1);
}
