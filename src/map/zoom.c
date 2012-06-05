/*
 * zoom.c -- scaled-up terrain
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
#include "zoom.h"


/******************************************************************************
 * ZOOMING
 * 
 * "Zooming in" creates a new map which is a transformation of the 
 * currently active one. The new map is pushed to the top of a stack, 
 * the "zoom stack", and becomes the currently active map. 
 *
 * "Zooming out" pops the topmost map of the zoom stack, and discards
 * it, unless it is the last map in the stack. The new topmost map
 * on the zoom stack becomes the currently active map.
 ******************************************************************************/
#define h_scale_factor (3)
#define w_scale_factor (6)
#define h_screen       (LINES)
#define w_screen       (COLS)
#define zoomed_y_step  (h_screen / h_scale_factor)
#define zoomed_x_step  (w_screen / w_scale_factor)

/**
 * gen_zoom -- generates a map which is a scaled-up rendering of another map
 * @map: the "parent" map which will be used to generate the inset
 * 
 * Note
 * Uses a simple linear transformation that goes something like this:
 *      old_range = (old_max - old_min)
 *      new_range = (new_max - new_min)
 *      new_value = (((old_value-old_min)*new_range) / old_range) + new_min
 */
struct map_t *gen_zoom(struct map_t *map)
{
        struct map_t *new;
        double **val;
        int big_h;
        int big_w;
        int scaled_h;
        int scaled_w;
        int i;
        int j;

        big_h = pos_boxh(map->pos) * h_scale_factor;
        big_w = pos_boxw(map->pos) * w_scale_factor;

        val = empty_simplex_matrix(big_h, big_w);

        for (i=0; i<big_h; i++) {
        for (j=0; j<big_w; j++) {
                scaled_h = (((i) * h_scale_factor) / h_screen);
                scaled_w = (((j) * w_scale_factor) / w_screen);

                val[i][j] = map->pmap[scaled_h][scaled_w];
        }
        }

        new = new_map(big_h, big_w);
        map_gen(new, val, MAP_NOSMOOTH);

        map_render(new);

        return (new);
}


inline void world_to_zoom(int y_world, int x_world, int *y_zoom, int *x_zoom)
{
        *y_zoom = (y_world * zoomed_y_step);
        *x_zoom = (x_world * zoomed_x_step);
}


void set_zoom(struct mapbook_t *mapbook, int y, int x)
{
        int y_zoom;
        int x_zoom;

        world_to_zoom(y, x, &y_zoom, &x_zoom);

        pos_setyx(MAPBOOK->map[ZOOM]->pos, y_zoom, x_zoom);

        MAPBOOK->hide(MAPBOOK->active);
        MAPBOOK->active = MAPBOOK->map[ZOOM];
        MAPBOOK->show(MAPBOOK->active);
}








