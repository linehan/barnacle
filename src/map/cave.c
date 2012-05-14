#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "map.h"
#include "tile.h"


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

        x_start = w/4;
        x_final = x_start * 3;

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



struct map_t *new_cave(void)
{
        struct map_t *cave;

        cave = new_map(FULLSCREEN);
        cave->pmap = simplex_matrix(FULLSCREEN);        
        
        /*gen_cave(cave);*/
        gen_cavern(cave);

        return (cave);
}


