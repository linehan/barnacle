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
#include "tile.h"

/* -------------------------------------------------------------------------- */
void map_restack(void *mymap);
void map_render(void *mymap);
void map_show(void *mymap);
void map_hide(void *mymap);
/* -------------------------------------------------------------------------- */



/******************************************************************************
 * Map creator functions
 * 
 * new_map -- creates a new map of given dimension
 * new_inset -- creates a new map which is a scaled-up version of another
 *
 ******************************************************************************/ 

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
        new->mx     = new_matrix(h, w);
        new->mobs   = new_matrix(h, w);
        new->hook   = new_matrix(h, w);
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
 * new_inset -- generates a map which is a scaled-up inset of another map
 * @map: the "parent" map which will be used to generate the inset
 * @h: height of the inset
 * @w: width of the inset
 * @y: y-coordinate of the inset's top-left corner
 * @x: x-coordinate of the inset's top-left corner
 * 
 * Notes
 * Uses a simple linear transformation that goes something like this:
 *      old_range = (old_max - old_min)
 *      new_range = (new_max - new_min)
 *      new_value = (((old_value-old_min)*new_range) / old_range) + new_min
 */
struct map_t *new_inset(struct map_t *map, int h, int w, int y, int x)
{
        struct map_t *new;
        double **val;
        int old_h_range;
        int old_w_range;
        int new_h_range;
        int new_w_range;
        int scaled_h;
        int scaled_w;
        int i;
        int j;

        val = empty_simplex_matrix(LINES, COLS);

        old_h_range = (LINES - 0);
        old_w_range = (COLS - 0);
        new_h_range = (h - 0);
        new_w_range = (w - 0);

        for (i=0; i<LINES; i++) {
        for (j=0; j<COLS; j++) {
                scaled_h = (((i - 0) * new_h_range) / old_h_range) + y;
                scaled_w = (((j - 0) * new_w_range) / old_w_range) + x;

                val[i][j] = map->pmap[scaled_h][scaled_w];
        }
        }

        new = new_map(FULLSCREEN);
        map_gen(new, val, MAP_NOSMOOTH);

        return (new);
}


/*
 * new_mapbook -- allocate and initialize a new mapbook
 */
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
        new->field = new_inset(new->world, FIELD_HEIGHT, FIELD_WIDTH, 0, 0);

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


/******************************************************************************
 * Map generation and raster parsing 
 * 
 * map_gen   -- generate a floating-point matrix of noise value
 * map_label -- process a noise matrix and assign label values to each cell 
 *
 ******************************************************************************/ 

/*
 * map_gen -- fill a new map with generated data that can be parsed
 * @map: pointer to a previously-allocated map
 * @pmap: matrix of doubles used for labeling. If NULL, simplex noise is used
 * @opt: option parameter
 */
void map_gen(struct map_t *map, double **pmap, int opt)
{
        print_status("Generating noise...");

        if (pmap == NULL)
                map->pmap = simplex_matrix(map->ufo.box.h, map->ufo.box.w);
        else    map->pmap = pmap;

        print_status(SUCCESS);

        map_label(map, opt); 
}

/*
 * map_label -- parse the generated noise values and apply labels
 * @map: pointer to a previously-allocated map
 * @opt: some option
 *
 * Notes
 * Perform a detailed series of passes over the map's matrix, and apply a
 * variety of strategies to label grid cells so that they can later be
 * detected by the map_render routine.
 * 
 * The rules that determine a certain labeling are somewhat tedious, and 
 * some minimum amount of fussiness should be expected. Refer to the
 * documentation of the individual subroutines.
 */
void map_label(struct map_t *map, int opt)
{
        if (opt == MAP_DOSMOOTH) {
                print_status("Smoothing noise...");
                smooth_layers(map);
                print_status(SUCCESS);
        }

        print_status("Laying tiles...");
        label_regions(map);
        print_status(SUCCESS);

        print_status("Drawing cliffs...");
        label_cliffs(map);
        print_status(SUCCESS);

        print_status("Planting trees...");
        label_treetops(map);
        print_status(SUCCESS);

        print_status("Trimming trees...");
        label_treetops_trim(map);
        print_status(SUCCESS);

        print_status("Eating breakfast...");
        label_treetrunks(map);
        print_status(SUCCESS);

        print_status("Going swimming...");
        label_shorelines(map);
        print_status(SUCCESS);

        print_status("\n -- Map complete --\n\n");
}



/******************************************************************************
 * Mapbook methods
 * 
 * map_render  -- scan cell labels and write appropriate tiles to screen
 * map_restack -- re-stack (flatten) map layers after changing a lower one
 * map_hide    -- hide the panel associated with a map
 * map_show    -- show the panel associated with a map
 *
 ******************************************************************************/ 

/*
 * map_render (METHOD) -- draw the graphics for all labels in the map matrix
 * @mymap: void * pointer to a previously-allocated map
 *
 * Traverse the map's matrix and simply pass every label encountered
 * to the place_tile routine. Any labels associated with a tile rendering
 * will have that rendering applied to the grid cell at (i,j).
 */
void map_render(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        int i;
        int j;

        for (i=0; i<map->mx->rows; i++) {
        for (j=0; j<map->mx->cols; j++) {
                place_tile(map, i, j, get_nibble(mx_val(map->mx, i, j), LAY));
        }
        }
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
        for (i=0; i<NASSETS; i++) {
                if (map->asset[i])
                        overlay(map->asset[i]->win, PEEK(map->W));
        }
}

/*
 * map_hide (METHOD) -- hide the panel associated with a map
 * @mymap: void * pointer to a previously-allocated map
 */
void map_hide(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        hide_panel(map->pan);
}

/*
 * map_show (METHOD) -- show the panel associated with a map
 * @mymap: void * pointer to a previously-allocated map
 */
void map_show(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        show_panel(map->pan);
}

/*
 * map_swap (METHOD) -- exchange the current active map for the inactive one 
 */
void map_swap(void)
{
        MAPBOOK->hide(ACTIVE);
        MAPBOOK->field_is_active ^= true;

        ACTIVE = (MAPBOOK->field_is_active) ? FIELD : WORLD;

        map_refresh(ACTIVE);
        MAPBOOK->show(ACTIVE);
}


/*
 * map_set_extra -- set a special map to be displayed
 */
void map_set_extra(void *mymap)
{
        EXTRA = (struct map_t *)mymap;

        MAPBOOK->hide(ACTIVE);

        if (EXTRA == NULL) {
                MAPBOOK->extra_is_active = false;
                ACTIVE = (MAPBOOK->field_is_active) ? FIELD : WORLD;
        } else {
                MAPBOOK->extra_is_active = true;
                ACTIVE = EXTRA;
        }

        map_restack(ACTIVE);
        map_refresh(ACTIVE);
        MAPBOOK->show(ACTIVE);
}

/******************************************************************************
 * Map functions 
 * 
 * map_trigger -- detect if an event has been triggered by a mob 
 * map_roll    -- move the map 
 *
 ******************************************************************************/ 

/*
 * map_trigger -- detect if a mob is on a tile that should trigger an event
 * @map: pointer to the map containing the event information
 * @mob: pointer to the mob
 */
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


/*
   map_roll -- move the map around
 * @map: pointer to the map you want to move
 * @dir: 'lrud' directional motion char
 */
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


/*
 * map_hit -- crudely detect collisions
 * @map: pointer to a map structure
 * @rec: the bounding rectangle of the object being tested
 *
 * Notes
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

