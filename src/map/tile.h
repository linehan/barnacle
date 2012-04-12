#pragma once
#ifndef __TILE_H
#define __TILE_H

#include <stdint.h>
#include "terrain.h"
#include "weather.h"
#include "map.h"
#include "../eng/bytes.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/titlecard.h"
#include "../lib/stoc/stoc.h"
#include "../test/test.h"

/*
 * Labels identify the tile's rendering
 */

static inline void wipe_label(uint32_t *val)
{
        set_nibble(val, LAY, 0);
        set_nibble(val, ALT, 0);
}
static inline void place_ocean_label(uint32_t *val)
{
        const int layer = BGR;
        const int elev  = 0;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_swell_label(uint32_t *val)
{
        const int layer = BGR;
        const int elev  = 0;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_shoal_label(uint32_t *val)
{
        const int layer = SHO;
        const int elev  = 1;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_beach_label(uint32_t *val)
{
        const int layer = BEA;
        const int elev  = 2;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_terra_label(uint32_t *val)
{
        const int layer = TOP;
        const int elev  = 4;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_cliff_label(uint32_t *val)
{
        const int layer = DRP;
        const int elev  = 4;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_treetop_label(uint32_t *val)
{
        const int layer = TTO;
        const int elev  = 4;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}
static inline void place_treetrunk_label(uint32_t *val)
{
        const int layer = TTR;
        const int elev  = 4;
        set_nibble(val, LAY, layer);                            
        set_nibble(val, ALT, elev);
}

/*
 * Write a blank tile, erasing any previous tiles.
 */
static inline void wipe_tile(struct map_t *map, int y, int x, int layer)
{
        const short color = 0;
        const attr_t attr = 0;
        mvwp(PLATE(map, layer), y, x, L" ", color, attr);
}
/*
 * Place a tile of static ocean
 */
static inline void place_ocean_tile(struct map_t *map, int y, int x)
{
        const int layer = BGR;
        const short color = SEA_MED;
        const attr_t attr = 0;
        mvwp(PLATE(map, layer), y, x, br_dis(0), color, attr); 
}
/*
 * Place a tile of dynamic ocean
 */
static inline void place_swell_tile(struct map_t *map, int y, int x)
{
        const int layer = BGR;
        const short color = SEA_MED;
        const attr_t attr = 0;
        mvwp(PLATE(map, layer), y, x, br_dis(2), color, attr); 
}
/*
 * Place a tile of partially-submerged sand 
 */
static inline void place_shoal_tile(struct map_t *map, int y, int x)
{
        const int layer = SHO;
        const short color  = SEA_MED;
        const attr_t attr1 = 0;
        const attr_t attr2 = A_REVERSE;
        if (flip_biased(0.7))
                mvwp(PLATE(map, layer), y, x, br_atl(2), color, attr1);
        else    
                mvwp(PLATE(map, layer), y, x, L"█", color, attr2);
}
/*
 * Place a tile of beach sand
 */
static inline void place_beach_tile(struct map_t *map, int y, int x)
{
        const int layer = BEA;
        const short color  = A_SEA_LAGOON;
        const attr_t attr1 = 0;
        const attr_t attr2 = A_REVERSE;
        if (flip_biased(0.3))
                mvwp(PLATE(map, layer), y, x, br_dis(1), color, attr1);
        else    
                mvwp(PLATE(map, layer), y, x, L"█", color, attr2);
}
/*
 * Place whatever "basic" ground tile is currently active
 */
static inline void place_terra_tile(struct map_t *map, int y, int x)
{
        const int layer = TOP;
        const short color = GRASSY;
        const attr_t attr = 0;
        mvwp(PLATE(map, layer), y, x, L"▒", color, attr);
}
/*
 * Place a perspective tile to indicate a drop of elevation
 */
static inline void place_cliff_tile(struct map_t *map, int y, int x)
{
        const int layer = DRP;
        const short color = LAND;
        const attr_t attr = 0;
        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"▓", color, attr);
}
/*
 * Place a tile corresponding to the "top" of a tree
 */
static inline void place_treetop_tile(struct map_t *map, int y, int x)
{
        const int layer = TTO;
        const short color = TREETOP;
        const attr_t attr = 0;
        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"⬢", color, attr);
}
/*
 * Place a tile corresponding to the "trunk" of a tree, analagous
 * to the "cliff" of a terra tile
 */
static inline void place_treetrunk_tile(struct map_t *map, int y, int x)
{
        const int layer = TTR;
        const short color = TREETRUNK;
        const attr_t attr = 0;
        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"╹", color, attr);
}

/*
 * Place a tile 
 */
static inline void place_tile(struct map_t *map, int y, int x, int type)
{
        switch (type) {
        case XXX:
                wipe_tile(map, y, x, BGR);
                break;
        case BGR:
                place_ocean_tile(map, y, x);
                break;
        case HIG:
                break;
        case RIM:
                break;
        case DRP:
                place_cliff_tile(map, y, x);
                break;
        case TOP:
                place_terra_tile(map, y, x);
                break;
        case SHO:
                place_shoal_tile(map, y, x);
                break;
        case BEA:
                place_beach_tile(map, y, x);
                break;
        case TTO:
                place_treetop_tile(map, y, x);
                break;
        case TTR:
                place_treetrunk_tile(map, y, x);
                break;
        }
}

#endif
