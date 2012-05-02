#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "map.h"
#include "tile.h"


void gen_cave(struct map_t *map)
{
        struct cell_t *cell;
        int len = 200;
        int height;
        int width;
        int y;
        int x;
        int i;
        int j;

        height = map->ufo.box.h-1;
        width  = map->ufo.box.w-1;

        y = LINES;
        x = COLS/2;

        cell = new_cell(y,x);

        while (len-->0) 
                cell_walk(&cell, height, width, WLK_VN|WLK_FW|WLK_DRUNK);

        while (cell->parent) {
                place_cavefloor_label(mx_get(map->tile, cell->y, cell->x));
                cell = cell->parent;
        }

        for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
                if (TILE(map, i, j) == CAVEFLOOR)
                        place_cavefloor_tile(map, i, j);
                else
                        place_cavesolid_tile(map, i, j);
        }
        }
}


struct map_t *new_cave(void)
{
        struct map_t *cave;

        cave = new_map(FULLSCREEN);
        cave->pmap = empty_simplex_matrix(LINES, COLS);        
        
        gen_cave(cave);

        return (cave);
}


