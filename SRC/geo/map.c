// vim:fdm=marker
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
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

#include "../gfx/gfx.h"
#include "../gen/perlin.h"
#include "../pan/test.h"
#include "../lib/ufo.h"

#include "../lib/redblack/rb.h"
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

        new->h    = rows;
        new->w    = cols;
        new->a    = rows*cols;
        new->padx = 1;
        new->pady = 1;
        new->ufo  = new_ufo(rows, cols, 0, 0, (2*rows), (2*cols), 0, 0);
        new->W    = malloc(sizeof new->W);
        new->pan  = malloc(sizeof new->pan);
        new->win  = malloc(sizeof new->win);
        new->tree = malloc(sizeof(struct rb_tree));
        new->tree->root = NULL;
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

/*
  Retreive a cell with key 'z', and set its 'n'th nibble to state 's'.
*/
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
void set_cell(struct rb_tree *tree, uint32_t z, int n, int s)
{
        struct rb_node *p; // Pointer to the data bucket to be operated on

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        p->data &= scrub[n];
        p->data |= (state[s]<<offset[n]);
}
/*
  Tests nibble 'n' of cell with key 'z'. If nibble has state 's', is_cell() 
  returns 1, else it returns 0.
*/
int is_cell(struct rb_tree *tree, uint32_t z, int n, int s)
{
        struct rb_node *p; // Pointer to the data bucket to be operated on
        uint32_t C;        // Copy of the cell's data

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        C = p->data;      // Make a copy of the data
        C &= ~(scrub[n]); // Wipe all but the nibble we want
        C >>= offset[n];  // Push our nibble to the end

        return (C == state[s]) ? 1 : 0;
}
/*
  Returns a pointer to a string, which represents the enum tag corresponding
  to the state of nibble 'n' of cell with key 'z'.
*/
const char *get_celltag(struct rb_tree *tree, uint32_t z, int n)
{
        struct rb_node *p; // Pointer to the data bucket to be operated on
        uint32_t C;        // Copy of the cell's data

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        C = p->data;      // Make a copy of the data
        C &= ~(scrub[n]); // wipe all but the nibble we want
        C >>= offset[n];  // push our nibble to the end

        return tags[n][C];
}
/*
  Prints a formatted string consisting of the enum tags for every nibble and
  state of cell with key 'z'.
*/
void stat_cell(struct rb_tree *tree, uint32_t z)
{
        struct rb_node *p; // Pointer to the data bucket to be operated on
        uint32_t C;        // Copy of the cell's data.
        int n = NNIBS-1;   // Number of nibbles in a cell (-1 for fencepost)

        p = rb_retreive(tree->root, z);
        if (p == NULL) return;

        do {
                C = p->data;      // Make a copy
                C &= ~(scrub[n]); // Wipe all but the nibble we want
                C >>= offset[n];  // Push our nibble to the end
                wprintw(INSPECTORMSGWIN, "%3s: %3s | ", nyb_tags[n], tags[n][C]);
        } while (n-->0);
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
        case 0:
                break;
        }
        map_refresh(map);
        scr_refresh();
}
