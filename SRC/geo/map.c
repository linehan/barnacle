// vim:fdm=marker
/*******************************************************************************
 FILENAME:  map.c
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "map.h"
#include "terrain.h"
#include "weather.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gen/perlin.h"
#include "../gen/dice.h"
#include "../pan/test.h"
#include "../lib/ufo.h"

#include "../lib/redblack/rb.h"
#include "../lib/sort/mergesort.h"
#include "../lib/sort/quicksort.h"
#include "../lib/morton.h"


void build_rb_tree(struct rb_tree *tree, int rows, int cols, int total)
{
        uint32_t i, j; // Iterators
        int n = 0;     // Total number of nodes
        uint32_t *m;   // Morton code for each node
        uint32_t z;    // Stores computed Morton code
        uint32_t *tmp; // Will be used to initialize data in tree

        m = malloc(total * sizeof(uint32_t));
        // Collect each coordinate's Morton code in m[]
        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        mort(i, j, &z);
                        m[n++] = z;
                }
        }
        // Sort them
        quicksort(m, n);
        // Insert them into the red-black tree
        while (n-->0) {
                rb_insert(tree, m[n]);
        }
}

MAP *new_map(int rows, int cols)
{
        MAP *new = malloc(sizeof *new);

        new->h    = rows;
        new->w    = cols;
        new->a    = rows*cols;
        new->padx = 1;
        new->pady = 1;
        new->ufo  = new_ufo(rows, cols, 0, 0, 0, 0);
        new->W    = malloc(sizeof new->W);
        new->P    = malloc(sizeof new->P);
        new->tree = malloc(sizeof(struct rb_tree));
        new->tree->root = NULL;
        build_rb_tree(new->tree, new->h, new->w, new->a);

        return (new);
}

void gen_map(MAP *map)
{
        int i;
        double **pmap = gen_perlin_map(map->h, map->w); // 2D Perlin map

        map->W   = newpad(map->h, map->w); // Create new primary pad
        map->win = newwin(LINES, COLS, 0, 0);
        map->P   = new_panel(map->win);      // Attach to window

        for (i=0; i<16; i++) {
                map->L[i] = newpad(map->h, map->w);
        }

        draw_layers(map, pmap);

        for (i=0; i<16; i++) {
                overlay(map->L[i], map->W);
        }

        return map;
}

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
void set_nyb(struct rb_tree *tree, uint32_t z, int n, int s)
{
        struct rb_node *p; // Pointer to the data bucket to be operated on
        uint32_t C;        // Copy of B

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        p->data &= scrub[n];
        p->data |= (state[s]<<offset[n]);

        /*rb_store(map->tree, z, C);*/
}
//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  IN (n): An enum value indicating a nibble of the tile.                    #
//#  IN (s): An enum value indicating a nibble state.                          #
//#  OUTPUT: Returns 1 if the state of nibble n of B is s, else returns 0.     #
//##############################################################################
int is_nyb(uint32_t B, int n, int s)
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
const char *get_nybtag(uint32_t B, int n)
{
        B &= ~(scrub[n]); // wipe all but the nibble we want
        B >>= offset[n];  // push our nibble to the end

        return tags[n][B];
}
//##############################################################################
//#  IN (B): A single tile from the Morton array.                              #
//#  OUTPUT: Side effects -- prints each nibble label and it's state label.    #
//##############################################################################
void stat_nyb(struct rb_tree *tree, uint32_t z)
{
        struct rb_node *p;
        uint32_t C;
        int n = NNIBS;

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        C = p->data; // Make a copy

        while (n--) {
                C &= ~(scrub[n]); // wipe all but the nibble we want
                C >>= offset[n];  // push our nibble to the end

                wprintw(INSPECTORMSGWIN, "%3s: %3s | ", nyb_tags[n], tags[n][C]);
                C = p->data; // Make another copy
        }
}
