/*
 * map.c -- All that terrain
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "../gfx/gfx.h"
#include "../gfx/ui/titlecard.h"
#include "../lib/stoc/stoc.h"
#include "../test/test.h"
#include "../lib/list.h"
#include "map.h"
#include "control.h"
#include "terrain.h"
#include "tile.h"



void map_restack(void *mymap);
void map_render(void *mymap);
void map_show(void *mymap);
void map_hide(void *mymap);



/******************************************************************************
 * Map creator functions
 * 
 * new_map -- creates a new map of given dimension
 * new_inset -- creates a new map which is a scaled-up version of another
 *
 ******************************************************************************/ 

/**
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
       
        new = malloc(sizeof(struct map_t));

        /* Build some stuff */
        new->id     = mt_random();
        new->tag    = MAP2DF;
        new->tile   = new_matrix(h, w);
        new->mobs   = new_matrix(h, w);
        new->door   = new_matrix(h, w);
        new->item   = new_matrix(h, w);
        new->pos    = new_pos(SCR_HEIGHT, SCR_WIDTH, SCR_Y0, SCR_X0, h, w, 0, 0);

        /* Build windows, pads, and panels */
        new->win    = newwin(LINES, COLS, 0, 0); /* Fullscreen */
        new->pan    = new_panel(new->win);
        new->L[RIM] = new_multiwin(h, w, 0, 0, 4);
        new->L[BGR] = new_multiwin(h, w, 0, 0, 1);
        new->W      = new_multiwin(h, w, 0, 0, 2);

        return (new);
}



/**
 * new_mapbook -- allocate and initialize a new mapbook
 */
struct mapbook_t *new_mapbook(void)
{
        struct mapbook_t *new;
        new = malloc(sizeof(struct mapbook_t));

        /* Methods */
        new->render  = &map_render;  /* see terrain.c */
        new->restack = &map_restack;
        new->show    = &map_show;
        new->hide    = &map_hide;

        /* Make the world map */
        new->map[WORLD] = new_map(WORLD_HEIGHT, WORLD_WIDTH);
        map_gen(new->map[WORLD], NULL, MAP_DOSMOOTH);

        /* Make the zoom map */
        new->map[ZOOM] = gen_zoom(new->map[WORLD]);

        new->render(new->map[WORLD]);
        new->restack(new->map[WORLD]);

        /* Set world as the active map */
        new->active = new->map[WORLD];

        return (new);
}



/******************************************************************************
 * Map generation and raster parsing 
 * 
 * map_gen   -- generate a floating-point matrix of noise value
 * map_label -- process a noise matrix and assign label values to each cell 
 *
 ******************************************************************************/ 

/**
 * map_gen -- fill a new map with generated data that can be parsed
 * @map: pointer to a previously-allocated map
 * @pmap: matrix of doubles used for labeling. If NULL, simplex noise is used
 * @opt: option parameter
 */
void map_gen(struct map_t *map, double **pmap, int opt)
{
        print_status("Generating noise...");

        if (pmap == NULL)
                map->pmap = simplex_matrix(pos_boxh(map->pos), pos_boxw(map->pos));
        else    map->pmap = pmap;

        print_status(SUCCESS);

        map_label(map, opt); 
}

/**
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

        print_status("Trimming canopy...");
        label_treetops_trim(map);
        print_status(SUCCESS);

        print_status("Labeling lumber...");
        label_treetrunks(map);
        print_status(SUCCESS);

        print_status("Combing beaches...");
        label_beaches(map);
        print_status(SUCCESS);

        print_status("Lining shores...");
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

/**
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

        for (i=0; i<pos_boxh(map->pos); i++) {
        for (j=0; j<pos_boxw(map->pos); j++) {
                place_tile(map, i, j, get_byte(mx_val(map->tile, i, j), LAB));
        }
        }
}


/**
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

        overlay(PEEK(map->L[BGR]), PEEK(map->W));
        overlay(PEEK(map->L[RIM]), PEEK(map->W));
}

/**
 * map_hide (METHOD) -- hide the panel associated with a map
 * @mymap: void * pointer to a previously-allocated map
 */
void map_hide(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        hide_panel(map->pan);
}

/**
 * map_show (METHOD) -- show the panel associated with a map
 * @mymap: void * pointer to a previously-allocated map
 */
void map_show(void *mymap)
{
        struct map_t *map = (struct map_t *)mymap;
        show_panel(map->pan);
        map_restack(ACTIVE);
        map_refresh(ACTIVE);
}

/**
 * map_cycle -- cycle through pages of the mapbook
 */
void map_cycle(void)
{
        switch (MAPBOOK->page) {
        case EXTRA:
                MAPBOOK->page = WORLD;
                break;
        case WORLD:
                MAPBOOK->page = ZOOM;
                break;
        case ZOOM:
                MAPBOOK->page = EXTRA;
                break;
        }

        MAPBOOK->hide(ACTIVE);
        if (MAPBOOK->map[MAPBOOK->page])
                MAPBOOK->active = MAPBOOK->map[MAPBOOK->page];
        MAPBOOK->show(ACTIVE);
}


/**
 * map_set_extra -- set a special map to be displayed
 */
void map_set(enum map_page pg, void *mymap)
{
        MAPBOOK->map[pg] = (struct map_t *)mymap;
        MAPBOOK->page = pg;

        MAPBOOK->hide(ACTIVE);
        MAPBOOK->active = MAPBOOK->map[pg];
        MAPBOOK->show(ACTIVE);
}




/**
 * map_hit -- crudely detect collisions
 * @map: pointer to a map structure
 * @rec: the bounding rectangle of the object being tested
 *
 * Notes
 * Returns 1 (true) if collision detected, otherwise returns 0 (false).
 */
bool map_hit(struct map_t *map, struct noun_t *noun)
{
        int i, j;
        int y, x;
        int h, w;

        y = pos_y(noun->pos);
        x = pos_x(noun->pos);
        h = pos_h(noun->pos);
        w = pos_w(noun->pos);

        for (i=y; i<(y + h); i++) {
        for (j=x; j<(x + w); j++) {
                if (LABEL(mx_get(map->tile, i, j), DRP,TTO,CAVEWALL,CAVESOLID)) 
                        return true;
        }
        }
        return false;
}


bool mob_hit(struct map_t *map, struct noun_t *noun)
{
        int y;
        int x;
        uint32_t val;
       
        y = pos_y(noun->pos);
        x = pos_x(noun->pos);

        val = mx_val(map->mobs, y, x);

        return (val != 0 && val != noun->id) ? true : false;
}


