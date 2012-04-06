// vim:fdm=marker
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/redblack/rb.h"
#include "state.h"



/* Bit masks for each of the 16 possible states of a single nibble. */
static const uint32_t 
state[NSTATES] = {0x00000000, 0x00000001, 0x00000002, 0x00000003,
                  0x00000004, 0x00000005, 0x00000006, 0x00000007,
                  0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
                  0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F};


/* Bit masks for each of the 8 nibbles in a 32-bit uint32_t. */
static const uint32_t 
scrub[NNIBS]   = {0x0FFFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, 0xFFF0FFFF,
                  0xFFFF0FFF, 0xFFFFF0FF, 0xFFFFFF0F, 0xFFFFFFF0};


/* Inverted bit masks for each of the 8 nibbles in a 32-bit uint32_t. */
static const uint32_t 
gouge[NNIBS]   = {0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
                  0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F};


/* The left offset of each of the 8 nibbles in a 32-bit uint32_t. */
static const uint32_t 
offset[NNIBS]  = {28, 24, 20, 16, 12, 8, 4, 0};




/*
  Retreive a cell with key 'z', and set nibble 'n' of word 'w' to state 's'.
*/
void set_state(struct rb_tree *tree, uint32_t key, int w, int n, int s)
{
        rb_peek(tree, key);

        if (tree->peek == NULL) return;

        tree->peek->data[w] &= scrub[n];
        tree->peek->data[w] |= (state[s]<<offset[n]);
}

/*
  Retreive nibble 'n' of word 'w' of cell 'z', and return its state 's'.
*/
int get_state(struct rb_tree *tree, uint32_t key, int w, int n)
{
        uint32_t c; /* Copy of data */

        rb_peek(tree, key);
        if (tree->peek == NULL) return;
        
        c   = tree->peek->data[w]; 
        c  &= ~(scrub[n]);         /* Wipe all but n */
        c >>= offset[n];           /* Move n to the end */

        return (int)(c);
}

/*
  Return state 's' of nibble 'n', in uint32_t 'v'
*/
int see_state(int n, uint32_t *v)
{
        uint32_t c = *v; // copy

        c &= ~(scrub[n]);
        c >>= offset[n];
        return (int)(c);
}


/*
  Retreive nibble 'n' of word 'w' of cell 'z', and test its state against 's'.
*/
int is_state(struct rb_tree *tree, uint32_t key, int w, int n, int s)
{
        uint32_t c; /* Copy of data */

        rb_peek(tree, key);
        if (tree->peek == NULL) return (0);

        c   = tree->peek->data[w]; 
        c  &= ~(scrub[n]);         /* Wipe all but n */
        c >>= offset[n];           /* Move n to the end */

        return (c == state[s]) ? 1 : 0;
}


