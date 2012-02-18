// vim:fdm=marker
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <ncurses.h>
#include <panel.h>
#include "zbox.h"
#include "../lib/quadtree.h"
#include "../pan/test.h"

//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  IN (n): An enum value indicating which nibble of the tile.                #
//#  IN (s): An enum value indicating which state to set.                      #
//#  OUTPUT: Side effects -- sets nibble n of B to state s.                    #
//##############################################################################
//{{{1 Detailed explanation
//##############################################################################
//#                                                                            #
//#   TILE   0010 0011 0100 1110 1111 \__                                      #
//#  SCRUB   1111 0000 1111 1111 1111 /  \                                     #
//#          |||| |||| |||| |||| ||||    AND                                   #
//#  CLEAN   0010 0000 0100 1110 1111 \__                                      #
//#  STATE   0000 1001 0000 0000 0000 /  \                                     #
//#          |||| |||| |||| |||| ||||    OR                                    #
//#  FINAL   0010 1001 0100 1110 1111                                          #
//#                                                                            #
//#                                                                            #
//#  state[opt]    the hex bitmask indexed by opt, (an enum)                   #
//#  offset[tag]   the offset in bits of 'tag', indexed by 'tag' (an enum)     #
//#                                                                            #
//#                                                                            #
//#  (state[O]<<offset[tag])                                                   #
//#     |     |    |                                                           #
//#     |     |    +-----------------------------+                             #
//#     +-----|------------+                     |                             #
//#           |         ___|___           _______|________                     #
//#           |   Shift state[O] right by offset[tag] bits.                    #
//#           |   ``|``          ``|``                                         #
//#           +-----+--------------+                                           #
//#                                                                            #
//##############################################################################
//}}}1
inline void _set_nyb(uint32_t *B, int n, int s)
{
        *B &= scrub[n];
        *B |= (state[s]<<offset[n]);
}
//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  IN (n): An enum value indicating a nibble of the tile.                    #
//#  IN (s): An enum value indicating a nibble state.                          #
//#  OUTPUT: Returns 1 if the state of nibble n of B is s, else returns 0.     #
//##############################################################################
int _is_nyb(uint32_t B, int n, int s)
{
        wprintw(DIAGNOSTIC_WIN, "\ns: %u\nB: %u\nstate[s]: %u\n", s, B, state[s]);
        B &= ~(scrub[n]); // wipe all but the nibble we want
        B >>= offset[n];  // push our nibble to the end
        wprintw(DIAGNOSTIC_WIN, "\ns: %u\nB: %u\nstate[s]: %u\n", s, B, state[s]);

        return (B == state[s]) ? 1 : 0;
}
//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  IN (n): An enum value indicating which nibble of the tile.                #
//#  OUTPUT: A string, the enum label of the nibble's state.                   #
//##############################################################################
const char *_get_nybtag(uint32_t B, int n)
{
        B &= ~(scrub[n]); // wipe all but the nibble we want
        B >>= offset[n];  // push our nibble to the end

        return tags[n][B];
}
//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  OUTPUT: Side effects -- prints each nibble label and it's state label.    #
//##############################################################################
void _stat_nyb(uint32_t B)
{
        uint32_t C = B; // a copy of B
        int n = NNIBS;
        while (n--) {
                C &= ~(scrub[n]); // wipe all but the nibble we want
                C >>= offset[n];  // push our nibble to the end

                wprintw(INSPECTORMSGWIN, "%3s: %3s | ", nyb_tags[n], tags[n][C]);
                C = B;
        }
}
//##############################################################################
//# Allocates memory for a new ZBOX, initializing appropriate values.          #
//##############################################################################
ZBOX *new_zbox(uint32_t rows, uint32_t cols)
{
        uint32_t i, j;
        ZBOX *new;

        new = malloc(sizeof(ZBOX));
        new->n = (rows*cols); // total cells

        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        mort(i, j, &(new->z));
                        new->bst = bst_insert(new->bst, new->z);
                }
        }
        return new;
}
