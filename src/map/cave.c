#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "map.h"
#include "tile.h"

#define LABEL(val,n,...) or_byte((val), LAY, (n), __VA_ARGS__)


inline void place_earth_label(uint32_t *val)
{
        set_byte(val, LAY, RIM);
        set_byte(val, ALT, 10);
}

inline void place_wall_label(uint32_t *val)
{
        set_byte(val, LAY, TOP);
        set_byte(val, ALT, 4);
}
inline void place_floor_label(uint32_t *val)
{
        set_byte(val, LAY, DRP);
        set_byte(val, ALT, 3);
}


void gen_cave(struct map_t *map)
{
        struct cell_t *cell;
        int len = map->ufo.box.w;
        int height;
        int width;
        int y;
        int x;
        int i;
        int j;

        height = map->ufo.box.h-1;
        width  = map->ufo.box.w-1;

        y = 0;
        x = COLS/2;

        cell = new_cell(y,x);

        while (len-->0) 
                cell_walk(&cell, height, width, WLK_VN|WLK_FW|WLK_DRUNK);

        while (cell->parent) {
                place_floor_label(mx_get(map->mx, cell->y, cell->x));
                cell = cell->parent;
        }

        for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
                if (!LABEL((mx_val(map->mx, i, j)), 1, XXX))
                        place_floor_tile(map, i, j);
                else
                        place_earth_tile(map, i, j);
        }
        }
}


struct map_t *new_cave(void)
{
        struct map_t *cave;

        /*print_status("Preparing cave...");*/

        cave = new_map(LINES*3, COLS*3);
        cave->pmap = empty_simplex_matrix(LINES*3, COLS*3);        
        
        gen_cave(cave);

        return (cave);
}


