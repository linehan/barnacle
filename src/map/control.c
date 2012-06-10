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

static WINDOW *nav_win;
static PANEL  *nav_pan;
static WINDOW *frame_win;
static PANEL  *frame_pan;
#define frame_h (3)
#define frame_w (6)
#define nav_h (1)
#define nav_w (COLS)
static int y_frame;
static int x_frame;
static int y_map;
static int x_map;
static int zoomlevel;
static const char *page[]={"World map", "Regional map", "Local map", "Extra map"};

struct cursor_t *cursor;

bool map_controls_active;


/******************************************************************************
 * SCROLLING 
 * 
 * Scrolling the map is accomplished by copying the contents of the map
 * pad, which extends beyond the screen boundaries, into the screen
 * buffer. The initial coordinates (upper-left) of the copy box are
 * shifted by the keystrokes so that a new portion of the pad is copied
 * into the screen buffer on the next call to map_refresh();
 * 
 ******************************************************************************/

/**
 * map_scroll -- move the map around
 * @map: pointer to the map you want to move
 * @dir: 'lrud' directional motion char
 */
void map_scroll(struct map_t *map, int dir)
{
        switch (dir) {
        case 'w': 
                pos_u(map->pos);
                break;
        case 'a': 
                pos_l(map->pos);
                break;
        case 's': 
                pos_d(map->pos);
                break;
        case 'd': 
                pos_r(map->pos);
                break;
        case 'W': 
                pos_ustep(map->pos, 4);
                break;
        case 'A': 
                pos_lstep(map->pos, 5);
                break;
        case 'S': 
                pos_dstep(map->pos, 4);
                break;
        case 'D': 
                pos_rstep(map->pos, 5);
                break;
        }
        map_refresh(map);
}



/******************************************************************************
 * MAP NAVIGATION 
 * 
 * Cursors are rendered and map information is printed to a special
 * map navigation dock.
 ******************************************************************************/


void map_cursor_init(void)
{
        cursor = new_cursor(frame_h, frame_w, 0, 0, LINES-1, COLS, 0, 0, HJKL);
        nav_win = newwin(nav_h, nav_w, LINES-nav_h, CENT_X-(nav_w/2));
        nav_pan = new_panel(nav_win);
        wbkgrnd(nav_win, &PURPLE[2]);
}

void draw_zoom_box(struct map_t *map, int y, int x)
{
        /*------------------------------------------------------------*/
        copywin(PLATE(map, BGR), cursor->win, y, x, 0, 0, frame_h-1, frame_w-1, 0);
        copywin(PLATE(map, RIM), cursor->win, y, x, 0, 0, frame_h-1, frame_w-1, 1);
        /*------------------------------------------------------------*/
        take_bgcolor_yx(cursor->win, 0         , 0         , INSET_UL);
        take_bgcolor_yx(cursor->win, 0         , frame_w-1 , INSET_UR);
        take_bgcolor_yx(cursor->win, frame_h-1 , 0         , INSET_DL);
        take_bgcolor_yx(cursor->win, frame_h-1 , frame_w-1 , INSET_DR);
        /*------------------------------------------------------------*/
        mvwcch(cursor->win, 0         , 0         , L"▛" , 0 , INSET_UL);
        mvwcch(cursor->win, 0         , frame_w-1 , L"▜" , 0 , INSET_UR);
        mvwcch(cursor->win, frame_h-1 , 0         , L"▙" , 0 , INSET_DL);
        mvwcch(cursor->win, frame_h-1 , frame_w-1 , L"▟" , 0 , INSET_DR);
        /*------------------------------------------------------------*/
}


void print_map_control(int y, int x)
{
        werase(nav_win);
        mvwpumpw(nav_win, 0, 2,  L"y:%03d, x:%03d", pos_y(ACTIVE->pos), pos_x(ACTIVE->pos));
        mvwpumpw(nav_win, 0, 16, L"▏ ymax:%d xmax:%d", ACTIVE->pos->ymax, ACTIVE->pos->xmax);
        mvwpumpw(nav_win, 0, COLS-17, L"▕  %s", page[zoomlevel]);
}

void update_map_control(void)
{
        if (!map_controls_active)
                return;

        hide_dock();
        show_panel(nav_pan);
        top_panel(nav_pan);
}



/******************************************************************************
 * MAP CURSOR CONTROL
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
        bool dozoom = false;
        struct map_t *tmp;

        if (!nav_win)
                map_cursor_init();

        top_panel(nav_pan);

        switch (ch) 
        {
        case MODE_STARTED:
                map_controls_active = true;
                cursor->Show(cursor);
                show_panel(nav_pan);
                break;
                
        /* Scroll the map in a direction */
        case 'w': case 'a': case 's': case 'd':
        case 'W': case 'A': case 'S': case 'D':
                map_scroll(MAPBOOK->active, ch);
                break;

        /* Move the zoom frame in a direction */
        case 'h': case 'j': case 'k': case 'l':
        case 'H': case 'J': case 'K': case 'L':
                cursor->Move(cursor, ch);
                break;

        /* Select the current zoom frame */
        case '\n':
                dozoom = true;
                break;

        case KEY_ESC:
        case 'm':
                map_controls_active = false;
                show_dock();
                cursor->Hide(cursor);
                hide_panel(nav_pan);
                return MODE_RELEASE;
        }

        /* Adjust for changing y0 or x0 of the map pad */
        y_map = pos_y(cursor->pos) + pos_y(ACTIVE->pos);
        x_map = pos_x(cursor->pos) + pos_x(ACTIVE->pos);
        zoomlevel = MAPBOOK->page;

        print_map_control(y_map, x_map);
        draw_zoom_box(ACTIVE, y_map, x_map);

        if (dozoom)
                set_zoom(MAPBOOK, y_map, x_map);

        return MODE_PERSIST;
}

