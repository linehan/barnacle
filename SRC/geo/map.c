// vim:fdm=marker
<<<<<<< .merge_file_NFMWFU
//##############################################################################
//# FILENAME: map.c                                                            #
//# -------------------------------------------------------------------------- #
//# Data structures and functions for a procedural world map.                  #
//#                                                                            #
//# GRAPHICAL DATA                                                             #
//#                                                                            #
//#     WINDOW *L[16];                                                         #
//#             Array of 16 pads, one for each graphics layer. Each layer is   #
//#             drawn on individually during terrain generation. When done,    #
//#             the array is flattened using the overlay(); routine.           #
//#                                                                            #
//#     WINDOW *W;                                                             #
//#             The destination of the overlay(); call following terrain       #
//#             generation. Holds the aggregate of the 16 layer pads, in       #
//#             proper order. Should serve as the sole "world map window".     #
//#                                                                            #
//#     WINDOW *win;                                                           #
//#             The effective "viewport" for the pad *W. Pads cannot be        #
//#             displayed all at once, nor can they be attached to PANELs      #
//#             without causing trouble. By using *win and the copywin();      #
//#             routine, we can minimize this shortcoming.                     #
//#                                                                            #
//#     PANEL *pan;                                                            #
//#             The PANEL to which *win is attached.                           #
//#                                                                            #
//# CELLULAR DATA                                                              #
//#                                                                            #
//#     struct rb_tree *tree;                                                  #
//#             Stores data about each cell (character box) of the world map.  #
//#             *tree is a red-black tree, a kind of self-balancing binary     #
//#             search tree. The lookup key for a particular cell is the       #
//#             Morton code, or z-code, of that cell's y and x coordinates.    #
//#             See "morton.h".                                                #
//#                                                                            #
//#     struct ufo_t *ufo;                                                     #
//#             Used to simplify movement of object within a specified range.  #
//#             See "ufo.h".                                                   #
//#                                                                            #
//##############################################################################
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <wchar.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

#include "../gfx/gfx.h"
#include "../gen/perlin.h"
#include "../pan/test.h"
#include "../lib/ufo.h"

#include "../lib/sort/quicksort.h"
#include "../lib/morton.h"

#include "map.h"
#include "terrain.h"
//##############################################################################
/*
  Initialize and construct the red-black tree structure. Generates the 
  Morton code keys and inserts them into the tree in sorted order.
*/
void build_rb_tree(struct rb_tree *tree, int rows, int cols, int total)
{
        uint32_t i, j; // Iterators
        uint32_t *m;   // Morton code for each node
        uint32_t z;    // Stores computed Morton code
        int n;         // Total number of nodes

        m = malloc(total * sizeof(uint32_t));
        n = 0;

        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        MORT(i, j, &z);
                        m[n++] = z; // Collect each coordinate's Morton code
                }
        }
        quicksort(m, n); // Sort them
        while (n-->0) {
                rb_insert(tree, m[n]); // Insert them into the rb-tree
        }
}

/*
  Allocate memory for a new struct map_t, and initialize some of its members.
*/
struct map_t *new_map(int rows, int cols)
{
        struct map_t *new = malloc(sizeof *new);
=======
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
#include "../cmd/control.h"
#include "../pan/test.h"
#include "../lib/ufo.h"
#include "../gfx/zbox.h"

#include "../lib/redblack/rb.h"
#include "../lib/mergesort.h"
#include "../lib/morton.h"


MAP *new_map(int rows, int cols)
{
        MAP *new = malloc(sizeof *new);
>>>>>>> .merge_file_SU1vDU

        new->h    = rows;
        new->w    = cols;
        new->a    = rows*cols;
        new->padx = 1;
        new->pady = 1;
<<<<<<< .merge_file_NFMWFU
        new->ufo  = new_ufo(rows, cols, 0, 0, (2*rows), (2*cols), 0, 0);
        new->W    = malloc(sizeof new->W);
        new->pan  = malloc(sizeof new->pan);
        new->win  = malloc(sizeof new->win);
        new->tree = new_tree(1);
        build_rb_tree(new->tree, new->h, new->w, new->a);

        return (new);
}
/*
  Generates terrain using a Perlin simplex noise map, which is passed to
  draw_layers();. Once draw_layers(); returns, the layers in *L[16] are
  flattened into *W.
*/
void gen_map(struct map_t *map)
{
        int i;
        double **pmap = gen_perlin_map(map->h, map->w); // 2D Perlin map

        map->win = newwin(LINES, COLS, 0, 0); // Fullscreen
        map->pan = new_panel(map->win);

        for (i=0; i<16; i++) {
                if (i == RIM) map->L[i] = new_winring(map->h, map->w, 0, 0, 2);
                else          map->L[i] = new_winring(map->h, map->w, 0, 0, 1);
        }

        map->W = new_winring(map->h, map->w, 0, 0, 2);

        draw_layers(map, pmap);
        draw_water_rim(map);
        restack_map(map);
}

void restack_map(struct map_t *map)
{
        int i;
        for (i=0; i<16; i++) {
                overlay(PEEK(map->L[i]), PEEK(map->W));
        }
}

void roll_map(struct map_t *map, int dir)
{
        switch (dir) {
        case 'l': 
                ufo_l(map->ufo);
                break;
        case 'r': 
                ufo_r(map->ufo);
                break;
        case 'u': 
                ufo_u(map->ufo);
                break;
        case 'd': 
                ufo_d(map->ufo);
                break;
        }
        map_refresh(map);
        scr_refresh();
=======
        new->ufo  = new_ufo(rows, cols, 0, 0, 0, 0);
        new->W    = malloc(sizeof new->W);
        new->P    = malloc(sizeof new->P);
        new->tree = malloc(sizeof new->tree);

        return (new);
}

void build_rb_tree(struct rb_tree *tree, int rows, int cols, int total)
{
        uint32_t i, j;     // Iterators
        int n = 0;         // Total number of nodes
        uint32_t m[total]; // Morton code for each node
        uint32_t z;        // Stores computed Morton code
        void *data;        // Data bucket to be allocated in node

        // Collect each coordinate's Morton code in m[]
        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        mort(i, j, &z);
                        m[n++] = z;
                }
        }
        // Sort them
        merge_sort(m, n);
        // Insert them into the red-black tree
        while (n-->0) {
                rb_insert(tree, m[n]);
                data = rb_retreive(tree, m[n]);
                data = calloc(1, sizeof(uint32_t)); // Initialize data field
        }
}

MAP *gen_map(int rows, int cols)
{
        int i;
        double **pmap = gen_perlin_map(rows, cols); // 2D Perlin map

        MAP *map = new_map(rows, cols);
        map->W   = newpad(map->h, map->w); // Create new primary pad
        map->P   = new_panel(map->W);      // Attach to window

        build_rb_tree(map->tree, map->h, map->w, map->a);
        
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
void set_nyb(uint32_t *B, int n, int s)
{
        *(B) &= scrub[n];
        *(B) |= (state[s]<<offset[n]);
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
void stat_nyb(uint32_t B)
{
        uint32_t C = B; // a copy of B
        int n = NNIBS;
        while (n--) {
                C &= ~(scrub[n]); // wipe all but the nibble we want
                C >>= offset[n];  // push our nibble to the end

                wprintw(INSPECTORMSGWIN, "%3s: %3s | ", nyb_tags[n], tags[n][C]);
                C = B;
        }
>>>>>>> .merge_file_SU1vDU
}
