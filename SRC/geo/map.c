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

#include "../lib/redblack/rb.h"
#include "../lib/sort/quicksort.h"
#include "../lib/morton.h"
#include "../itm/domain.h"

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
        new->dom   = new_dom(map_nibs, map_opts);
        build_rb_tree(new->dom->tree, new->h, new->w, new->a);

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
}
