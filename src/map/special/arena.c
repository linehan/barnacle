/* 
 * arena.c -- The "arena" style test map.
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
#include <stdio.h>
#include <stdlib.h>
#include "../../com/barnacle.h"
#include "../../gfx/gfx.h"
#include "../../lib/matrix.h"
#include "../terrain.h"
#include "../tile.h"
#include "../map.h"

#include "../../lib/stoc/stoc.h"
#include "../../eng/bytes.h"
#include "../../gfx/ui/titlecard.h"
#include "../../test/test.h"
#include "cave.h"
#include "../door.h"


#define ISLAND_FAT 10

#define BEACH_F  0.021f
#define TERRA_F  0.451f
#define OCEAN_F -0.05f

#define ARENA_W (COLS/2)
#define ARENA_H (LINES/2)

#define ISLAND_H_HI LINES-(ARENA_H/2)+ISLAND_FAT
#define ISLAND_H_LO (ARENA_H/2)-ISLAND_FAT

#define ISLAND_W_HI COLS-(ARENA_W/2)+ISLAND_FAT+3
#define ISLAND_W_LO (ARENA_W/2)-ISLAND_FAT-1

        

struct map_t *map_preset_arena(void)
{
        struct map_t *arena;
        struct map_t *cave;
        double **armap;
        int i;
        int j;

        armap = empty_simplex_matrix(FULLSCREEN);
       
        for (i=0; i<LINES; i++) {
        for (j=0; j<COLS; j++) {
                if (i >= ARENA_H/2 && i <= LINES-ARENA_H/2
                &&  j >= ARENA_W/2 && j <= COLS-ARENA_W/2)
                        armap[i][j] = BEACH_F;
                else if (i > ISLAND_H_LO && i < ISLAND_H_HI 
                &&       j > ISLAND_W_LO-2 && j < ISLAND_W_HI)
                {
                        if (i > ISLAND_H_LO+ISLAND_FAT+5
                        &&  i < ISLAND_H_HI-ISLAND_FAT
                        &&  j < ISLAND_W_LO+ISLAND_FAT+1) {
                                if (j > ISLAND_W_LO+ISLAND_FAT-2)
                                        armap[i][j] = BEACH_F;
                                else
                                        armap[i][j] = -0.05;
                        } else {
                                armap[i][j] = TERRA_F;
                        }
                }
                else
                        armap[i][j] = -0.05;
        }
        }
       
        arena = new_map(FULLSCREEN);
        map_gen(arena, armap, MAP_NOSMOOTH);

        /* Make a cave entrance */
        cave = new_dungeon();
        put_door(CAVE_DOOR, arena, cave, 
                 1, 1, ARENA_H/2-1, COLS/2,
                 1, 1, LINES/2-10, COLS/2);

        cave_connect_door(cave);
        map_render(arena);
        map_render(cave);

        return arena;
}


struct map_t *map_preset_sand(void)
{
        struct map_t *sandmap;
        double **sand;
        int i;
        int j;

        sand = empty_simplex_matrix(LINES, COLS);

        for (i=0; i<LINES; i++) {
        for (j=0; j<COLS; j++) {
                sand[i][j] = BEACH_F;
        }
        }

        sandmap = new_map(FULLSCREEN);
        map_gen(sandmap, sandmap->pmap, MAP_NOSMOOTH);

        /*sandmap->asset[1] = new_asset(0);*/
        /*sandmap->asset[1]->put(sandmap, 1, ((LINES/2)-(LINES/3)), ((COLS/2)-2));*/

        map_render(sandmap);

        return sandmap;
}


