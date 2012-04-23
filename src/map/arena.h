#pragma once
#ifndef __ARENA_H
#define __ARENA_H

#include "map.h"

struct map_t *map_preset_arena(void);
struct map_t *map_preset_sand(void);
struct map_t *map_inset(struct map_t *map, int h, int w, int y, int x);

#endif
