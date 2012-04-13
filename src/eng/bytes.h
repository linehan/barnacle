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


#define NNIBS    8  // Number of nibbles in a state word.
#define NSTATES 16  // Number of states each nibble can take.
#define NBITS   32  // Number of bits (total) in a state word.

/* Directional */
enum hdg_t {NORTH,NNE,NE,ENE,EAST,ESE,SE,SSE,SOUTH,SSW,SW,WSW,WEST,WNW,NW,NNW};        
static const char *hdg_tag[NSTATES] = {"NORTH","NNE","NE","ENE","EAST","ESE",
                                       "SE","SSE","SOUTH","SSW","SW","WSW",
                                       "WEST","WNW","NW", "NNW"};
/* Graduated */
enum grad_t {NIL=1,HI5=2,HI4=3,HI3=4,HI2=5,HI1=6,HI0=7,LO5=8,LO4=9,LO3=10,
             LO2=11,LO1=12,LO0=13,ALL=14};
static const char *grad_tag[NSTATES] = {"XXX","NIL","HI5","HI4","HI3","HI2",
                                        "HI1","HI0","LO5","LO4","LO3","LO2",
                                        "LO1","LO0","ALL"};

/* Scalar value */
/* No enum is necessary, since it would be redundant. */
//static const char *scal_tag[NSTATES] = {"XX","00","01","02","03","04","05",
                                        //"06","07","08","09","10","11","12",
                                        //"13","14"};


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
