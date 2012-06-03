/*
 * inset.c -- generate zoomed insets of a map
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
#include "../com/barnacle.h"
#include "../lib/stoc/stoc.h"
#include "../test/test.h"
#include "../gfx/ui/cursor.h"
#include "map.h"
#include "inset.h"
#include "terrain.h"
#include "tile.h"


/*
 * new_inset -- generates a map which is a scaled-up inset of another map
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
struct map_t *new_inset(struct map_t *map, int h, int w, int y, int x)
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

        val = empty_simplex_matrix(FULLSCREEN);

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

        new = new_map(FULLSCREEN);
        map_gen(new, val, MAP_NOSMOOTH);

        return (new);
}


/**
 * zoom_in -- draw an inset for the next zoom level of the screen
 */
/*void zoom_in(struct mapbook_t *mapbook, int y, int x)*/
/*{*/
        /*struct map_t *newzoom;*/

        /*newzoom = new_inset(mapbook->active, 6, 8, y, x);*/

        /*list_add(&mapbook->zoom, &newzoom->node);*/
/*}*/




/**
 * inset_cursor -- draw an inset cursor on the screen and allow movement
 */
int inset_cursor(int ch)
{
        #define frame_w 6 
        #define frame_h 3
        static WINDOW *frame_win;
        static PANEL  *frame_pan;
        static int y;
        static int x;

        if (!frame_win) {
                frame_win = newwin(frame_h, frame_w, 0, 0);
                frame_pan = new_panel(frame_win);
        }

        switch (ch) 
        {
        case MODE_STARTED:
                show_panel(frame_pan);
                break;
        case 'h':
                move_cursor(frame_pan, &y, &x, 'l', 1);
                break;
        case 'H':
                move_cursor(frame_pan, &y, &x, 'l', 4);
                break;
        case 'l':
                move_cursor(frame_pan, &y, &x, 'r', 1);
                break;
        case 'L':
                move_cursor(frame_pan, &y, &x, 'r', 4);
                break;
        case 'k':
                move_cursor(frame_pan, &y, &x, 'u', 1);
                break;
        case 'K':
                move_cursor(frame_pan, &y, &x, 'u', 4);
                break;
        case 'j':
                move_cursor(frame_pan, &y, &x, 'd', 1);
                break;
        case 'J':
                move_cursor(frame_pan, &y, &x, 'd', 4);
                break;
        case '?':
        case '\n':
        case KEY_ESC:
                FIELD = new_inset(ACTIVE, frame_h, frame_w, y, x);
                MAPBOOK->render(FIELD);
                MAPBOOK->restack(FIELD);
                ACTIVE = FIELD;
                MAPBOOK->page = MAP_FIELD;
                hide_panel(frame_pan);
                return MODE_RELEASE;
        }

        /*------------------------------------------------------------*/
        copywin(PLATE(ACTIVE, BGR), frame_win, y, x, 0, 0, frame_h-1, frame_w-1, 0);
        copywin(PLATE(ACTIVE, RIM), frame_win, y, x, 0, 0, frame_h-1, frame_w-1, 1);
        /*------------------------------------------------------------*/
        take_bgcolor_yx(frame_win , 0         , 0         , INSET_UL);
        take_bgcolor_yx(frame_win , 0         , frame_w-1 , INSET_UR);
        take_bgcolor_yx(frame_win , frame_h-1 , 0         , INSET_DL);
        take_bgcolor_yx(frame_win , frame_h-1 , frame_w-1 , INSET_DR);
        /*------------------------------------------------------------*/
        mvwcch(frame_win , 0         , 0         , L"▛" , 0 , INSET_UL);
        mvwcch(frame_win , 0         , frame_w-1 , L"▜" , 0 , INSET_UR);
        mvwcch(frame_win , frame_h-1 , 0         , L"▙" , 0 , INSET_DL);
        mvwcch(frame_win , frame_h-1 , frame_w-1 , L"▟" , 0 , INSET_DR);
        /*------------------------------------------------------------*/

        return MODE_PERSIST;
}
        

