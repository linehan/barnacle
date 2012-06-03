/*
 * map/control.c -- pan and zoom over the world map, track location
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
#include "control.h"
#include "terrain.h"
#include "tile.h"

/******************************************************************************
 * ZOOMING
 * 
 * "Zooming in" creates a new map which is a transformation of the 
 * currently active one. The new map is pushed to the top of a stack, 
 * the "zoom stack", and becomes the currently active map. 
 *
 * "Zooming out" pops the topmost map of the zoom stack, and discards
 * it, unless it is the last map in the stack. The new topmost map
 * on the zoom stack becomes the currently active map.
 ******************************************************************************/

/**
 * new_zoom -- generates a map which is a scaled-up rendering of another map
 * @map: the "parent" map which will be used to generate the inset
 * @h: height of the inset
 * @w: width of the inset
 * @y: y-coordinate of the inset's top-left corner
 * @x: x-coordinate of the inset's top-left corner
 * 
 * Note
 * Uses a simple linear transformation that goes something like this:
 *      old_range = (old_max - old_min)
 *      new_range = (new_max - new_min)
 *      new_value = (((old_value-old_min)*new_range) / old_range) + new_min
 */
struct map_t *new_zoom(struct map_t *map, int y, int x)
{
        #define h_scale_factor 6
        #define w_scale_factor 8
        #define h_screen LINES
        #define w_screen COLS
        struct map_t *new;
        double **val;
        int scaled_h;
        int scaled_w;
        int i;
        int j;

        val = empty_simplex_matrix(FULLSCREEN);

        for (i=0; i<h_screen; i++) {
        for (j=0; j<w_screen; j++) {
                scaled_h = (((i - 0) * h_scale_factor) / h_screen) + y;
                scaled_w = (((j - 0) * w_scale_factor) / w_screen) + x;

                val[i][j] = map->pmap[scaled_h][scaled_w];
        }
        }

        new = new_map(FULLSCREEN);
        map_gen(new, val, MAP_NOSMOOTH);

        return (new);
}


/**
 * current_zoom_map -- return the map of the current zoom level
 * @mapbook: pointer to the mapbook structure
 */
struct map_t *current_zoom_map(struct mapbook_t *mapbook)
{
        struct map_t *tmp;

        tmp = list_top(&mapbook->zoom, struct map_t, node);

        return tmp ? tmp : NULL;
}


/**
 * zoom_in -- draw an inset for the next zoom level of the screen
 * @mapbook: pointer to the mapbook structure
 * @y      : y0 coordinate of zoom frame
 * @x      : x0 coordinate of zoom frame
 */
void zoom_in(struct mapbook_t *mapbook, int y, int x)
{
        struct map_t *newzoom;

        /* Create an inset of the currently active map
         * in the mapbook. */
        newzoom = new_zoom(mapbook->active, y, x);

        /* If this is the base zoom level, the currently
         * active map becomes the root node. */
        if (list_empty(&mapbook->zoom))
                list_add(&mapbook->zoom, &mapbook->active->node);

        /* Push the new inset to the zoom stack */
        list_add(&mapbook->zoom, &newzoom->node);

        /* Set it as the active map */
        mapbook->active = current_zoom_map(mapbook);
        mapbook->render(mapbook->active);
        mapbook->restack(mapbook->active);
        mapbook->show(mapbook->active);
}


/**
 * zoom_out -- remove an inset from the zoom list
 * @mapbook: pointer to the mapbook structure
 */
void zoom_out(struct mapbook_t *mapbook)
{
        struct map_t *top;

        /* If this is the base zoom level, we can't zoom 
         * out any more. */
        if (list_empty(&mapbook->zoom))
                return; 

        /* Remove the current zoom level from the stack */
        mapbook->hide(mapbook->active);
        top = current_zoom_map(mapbook);
        list_del_from(&mapbook->zoom, &top->node);

        /* Set the new zoom level as the active map */
        mapbook->active = current_zoom_map(mapbook);
        mapbook->show(mapbook->active);
}


/******************************************************************************
 * MAP CURSOR 
 * 
 * The map can be scrolled and zoomed with a cursor operating on the
 * movement keys.
 ******************************************************************************/

/**
 * map_control -- draw a cursor on the screen and accept control from FSM
 * @ch: the input word from the FSM
 */
int map_control(int ch)
{
        #define frame_w 6 
        #define frame_h 3
        static WINDOW *frame_win;
        static PANEL  *frame_pan;
        static int y;
        static int x;
        static int y_map;
        static int x_map;

        if (!frame_win) {
                frame_win = newwin(frame_h, frame_w, 0, 0);
                frame_pan = new_panel(frame_win);
        }
        top_panel(frame_pan);

        switch (ch) 
        {
        case MODE_STARTED:
                show_panel(frame_pan);
                break;
        case 'w':
        case 'a':
        case 's':
        case 'd':
        case 'W':
        case 'A':
        case 'S':
        case 'D':
                map_scroll(MAPBOOK->active, ch);
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
        case '\n':
                zoom_in(MAPBOOK, y, x);
                break;
        case ' ':
                zoom_out(MAPBOOK);
                break;
        case KEY_ESC:
        case 'm':
                hide_panel(frame_pan);
                return MODE_RELEASE;
        }

        y_map = pos_y(ACTIVE->pos) + y;
        x_map = pos_x(ACTIVE->pos) + x;

        /*------------------------------------------------------------*/
        copywin(PLATE(ACTIVE, BGR), frame_win, 
                      y_map, x_map, 0, 0, frame_h-1, frame_w-1, 0);
        copywin(PLATE(ACTIVE, RIM), frame_win, 
                      y_map, x_map, 0, 0, frame_h-1, frame_w-1, 1);
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
