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
#include "../mob/mob.h"
#include "../test/test.h"
#include "map.h"
#include "terrain.h"

/* -------------------------------------------------------------------------- */

void map_restack(void *mymap);
void map_show(void *mymap);
void map_hide(void *mymap);

/* -------------------------------------------------------------------------- */

/*
 * new_map -- allocate and initialize new map structure 
 * @h: the desired map height
 * @w: the desired map width
 */
struct map_t *new_map(int h, int w)
{
        #define SCR_WIDTH COLS 
        #define SCR_HEIGHT LINES
        #define SCR_X0 0
        #define SCR_Y0 0

        struct map_t *new;
        int i;
       
        new = malloc(sizeof(struct map_t));

        /* Build some stuff */
        new->mx = new_matrix(h, w);
        new->hook = new_matrix(h, w);
        set_ufo(&new->ufo, SCR_HEIGHT, SCR_WIDTH, SCR_Y0, SCR_X0, h, w, 0, 0);

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


void map_hide(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        hide_panel(map->pan);
}

void map_show(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        show_panel(map->pan);
}


void map_trigger(struct map_t *map, struct mob_t *mob)
{
        int y;
        int x;
        int i;

        y = ufo_y(mob, ufo);
        x = ufo_x(mob, ufo);

        i = mx_val(map->hook, y, x);
        
        if (map->asset[i] != NULL)
                map->asset[i]->trigger();
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


void map_swap(void)
{
        MAPBOOK->hide(ACTIVE);

        MAPBOOK->field_is_active ^= true;

        ACTIVE = (MAPBOOK->field_is_active) ? FIELD : WORLD;

        map_refresh(ACTIVE);
        MAPBOOK->show(ACTIVE);
}


struct mapbook *new_mapbook(void)
{
        struct mapbook *new;
        new = malloc(sizeof(struct mapbook));

        /* Methods */
        new->render  = &map_render;  /* see terrain.c */
        new->restack = &map_restack;
        new->show    = &map_show;
        new->hide    = &map_hide;

        /* World map */
        new->world = new_map(WORLD_HEIGHT, WORLD_WIDTH);
        map_gen(new->world, NULL, MAP_DOSMOOTH);

        /* Field map */
        new->field = map_inset(new->world, FIELD_HEIGHT, FIELD_WIDTH, 0, 0);

        /* Render and restack both */
        new->render(new->world);
        new->restack(new->world);
        new->render(new->field);
        new->restack(new->field);

        /* Set world as the active map */
        new->active = new->world;
        new->field_is_active = false;

        return (new);
}





