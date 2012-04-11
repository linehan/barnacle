#pragma once
#ifndef __BITMAP_H
#define __BITMAP_H

typedef uint32_t *BITSTRING;  /* Pointer to an unsigned byte array */
#define WORDBITS 32U          /* The number of bits in a byte */


#define FITBITS(nbits) ((nbits) / WORDBITS)



#define BitInWord(bit) /* Expands to the bitmask for a uint32_t */ \
        (1 << ((bit) % WORDBITS))

#define WordWithBit(str, bit) /* Return the word offset (index) of bit */ \
        (((BITSTRING)(str))[(bit) / WORDBITS])
        
#define BitIsTrue(str,bit)  /* Equivalent to AND */ \
        (WordWithBit(str,bit) & BitInWord(bit))

#define BitIsFalse(str,bit) /* Equivalent to NOT */ \
        (!BitIsTrue(str,bit))

#define BitSetTrue(str,bit)  /* Assign value of true */ \
        (WordWithBit(str,bit) |= BitInWord(bit))

#define BitSetFalse(str,bit) /* Assign value of false */ \
        (WordWithBit(str,bit) &= ~BitInWord(bit))

#define BitToggle(str, bit) /* Toggle a bit */ \
        (WordWithBit(str,bit) ^= BitInWord(bit))

#define BitValue(str, bit) /* Return boolean value of bit */ \
        (BitIsTrue((str),(bit)) ? True : False)
        
#define BitSet(str, bit, val) /* Set bit to certain boolean value */ \
        (val) ? BitTrue((str),(bit)) : BitFalse((str),(bit))
        
