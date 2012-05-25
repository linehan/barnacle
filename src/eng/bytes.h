#ifndef __BYTE_MODEL
#define __BYTE_MODEL

/*
  A set of model routines to pack, unpack, and alter the state of 
  (8-bit) byte values stored in a single uint32_t, as well as (4-bit)
  nibble values stored in the same.
*/
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include "../com/arawak.h"

static const int NBYTES=4;
static const int NNIBBLES=8;
//static const int NSTATES=256;

#define BYTESTATES 256
#define NNIBS    8  // Number of nibbles in a state word.
#define NSTATES 16  // Number of states each nibble can take.
#define NBITS   32  // Number of bits (total) in a state word.


// Lookup table for each possible state of a single byte.
static uint32_t state[]={
        0x00000000, 0x00000001, 0x00000002, 0x00000003,
        0x00000004, 0x00000005, 0x00000006, 0x00000007,
        0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
        0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,

        0x00000010, 0x00000011, 0x00000012, 0x00000013,
        0x00000014, 0x00000015, 0x00000016, 0x00000017,
        0x00000018, 0x00000019, 0x0000001A, 0x0000001B,
        0x0000001C, 0x0000001D, 0x0000001E, 0x0000001F,

        0x00000020, 0x00000021, 0x00000022, 0x00000023,
        0x00000024, 0x00000025, 0x00000026, 0x00000027,
        0x00000028, 0x00000029, 0x0000002A, 0x0000002B,
        0x0000002C, 0x0000002D, 0x0000002E, 0x0000002F,

        0x00000030, 0x00000031, 0x00000032, 0x00000033,
        0x00000034, 0x00000035, 0x00000036, 0x00000037,
        0x00000038, 0x00000039, 0x0000003A, 0x0000003B,
        0x0000003C, 0x0000003D, 0x0000003E, 0x0000003F,

        0x00000040, 0x00000041, 0x00000042, 0x00000043,
        0x00000044, 0x00000045, 0x00000046, 0x00000047,
        0x00000048, 0x00000049, 0x0000004A, 0x0000004B,
        0x0000004C, 0x0000004D, 0x0000004E, 0x0000004F,

        0x00000050, 0x00000051, 0x00000052, 0x00000053,
        0x00000054, 0x00000055, 0x00000056, 0x00000057,
        0x00000058, 0x00000059, 0x0000005A, 0x0000005B,
        0x0000005C, 0x0000005D, 0x0000005E, 0x0000005F,

        0x00000060, 0x00000061, 0x00000062, 0x00000063,
        0x00000064, 0x00000065, 0x00000066, 0x00000067,
        0x00000068, 0x00000069, 0x0000006A, 0x0000006B,
        0x0000006C, 0x0000006D, 0x0000006E, 0x0000006F,

        0x00000070, 0x00000071, 0x00000072, 0x00000073,
        0x00000074, 0x00000075, 0x00000076, 0x00000077,
        0x00000078, 0x00000079, 0x0000007A, 0x0000007B,
        0x0000007C, 0x0000007D, 0x0000007E, 0x0000007F,

        0x00000080, 0x00000081, 0x00000082, 0x00000083,
        0x00000084, 0x00000085, 0x00000086, 0x00000087,
        0x00000088, 0x00000089, 0x0000008A, 0x0000008B,
        0x0000008C, 0x0000008D, 0x0000008E, 0x0000008F
};


enum unit {BYTE, NIBBLE};


// Bit masks
static const uint32_t bscrub[]={0x00FFFFFF,0xFF00FFFF,0xFFFF00FF,0xFFFFFF00};

static const uint32_t nscrub[]={0x0FFFFFFF,0xF0FFFFFF,0xFF0FFFFF,0xFFF0FFFF,
                                0xFFFF0FFF,0xFFFFF0FF,0xFFFFFF0F,0xFFFFFFF0};
// Inverted bit masks
static const uint32_t bgouge[]={0xFF000000,0x00FF0000,0x0000FF00,0x000000FF};

static const uint32_t ngouge[]={0xF0000000,0x0F000000,0x00F00000,0x000F0000,
                                0x0000F000,0x00000F00,0x000000F0,0x0000000F};

// Bit offsets (little-endian)
static const uint32_t boffset[]={24, 16, 8, 0};
static const uint32_t noffset[]={28, 24, 20, 16, 12, 8, 4, 0};





/* Set byte 'b' of 'word' equal to state 's' */
static inline void set_byte(uint32_t *word, int b, int s)
{
        uint32_t c = *word; // copy

        c &= bscrub[b];
        c |= (state[s]<<boffset[b]);

        *word = c;
}


// Return the state value of byte 'b' of 'word'.
static inline int get_byte(uint32_t word, int b)
{
        word &= ~(bscrub[b]);
        word >>= boffset[b];
        return (int)(word);
}



// Returns 1 if byte 'b' of 'word' is equal to state 's', else returns 0.
static inline int is_byte(uint32_t word, int b, int s)
{
        word  &= ~(bscrub[b]);
        word >>=  boffset[b];   

        return (word == state[s]) ? 1 : 0;
}


// Returns 1 if byte 'b' of 'word' is equal to states 's0, s1...', else returns 0.
static inline int or_byte(uint32_t word, int b, int n, int s0,...)
{
        va_list states;
        int s;
     
        va_start(states, s0);         /* Initialize the argument list. */
        
        s = s0;

        while (n-->0 && ((word & ~(bscrub[b])) >> boffset[b]) != state[s])
                s = va_arg(states, int);

        va_end (states); 
        return (n>=0) ? 1 : 0;
}


// Attempts to copy each of the 4 byte values of 'word' to the memory
// at 'dest'. Expects 'dest' to be at least 4 ints.
static inline void unpack_bytes(uint32_t word, int *dest, size_t n)
{
        if (dest==NULL || n<NBYTES) return;
        int i;
        for (i=0; i<NBYTES; i++)
                dest[i]=get_byte(word, i); 
}




// Set nibble 'n' of 'word' equal to state 's'.
static inline inline void set_nibble(uint32_t *word, int n, int s)
{
        uint32_t c = *word; // copy

        c &= nscrub[n];
        c |= (state[s]<<noffset[n]);

        *word = c;
}


/* Return the value of nibble 'n' of 'word'. */
static inline int get_nibble(uint32_t word, int n)
{
        word  &= ~(nscrub[n]); 
        word >>=  noffset[n];          

        return (int)(word);
}

// Returns 1 if nibble 'n' of 'word' is equal to state 's', else returns 0.
static inline int is_nibble(uint32_t word, int n, int s)
{
        word  &= ~(nscrub[n]);
        word >>=  noffset[n];   

        return (word == state[s]) ? 1 : 0;
}

// Returns 1 if byte 'b' of 'word' is equal to states 's0, s1...', else returns 0.
static inline int or_nibble(uint32_t word, int b, int n, int s0,...)
{
        va_list states;
        int s;
     
        va_start(states, s0);         /* Initialize the argument list. */
        
        s = s0;

        while (n-->0 && ((word & ~(nscrub[b])) >> noffset[b]) != state[s])
                s = va_arg(states, int);

        va_end (states); 
        return (n>=0) ? 1 : 0;
}


// Attempts to copy each of the 8 nibble values of 'word' to the memory
// at 'dest'. Expects 'dest' to be able to hold at least 8 ints.
static inline void unpack_nibbles(uint32_t word, int *dest, size_t n)
{
        if (dest==NULL || n<NNIBBLES) return;
        int i;
        for (i=0; i<NNIBBLES; i++)
                dest[i]=get_nibble(word, i); 
}




static inline void add_byte(uint32_t *word, int b, int n)
{
        int byte = get_byte(*word, b);

        if (byte < (MAX(255,n) - MIN(255,n)))
                byte += n;
        else
                byte = 255;

        set_byte(word, b, byte);
}
static inline void sub_byte(uint32_t *word, int b, int n)
{
        int byte = get_byte(*word, b);

        if (byte > n)
                byte -= n;
        else
                byte = 0;

        set_byte(word, b, byte);
}

static inline void inc_byte(uint32_t *word, int b)
{
        int byte = get_byte(*word, b);

        if (byte < 255)
                byte += 1;

        set_byte(word, b, byte);
}
static inline void dec_byte(uint32_t *word, int b)
{
        int byte = get_byte(*word, b);

        if (byte > 0) byte--;

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
