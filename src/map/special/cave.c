/* 
 * cave.c -- Crufty cave generation.
 * "Crufty cave" had been considered as a name for this program.
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
#include "../../com/barnacle.h"
#include "../../lib/matrix.h"
#include "../../lib/stoc/stoc.h"
#include "../cell.h"
#include "../map.h"
#include "../tile.h"


void gen_cave_passage(struct map_t *map)
{
        struct cell_t *cell;
        int len = 200;
        int height;
        int width;
        int y;
        int x;
        int i;
        int j;

        height = pos_boxh(map->pos) - 1;
        width  = pos_boxw(map->pos) - 1;

        y = LINES;
        x = COLS/2;

        cell = new_cell(y,x);

        while (len-->0) 
                cell_walk(&cell, height, width, WLK_VN|WLK_FW|WLK_DRUNK);

        for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
                place_cavesolid_label(mx_get(map->tile, i, j));
        }
        }

        while (cell->parent) {
                place_cavefloor_label(mx_get(map->tile, cell->y, cell->x));
                cell = cell->parent;
        }
}


void cave_connect_door(struct map_t *map)
{
        struct cell_t *cell;
        struct cell_t **nbr;
        int h;
        int w;
        int i;
        int j;

        h = pos_boxh(map->pos) - 1;
        w = pos_boxw(map->pos) - 1;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
                if (DOOR(map, i, j)) {
                        cell = new_cell(i,j);
                }
        }
        }

        nbr = mk_neighbors(cell, h, w, WLK_MO);

        for (i=0; i<NUM_MOORE; i++) {
                SET_TILE(map, nbr[i]->y, nbr[i]->x, CAVEFLOOR);
        }
}


void gen_cavern(struct map_t *map)
{
        #define CAVERN -0.05f
        #define OCTAVES 6      /* Number of octaves to smooth */
        #define PERSIST 0.99f  /* Persistence factor */
        int h;
        int w;
        int x_start;
        int x_final;
        int i;
        int j;

        h = pos_boxh(map->pos) - 1;
        w = pos_boxw(map->pos) - 1;

        x_start = 0;
        x_final = COLS;

        perlin_smooth(map->pmap, h, w, PERSIST, OCTAVES);
        smooth_cycle(map->pmap, h, w, CAVERN, SMOOTH_BO, 4);

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
                if (j > x_start && j < x_final && map->pmap[i][j] > CAVERN)
                        place_cavefloor_label(mx_get(map->tile, i, j));
                else
                        place_cavesolid_label(mx_get(map->tile, i, j));
        }
        }
}

void special_cave(struct map_t *map)
{
        #define CAVERN -0.05f
        #define OCTAVES 6      /* Number of octaves to smooth */
        #define PERSIST 0.99f  /* Persistence factor */
        int h;
        int w;
        int x_start;
        int x_final;
        int y_start;
        int y_final;
        int platform_w;
        int platform_h;
        int i;
        int j;

        platform_w = LINES;
        platform_h = 4;

        x_start = ((COLS/2) - (platform_w/2));
        x_final = (x_start + platform_w);
        y_start = ((LINES/2) - 9);
        y_final = ((y_start + platform_h));

        h = LINES;
        w = COLS;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
                if (i > y_start && i < y_final && j > x_start && j < x_final)
                        place_cavesolid_label(mx_get(map->tile, i, j));
                else
                        place_cavefloor_label(mx_get(map->tile, i, j));
        }
        }
}


struct map_t *new_cave(void)
{
        struct map_t *cave;

        cave = new_map(FULLSCREEN);
        cave->pmap = simplex_matrix(FULLSCREEN);        

        cave->tag = MAP_CAVE;
        
        /*special_cave(cave);*/
        gen_cavern(cave);

        return (cave);
}


