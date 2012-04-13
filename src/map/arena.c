#include <stdio.h>
#include <stdlib.h>

#include "../lib/matrix.h"
#include "terrain.h"
#include "tile.h"
#include "weather.h"
#include "map.h"

#include "../lib/stoc/stoc.h"
#include "../eng/bytes.h"
#include "../gfx/ui/titlecard.h"
#include "../test/test.h"


#define ISLAND_FAT 10


#define BEACH_F  0.021
#define TERRA_F  0.451
#define OCEAN_F -0.05

#define ARENA_W (COLS/2)
#define ARENA_H (LINES/2)

#define ISLAND_H_HI LINES-(ARENA_H/2)+ISLAND_FAT
#define ISLAND_H_LO (ARENA_H/2)-ISLAND_FAT

#define ISLAND_W_HI COLS-(ARENA_W/2)+ISLAND_FAT+3
#define ISLAND_W_LO (ARENA_W/2)-ISLAND_FAT-1


struct map_t *map_preset_arena(void)
{
        struct map_t *arena;
        double **armap;
        int i;
        int j;

        armap = malloc(LINES * sizeof(double *));
        for (i=0; i<LINES; i++)
                armap[i] = malloc(COLS * sizeof(double));
       
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
       
        arena = new_map(LINES+1, COLS+1, LINES, COLS, 0, 0);
        map_gen(arena, armap, MAP_NOSMOOTH);

        /* Make a cave entrance */
        place_cave_label(mx_get(arena->mx, ARENA_H/2-1, COLS/2));
        place_cave_label(mx_get(arena->mx, ARENA_H/2-1, COLS/2-1));
        arena->render(arena);


        return arena;
}




