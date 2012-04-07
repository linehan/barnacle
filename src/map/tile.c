#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>

#include "terrain.h"
#include "weather.h"
#include "map.h"
#include "../eng/model/bytes.h"
#include "../eng/state.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/titlecard.h"
#include "../lib/morton.h"
#include "../lib/ufo.h"
#include "../lib/stoc/stoc.h"
#include "../test/test.h"


/*
 * Write a blank tile, erasing any previous tiles.
 */
void wipe_tile(struct map_t *map, uint32_t z, int layer)
{
        const short color = 0;
        const attr_t attr = 0;

        mvwp(PLATE(map, layer), TROM(z), L" ", color, attr);
}

/*
 * Place a tile of static ocean
 */
void place_ocean_tile(struct map_t *map, uint32_t z)
{
        const int layer   = BGR;
        const int elev    = 0;
        const short color = SEA_MED;
        const attr_t attr = 0;

        mvwp(PLATE(map, layer), TROM(z), br_dis(0), color, attr); 

        set_state(map->tree, z, 0, LAY, layer);                            
        set_state(map->tree, z, 0, ALT, elev);
}


/*
 * Place a tile of dynamic ocean
 */
void place_swell_tile(struct map_t *map, uint32_t z)
{
        const int layer    = BGR;
        const int elev     = 0;
        const short color  = SEA_MED;
        const attr_t attr = 0;

        mvwp(PLATE(map, layer), TROM(z), br_dis(2), color, attr); 
        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

/*
 * Place a tile of partially-submerged sand 
 */
void place_shoal_tile(struct map_t *map, uint32_t z)
{
        const int layer    = GRO;
        const int elev     = 1;
        const short color  = SEA_MED;
        const attr_t attr1 = 0;
        const attr_t attr2 = A_REVERSE;

        if (flip_biased(0.7))
                mvwp(PLATE(map, layer), TROM(z), br_atl(2), color, attr1);
        else    mvwp(PLATE(map, layer), TROM(z), L"█", color, attr2);

        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

/*
 * Place a tile of beach sand
 */
void place_beach_tile(struct map_t *map, uint32_t z)
{
        const int layer    = GRO;
        const int elev     = 2;
        const short color  = A_SEA_LAGOON;
        const attr_t attr1 = 0;
        const attr_t attr2 = A_REVERSE;

        if (flip_biased(0.3))
                mvwp(PLATE(map, layer), TROM(z), br_dis(1), color, attr1);
        else    mvwp(PLATE(map, layer), TROM(z), L"█", color, attr2);

        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

/*
 * Place whatever "basic" ground tile is currently active
 */
void place_terra_tile(struct map_t *map, uint32_t z)
{
        const int layer   = TOP;
        const int elev    = 4;
        const short color = GRASSY;
        const attr_t attr = 0;

        mvwp(PLATE(map, layer), TROM(z), L"▒", color, attr);
        set_state(map->tree, (z), 0, LAY, layer);
        set_state(map->tree, (z), 0, ALT, elev);
}

/*
 * Place a perspective tile to indicate a drop of elevation
 */
void place_cliff_tile(struct map_t *map, uint32_t z)
{
        const int layer   = DRP;
        const int elev    = 4;
        const short color = LAND;
        const attr_t attr = 0;

        wipe_tile(map, z, TOP);
        mvwp(PLATE(map, layer), TROM(z), L"▓", color, attr);
        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

/*
 * Place a tile corresponding to the "top" of a tree
 */
void place_treetop_tile(struct map_t *map, uint32_t z)
{
        const int layer   = VEG;
        const int elev    = 4;
        const short color = TREETOP;
        const attr_t attr = 0;

        wipe_tile(map, z, TOP);
        mvwp(PLATE(map, layer), TROM(z), L"⬢", color, attr);
        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

/*
 * Place a tile corresponding to the "trunk" of a tree, analagous
 * to the "cliff" of a terra tile
 */
void place_treetrunk_tile(struct map_t *map, uint32_t z)
{
        const int layer   = VEG;
        const int elev    = 4;
        const short color = TREETRUNK;
        const attr_t attr = 0;

        wipe_tile(map, z, TOP);
        mvwp(PLATE(map, layer), TROM(z), L"╹", color, attr);
        set_state(map->tree, z, 0, LAY, layer);
        set_state(map->tree, z, 0, ALT, elev);
}

