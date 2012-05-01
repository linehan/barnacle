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

static wchar_t base[]=L"⠀";
cchar_t lobase;
cchar_t hibase;

static inline void init_tiles(void)
{
        #define lobase_pair SEA_MED
        #define hibase_pair SEA__MED

        setcchar(&lobase, base, 0, lobase_pair, NULL);
        setcchar(&hibase, base, 0, hibase_pair, NULL);
}


static inline void wipe_label(uint32_t *val)
{
        set_byte(val, LAY, 0);
        set_byte(val, ALT, 0);
}
static inline void place_ocean_label(uint32_t *val)
{
        set_byte(val, LAY, OCN);                            
        set_byte(val, ALT, 0);
}
static inline void place_swell_label(uint32_t *val)
{
        set_byte(val, LAY, OCN);                            
        set_byte(val, ALT, 0);
}
static inline void place_shoal_label(uint32_t *val)
{
        set_byte(val, LAY, SHO);                            
        set_byte(val, ALT, 1);
}
static inline void place_beach_label(uint32_t *val)
{
        set_byte(val, LAY, BEA);                            
        set_byte(val, ALT, 2);
}
static inline void place_terra_label(uint32_t *val)
{
        set_byte(val, LAY, TOP);                            
        set_byte(val, ALT, 4);
}
static inline void place_cliff_label(uint32_t *val)
{
        set_byte(val, LAY, DRP);                            
        set_byte(val, ALT, 4);
}
static inline void place_treetop_label(uint32_t *val)
{
        set_byte(val, LAY, TTO);                            
        set_byte(val, ALT, 4);
}
static inline void place_treetrunk_label(uint32_t *val)
{
        set_byte(val, LAY, TTR);                            
        set_byte(val, ALT, 4);
}

#define layer BGR
/*
 * Write a blank tile, erasing any previous tiles.
 */
static inline void wipe_tile(struct map_t *map, int y, int x, int layer)
{
        mvwp(PLATE(map, layer), y, x, L" ", 0, 0);
}
/*
 * Place a tile of static ocean
 */
static inline void place_ocean_tile(struct map_t *map, int y, int x)
{
        //#define layer BGR 

        mvwadd_wch(PLATE(map, layer), y, x, &lobase);

        //#undef layer
}
/*
 * Place a tile of dynamic ocean
 */
static inline void place_swell_tile(struct map_t *map, int y, int x)
{
        //#define layer BGR 

        mvwadd_wch(PLATE(map, layer), y, x, &hibase);

        //#undef layer
}
/*
 * Place a tile of partially-submerged sand 
 */
static inline void place_shoal_tile(struct map_t *map, int y, int x)
{
        //#define layer SHO
        #define color SEA_MED
        #define attr  0

        if (flip_biased(0.7))
                mvwp(PLATE(map, layer), y, x, br_atl(2), color, attr);

        //#undef layer
        #undef color
        #undef attr
}
/*
 * Place a tile of beach sand
 */
static inline void place_beach_tile(struct map_t *map, int y, int x)
{
        //#define layer BEA
        #define color A_SEA_LAGOON
        #define attr1 0
        #define attr2 A_REVERSE

        if (flip_biased(0.3))
                mvwp(PLATE(map, layer), y, x, br_dis(1), color, attr1);
        else    
                mvwp(PLATE(map, layer), y, x, L"█", color, attr2);

        //#undef layer
        #undef color
        #undef attr1
        #undef attr2
}
/*
 * Place whatever "basic" ground tile is currently active
 */
static inline void place_terra_tile(struct map_t *map, int y, int x)
{
        //#define layer TOP
        #define color GRASSY
        #define attr 0

        mvwp(PLATE(map, layer), y, x, L"▒", color, attr);

        //#undef layer
        #undef color
        #undef attr
}
/*
 * Place a perspective tile to indicate a drop of elevation
 */
static inline void place_cliff_tile(struct map_t *map, int y, int x)
{
        //#define layer DRP
        #define color LAND
        #define attr 0

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"▓", color, attr);

        //#undef layer
        #undef color
        #undef attr
}
/*
 * Place a tile corresponding to the "top" of a tree
 */
static inline void place_treetop_tile(struct map_t *map, int y, int x)
{
        //#define layer TTO
        #define color TREETOP
        #define attr 0

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"⬢", color, attr);

        //#undef layer
        #undef color
        #undef attr
}
/*
 * Place a tile corresponding to the "trunk" of a tree, analagous
 * to the "cliff" of a terra tile
 */
static inline void place_treetrunk_tile(struct map_t *map, int y, int x)
{
        //#define layer TTR
        #define color TREETRUNK 
        #define attr 0

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"╹", color, attr);

        //#undef layer
        #undef color
        #undef attr
}


/*
 * Place a tile for a cave floor 
 */
static inline void place_floor_tile(struct map_t *map, int y, int x)
{
        //#define layer TOP 
        #define color PUR_GREY 
        #define attr 0

        mvwp(PLATE(map, layer), y, x, L"█", color, attr);

        //#undef layer
        #undef color
        #undef attr
}
/*
 * Place a tile for a cave wall 
 */
static inline void place_wall_tile(struct map_t *map, int y, int x)
{
        //#define layer TOP 
        #define color PUR_GREY 
        #define attr 0

        mvwp(PLATE(map, layer), y, x, L"▒", color, attr);

        //#undef layer
        #undef color
        #undef attr
}
static inline void place_earth_tile(struct map_t *map, int y, int x)
{
        //#define layer TTO
        #define color PUR_PURPLE 
        #define attr A_REVERSE 

        mvwp(PLATE(map, layer), y, x, L"█", color, attr);

        //#undef layer
        #undef color
        #undef attr
}


static inline void place_cavedoor_tile(struct map_t *map, int y, int x)
{
        //#define layer DOOR_CAVE
        #define color PUR_BROWN 
        #define attr  0

        mvwp(PLATE(map, layer), y, x, L"▔", color, attr);

        //#undef layer
        #undef color
        #undef attr
}



/*
 * Place a tile 
 */
static inline void place_tile(struct map_t *map, int y, int x, int type)
{
        switch (type) {
        case XXX:
                wipe_tile(map, y, x, OCN);
                break;
        case OCN:
                place_ocean_tile(map, y, x);
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
        case DOR:
                place_door_tile(map, y, x);
                break;
        }
}

#endif
