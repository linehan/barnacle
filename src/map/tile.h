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
        set_byte(val, LAB, 0);
        set_byte(val, ALT, 0);
}
static inline void place_ocean_label(uint32_t *val)
{
        set_byte(val, LAB, OCN);                            
        set_byte(val, ALT, 0);
}
static inline void place_swell_label(uint32_t *val)
{
        set_byte(val, LAB, OCN);                            
        set_byte(val, ALT, 0);
}
static inline void place_shoal_label(uint32_t *val)
{
        set_byte(val, LAB, SHO);                            
        set_byte(val, ALT, 1);
}
static inline void place_beach_label(uint32_t *val)
{
        set_byte(val, LAB, BEA);                            
        set_byte(val, ALT, 2);
}
static inline void place_terra_label(uint32_t *val)
{
        set_byte(val, LAB, TOP);                            
        set_byte(val, ALT, 4);
}
static inline void place_cliff_label(uint32_t *val)
{
        set_byte(val, LAB, DRP);                            
        set_byte(val, ALT, 4);
}
static inline void place_treetop_label(uint32_t *val)
{
        set_byte(val, LAB, TTO);                            
        set_byte(val, ALT, 4);
}
static inline void place_treetrunk_label(uint32_t *val)
{
        set_byte(val, LAB, TTR);                            
        set_byte(val, ALT, 4);
}
static inline void place_cavesolid_label(uint32_t *val)
{
        set_byte(val, LAB, CAVESOLID);
        set_byte(val, ALT, 10);
}
static inline void place_cavewall_label(uint32_t *val)
{
        set_byte(val, LAB, CAVEWALL);
        set_byte(val, ALT, 4);
}
static inline void place_cavefloor_label(uint32_t *val)
{
        set_byte(val, LAB, CAVEFLOOR);
        set_byte(val, ALT, 3);
}
static inline void place_rope_label(uint32_t *val)
{
        set_byte(val, LAB, ROPE);
        set_byte(val, ALT, 3);
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
        mvwadd_wch(PLATE(map, layer), y, x, &lobase);
}
/*
 * Place a tile of dynamic ocean
 */
static inline void place_swell_tile(struct map_t *map, int y, int x)
{
        mvwadd_wch(PLATE(map, layer), y, x, &hibase);
}
/*
 * Place a tile of partially-submerged sand 
 */
static inline void place_shoal_tile(struct map_t *map, int y, int x)
{
        #define color SEA_MED
        #define attr  0

        if (flip_biased(0.7))
                mvwp(PLATE(map, layer), y, x, br_atl(2), color, attr);

        #undef color
        #undef attr
}
/*
 * Place a tile of beach sand
 */
static inline void place_beach_tile(struct map_t *map, int y, int x)
{
        #define color A_SEA_LAGOON
        #define attr1 0
        #define attr2 A_REVERSE

        if (flip_biased(0.3))
                mvwp(PLATE(map, layer), y, x, br_dis(1), color, attr1);
        else    
                mvwp(PLATE(map, layer), y, x, L"█", color, attr2);

        #undef color
        #undef attr1
        #undef attr2
}
/*
 * Place whatever "basic" ground tile is currently active
 */
static inline void place_terra_tile(struct map_t *map, int y, int x)
{
        #define color GRASSY
        #define attr 0

        mvwp(PLATE(map, layer), y, x, L"▒", color, attr);

        #undef color
        #undef attr
}
/*
 * Place a perspective tile to indicate a drop of elevation
 */
static inline void place_cliff_tile(struct map_t *map, int y, int x)
{
        #define color LAND
        #define attr 0 

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"▓", color, attr);

        #undef color
        #undef attr
}
/*
 * Place a tile corresponding to the "top" of a tree
 */
static inline void place_treetop_tile(struct map_t *map, int y, int x)
{
        #define color TREETOP
        #define attr 0

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"⬢", color, attr);

        #undef color
        #undef attr
}
/*
 * Place a tile corresponding to the "trunk" of a tree, analagous
 * to the "cliff" of a terra tile
 */
static inline void place_treetrunk_tile(struct map_t *map, int y, int x)
{
        #define color TREETRUNK 
        #define attr 0

        wipe_tile(map, y, x, TOP);
        mvwp(PLATE(map, layer), y, x, L"╹", color, attr);

        #undef color
        #undef attr
}
/*
 * Place a tile for a cave floor 
 */
static inline void place_cavesolid_tile(struct map_t *map, int y, int x)
{
        #define color PUR_BLACK 
        #define attr A_REVERSE 

        mvwp(PLATE(map, layer), y, x, L"ᚡ", color, attr);

        #undef color
        #undef attr
}
/*
 * Place a tile for a cave wall 
 */
static inline void place_cavewall_tile(struct map_t *map, int y, int x)
{
        #define color PUR_GREY 
        #define attr 0

        mvwp(PLATE(map, layer), y, x, L"ᚡ", color, attr);

        #undef color
        #undef attr
}
static inline void place_cavefloor_tile(struct map_t *map, int y, int x)
{
        #define color PUR_SOLID
        #define attr A_REVERSE 

        mvwp(PLATE(map, layer), y, x, L" ", color, attr);

        #undef color
        #undef attr
}
static inline void place_caverubble_tile(struct map_t *map, int y, int x)
{
        #define color PUR_BLACK 
        #define attr A_REVERSE 

        //L"᙮", L"ι", L"⸒"

        mvwp(PLATE(map, layer), y, x, L"⸒", color, attr);

        #undef color
        #undef attr
}
static inline void place_cavedoor_tile(struct map_t *map, int y, int x)
{
        #define color PUR_BROWN 
        #define attr  A_REVERSE

        mvwp(PLATE(map, layer), y, x, L"⬣", color, attr);

        #undef color
        #undef attr
}
static inline void place_l_rope_anchor_tile(struct map_t *map, int y, int x)
{
        #define color BLACK_BEIGE 
        #define attr  0 

        mvwp(PLATE(map, layer), y, x, L"⸜", color, attr);

        #undef color
        #undef attr
}
static inline void place_r_rope_anchor_tile(struct map_t *map, int y, int x)
{
        #define color BLACK_BEIGE 
        #define attr  0 

        mvwp(PLATE(map, layer), y, x, L"⸝", color, attr);

        #undef color
        #undef attr
}
static inline void place_rope_tile(struct map_t *map, int y, int x)
{
        #define color BLACK_BEIGE 
        #define attr  0 

        mvwp(PLATE(map, layer), y, x, L"┆", color, attr);

        #undef color
        #undef attr
}

//ỺỻꚐꚑ⏓⏚⍊⍽⎵⸜⸝


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
        case CAVEFLOOR:
                place_cavefloor_tile(map, y, x);
                break;
        case CAVEWALL:
                place_cavewall_tile(map, y, x);
                break;
        case CAVESOLID:
                place_cavesolid_tile(map, y, x);
                break;
        case CAVERUBBLE:
                place_caverubble_tile(map, y, x);
                break;
        case ROPE:
                place_rope_tile(map, y, x);
                break;
        case L_ROPE_ANCHOR:
                place_l_rope_anchor_tile(map, y, x);
                break;
        case R_ROPE_ANCHOR:
                place_r_rope_anchor_tile(map, y, x);
                break;
        }
}

#endif
