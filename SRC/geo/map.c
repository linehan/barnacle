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
 * Given a rectangle struct rec_t, computes Morton codes for each coordinate
 * pair in the rectangle, sorts them, and inserts them into a red-black tree,
 * which is then returned. 
 */
struct rb_tree *grid_to_tree(int rows, int cols)
{
        uint32_t *m;          // Array of Morton codes, one per grid square
        int n;                // Total number of grid squares 
        int i;
        int j;

        struct rb_tree *tree = new_tree(1);
        m = malloc((rows*cols) * sizeof(uint32_t));
        n = 0;

        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        m[n++] = MORT(i, j);
                }
        }

        quicksort(m, n);

        while (n-->0) 
                rb_insert(tree, m[n]);

        free(m); // The sorted Morton codes have been copied into the tree

        return (tree);
}






/*
 * Allocate memory for a new struct map_t, and initialize certain
 * of its members.
 */
struct map_t *new_map(int h, int w, int scr_h, int scr_w, int scr_y, int scr_x)
{
        struct map_t *new = malloc(sizeof *new);

        new->pan  = malloc(sizeof new->pan);
        new->win  = malloc(sizeof new->win);

        set_ufo(&new->ufo, scr_h, scr_w, scr_y, scr_x, h, w, 0, 0);

        new->tree = grid_to_tree(h, w);

        return (new);
}







/*
 * Generates terrain using a Perlin simplex noise map, which is passed to
 * draw_layers();. Once draw_layers(); returns, the layers in *L[16] are
 * flattened into *W.
 */
void gen_map(struct map_t *map)
{
        int i;
        int h;
        int w;

        h = map->ufo.box.h;
        w = map->ufo.box.w;

        double **pmap = gen_perlin_map(h, w); // 2D Perlin map

        map->win = newwin(LINES, COLS, 0, 0); // Fullscreen
        map->pan = new_panel(map->win);

        for (i=0; i<NLAYERS; i++) {
                if (i == RIM) map->L[i] = new_multiwin(h, w, 0, 0, 2);
                else          map->L[i] = new_multiwin(h, w, 0, 0, 1);
        }

        map->W = new_multiwin(h, w, 0, 0, 2);

        draw_layers(map, pmap);
        draw_water_rim(map);
        restack_map(map);
}




void restack_map(struct map_t *map)
{
        int i;
        for (i=0; i<NLAYERS; i++) {
                overlay(PEEK(map->L[i]), PEEK(map->W));
        }
}




void roll_map(struct map_t *map, int dir)
{
        switch (dir) {
        case 'l': 
                ufo_left(map, ufo);
                break;
        case 'r': 
                ufo_right(map, ufo);
                break;
        case 'u': 
                ufo_up(map, ufo);
                break;
        case 'd': 
                ufo_down(map, ufo);
                break;
        }
        map_refresh(map);
        scr_refresh();
}
