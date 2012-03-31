#ifndef __BYTE_MODEL
#define __BYTE_MODEL

/*
  A set of model routines to pack, unpack, and alter the state of 
  (8-bit) byte values stored in a single uint32_t, as well as (4-bit)
  nibble values stored in the same.
*/
#include <assert.h>
#include <stdint.h>

static const int NBYTES=4;
static const int NNIBBLES=8;
//static const int NSTATES=256;

void     set_byte(uint32_t *word, int b, int s);
int      get_byte(uint32_t word, int b);
int       is_byte(uint32_t word, int b, int s);
int       or_byte(uint32_t word, int b, int n, int s0,...);
void unpack_bytes(uint32_t word, int *dest, size_t n);

void     set_nibble(uint32_t *word, int n, int s);
int      get_nibble(uint32_t word, int n);
int       is_nibble(uint32_t word, int n, int s);
int       or_nibble(uint32_t word, int b, int n, int s0,...);
void unpack_nibbles(uint32_t word, int *dest, size_t n);




static inline void inc_byte(uint32_t *word, int b)
{
        assert(word != NULL);
        set_byte(word, b, (get_byte(*word, b))+1);
}
static inline void dec_byte(uint32_t *word, int b)
{
        assert(word != NULL);
        int byte = get_byte(*word, b);

        if (byte > 0) byte--;
        else return;

        set_byte(word, b, byte);
}


static inline void inc_nibble(uint32_t *word, int n)
{
        assert(word != NULL);
        set_nibble(word, n, (get_nibble(*word, n))+1);
}
static inline void dec_nibble(uint32_t *word, int n)
{
        assert(word != NULL);
        int nib = get_nibble(*word, n);

        if (nib > 0) nib--;
        else return;

        set_nibble(word, n, nib);
}

#endif
