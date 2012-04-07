#pragma once
#ifndef __TILE_H
#define __TILE_H

#include <stdint.h>
#include "map.h"

void wipe_tile(struct map_t *map, uint32_t z, int layer);
void place_ocean_tile(struct map_t *map, uint32_t z);
void place_swell_tile(struct map_t *map, uint32_t z);
void place_shoal_tile(struct map_t *map, uint32_t z);
void place_beach_tile(struct map_t *map, uint32_t z);
void place_terra_tile(struct map_t *map, uint32_t z);
void place_cliff_tile(struct map_t *map, uint32_t z);
void place_treetop_tile(struct map_t *map, uint32_t z);
void place_treetrunk_tile(struct map_t *map, uint32_t z);

#endif
