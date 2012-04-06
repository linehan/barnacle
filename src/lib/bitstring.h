#include <stdint.h>
#define WORDBITS 32

#define BITMASK(b) (1 << ((b) % WORDBITS))
#define BITWORD(b) ((b) / WORDBITS)
#define BITSET(a, b) ((a)[BITWORD(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITWORD(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITWORD(b)] & BITMASK(b))
#define BITFIT(nb) ((nb + WORDBITS - 1) / WORDBITS)

void stat_cpuid(void);
unsigned ffz(unsigned v);
unsigned lzc(unsigned v);

