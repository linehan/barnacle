#pragma once
#ifndef __BITFIT_H
#define __BITFIT_H

#include <stdint.h>
#define WORDBITS 32


void stat_cpuid(void);
unsigned ffz(uint32_t val);
unsigned lzc(uint32_t val);


#define FHEX "%#010x"


#define BITSTRING(nb) ((nb + WORDBITS - 1) / WORDBITS)

#define BITMASK(b) (1 << ((b) % WORDBITS))
#define BITWORD(b) ((b) / WORDBITS)
#define BITSET(a, b) ((a)[BITWORD(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITWORD(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITWORD(b)] & BITMASK(b))

#define BITFFZ(a, b) while (BITTEST(a, b) && b++); b++



#endif
