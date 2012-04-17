/*
 * map.c
 *
 * A 'map' in this program refers to the persistent data structure that 
 * organizes the terrain, climate, and other spatial features in a regularly
 * addressable manner. In many ways it is the most important data structure
 * in the program, since it holds the context within which most other actions 
 * are executed, and to which most decisions refer.
 *
 * There are two primary members of a map structure: 
 *
 *      WINDOW *L[16]   An array of 16 pads (see curs_pad(3X))
 *      MATRIX *mx      A matrix containing label values
 *      
 * Essentially, the map stores two kinds of information, addressed to the
 * same square grid. One is rendering meant to be drawn to the screen (L[16]),
 * while the other is labeling, meant to be traversed, interpreted, or
 * altered by terrain algorithms.
 *
 * Document more later...
 */

#include <stdlib.h>
#include "../gfx/gfx.h"
#include "../gfx/ui/titlecard.h"
#include "../lib/stoc/stoc.h"
#include "../test/test.h"
#include "map.h"
#include "terrain.h"

/* -------------------------------------------------------------------------- */

void map_restack(void *mymap);

/* -------------------------------------------------------------------------- */

/*
 * new_map -- allocate and initialize new map structure 
 * @h: the desired map height
 * @w: the desired map width
 * @scr_h: the screen height for the pad window 
 * @scr_w: the screen width for the pad window
 * @scr_y: the screen y-offset for the pad window 
 * @scr_x: the screen x-offset for the pad window 
 */
struct map_t *new_map(int h, int w, int scr_h, int scr_w, int scr_y, int scr_x)
{
        struct map_t *new;
        int i;
       
        new = malloc(sizeof(struct map_t));

        /* Build some stuff */
        new->mx = new_matrix(h, w);
        set_ufo(&new->ufo, scr_h, scr_w, scr_y, scr_x, h, w, 0, 0);

        /* Add methods */
        new->render  = &map_render;  /* see terrain.c */
        new->restack = &map_restack;

        /* Build windows, pads, and panels */
        new->win = newwin(LINES, COLS, 0, 0); /* Fullscreen */
        new->pan = new_panel(new->win);
        for (i=0; i<NLAYERS; i++) {
                if (i == RIM) 
                        new->L[i] = new_multiwin(h, w, 0, 0, 4);
                else          
                        new->L[i] = new_multiwin(h, w, 0, 0, 1);
        }
        new->W = new_multiwin(h, w, 0, 0, 2);

        return (new);
}


/*
 * map_gen -- fill a new map with generated data that can be rendered
 * @map: pointer to a previously-allocated map
 * @pmap: matrix of doubles used for labeling. If NULL, simplex noise is used
 * @opt: option parameter
 */
void map_gen(struct map_t *map, double **pmap, int opt)
{
        print_status("Generating noise...");

        if (pmap == NULL) {
                map->pmap = simplex_matrix(map->ufo.box.h, map->ufo.box.w);
        } else {
                map->pmap = pmap;
        }

        print_status(SUCCESS);

        map_label(map, opt); 
        map->render(map);
        map->restack(map);
}


/*
 * map_restack (METHOD) -- re-draw the visual layers in their sorted order
 * @mymap: void * pointer to a previously-allocated map
 *
 * Loop over the map's WINDOW stack, and overlay each successive layer
 * into the WINDOW at map->W, so that when the loop terminates, map->W
 * holds the (non-destructive) union of all layers in the WINDOW stack.
 */
void map_restack(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        int i;

        for (i=0; i<NLAYERS; i++) {
                overlay(PEEK(map->L[i]), PEEK(map->W));
        }
}


/*
 * Returns 1 (true) if collision detected, otherwise returns 0 (false).
 */
int map_hit(struct map_t *map, struct rec_t *rec)
{
        uint32_t i;
        uint32_t j;

        for (i=rec->y; i<(rec->h+rec->y); i++) {
                for (j=rec->x; j<(rec->x+rec->w); j++) {
                        /*if (is_state(map->tree, z, 0, LAY, TOP)) return (1);*/
                        /*if (is_state(map->tree, z, 0, LAY, DRP)) return (1);*/
                        /*if (is_state(map->tree, z, 0, LAY, GRO)) return (1);*/
                        /*if (is_state(map->tree, z, 0, LAY, VEG)) return (1);*/
                }
        }
        return (0);
}



void map_roll(struct map_t *map, int dir)
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
}

