// vim:fdm=marker
/*******************************************************************************
    FILENAME:  terrain.c
    Functions for creating landscapes.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>

#include "terrain.h"
#include "weather.h"
#include "map.h"

#include "../lib/stoc/stoc.h"

#include "../eng/model/bytes.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../lib/morton.h"
#include "../test/test.h"
#include "../lib/ufo.h"

#include "../eng/state.h"
#include "../gfx/ui/titlecard.h"

#define BEACH_F 0.021
#define TERRA_F 0.450

#define ARENA_W (COLS/2)
#define ARENA_H (LINES/2)

struct map_t *make_arena(int h, int w, int y, int x)
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
                        if (i > ARENA_H/2 && i < LINES-ARENA_H/2   
                        &&  j > ARENA_W/2 && j < COLS-ARENA_W/2)
                                armap[i][j] = BEACH_F;
                        else
                                armap[i][j] = TERRA_F;
                }
        }
       
        arena = new_map(LINES+1, COLS+1, LINES, COLS, 0, 0);
        gen_map(arena, armap);

        return arena;
}




