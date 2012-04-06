#define WORDBITS 32

#define BITMASK(b) (1 << ((b) % WORDBITS))
#define BITWORD(b) ((b) / WORDBITS)
#define BITSET(a, b) ((a)[BITWORD(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITWORD(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITWORD(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + WORDBITS - 1) / WORDBITS)

