#include <stdio.h>
#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/fall.h"
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

#define FULLSCREEN LINES+1, COLS+1, LINES, COLS, 0, 0


#define _SANDHOLE_H 3
#define _SANDHOLE_W 4
short _SANDHOLE_C[]={DSAND,DSAND,DSAND,DSAND,
                     DSAND,ESAND,ESAND,DSAND,
                     CSAND,CSAND,CSAND,CSAND};
wchar_t _SANDHOLE_G[3][4]={L"▛▀▀▜",
                           L"▌▛▜▐",
                           L"▔▔▔▔"};


void put_asset(void *mymap, int idx, int y, int x)
{
        int i;
        int j;
        struct map_t *map = (struct map_t *)mymap;

        map->asset[idx]->y = y;
        map->asset[idx]->x = x;
        show_panel(map->asset[idx]->pan);
        move_panel(map->asset[idx]->pan, y, x);

        for (i=0; i<map->asset[idx]->h-1; i++) {
        for (j=0; j<map->asset[idx]->w; j++) {
                mx_set(map->hook, y+i, x+j, idx);
        }
        }
}


struct asset_t *new_asset(int a)
{
        struct asset_t *new;
        int i;
        int j;
        int idx;

        new = malloc(sizeof(struct asset_t));

        new->h = _SANDHOLE_H;
        new->w = _SANDHOLE_W;
        new->l = _SANDHOLE_H * _SANDHOLE_W;
        new->win = newwin(_SANDHOLE_H, _SANDHOLE_W, 0, 0);
        new->pan = new_panel(new->win);

        new->cch = malloc(new->l * sizeof(cchar_t));

        for (i=0; i<(new->h); i++) {
        for (j=0; j<(new->w); j++) {
                idx = (i*new->w)+j;
                setcchar(&new->cch[idx], &_SANDHOLE_G[i][j], 0, _SANDHOLE_C[idx], NULL);
                mvwadd_wch(new->win, i, j, &new->cch[idx]);
        }
        }

        new->put = &put_asset;
        new->trigger = &fall_screen;

        return (new);
}

        

struct map_t *map_preset_arena(void)
{
        struct map_t *arena;
        double **armap;
        int i;
        int j;

        armap = empty_simplex_matrix(LINES, COLS);
       
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
       
        arena = new_map(LINES, COLS);
        map_gen(arena, armap, MAP_NOSMOOTH);

        /* Make a cave entrance */
        place_cave_label(mx_get(arena->mx, ARENA_H/2-1, COLS/2));
        place_cave_label(mx_get(arena->mx, ARENA_H/2-1, COLS/2-1));
        /*arena->render(arena);*/


        return arena;
}


#define WITHIN_PAD(h,w,y0,x0, H,W,Y0,X0) \
       ((x0 + w >= X0) && (x0 <= X0 + W) && (y0 + h >= Y0) && (y0 <= y0 + H)) ? true : false

#define CENT_Y(h) (LINES-((LINES/2)+(h/2)))
#define CENT_X(w) (COLS-((COLS/2)+(w/2)))

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
        sandmap = new_map(LINES, COLS);
        map_gen(sandmap, sand, MAP_NOSMOOTH);

        sandmap->asset[1] = new_asset(0);
        sandmap->asset[1]->put(sandmap, 1, ((LINES/2)-(LINES/3)), ((COLS/2)-2));

        return sandmap;
}



/*
 * map_inset -- generates a map which is a scaled-up inset of another map
 * @map: the "parent" map which will be used to generate the inset
 * @h: height of the inset
 * @w: width of the inset
 * @y: y-coordinate of the inset's top-left corner
 * @x: x-coordinate of the inset's top-left corner
 * 
 * Notes
 * Uses a simple linear transformation that goes something like this:
 *      old_range = (old_max - old_min)
 *      new_range = (new_max - new_min)
 *      new_value = (((old_value-old_min)*new_range) / old_range) + new_min
 */
struct map_t *map_inset(struct map_t *map, int h, int w, int y, int x)
{
        struct map_t *new;
        double **val;
        int old_h_range;
        int old_w_range;
        int new_h_range;
        int new_w_range;
        int scaled_h;
        int scaled_w;
        int i;
        int j;

        val = empty_simplex_matrix(LINES, COLS);

        old_h_range = (LINES - 0);
        old_w_range = (COLS - 0);
        new_h_range = (h - 0);
        new_w_range = (w - 0);

        for (i=0; i<LINES; i++) {
        for (j=0; j<COLS; j++) {
                scaled_h = (((i - 0) * new_h_range) / old_h_range) + y;
                scaled_w = (((j - 0) * new_w_range) / old_w_range) + x;

                val[i][j] = map->pmap[scaled_h][scaled_w];
        }
        }

        new = new_map(LINES+1, COLS+1);
        map_gen(new, val, MAP_NOSMOOTH);

        return (new);
}
                 



