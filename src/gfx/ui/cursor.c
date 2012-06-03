/* 
 * cursor.c -- Unified cursor movement 
 * The "dock" is a small buffer drawn at the bottom of the window, 
 * containing information and notifications for the player.
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
#include "../../com/barnacle.h"

/**
 * move_cursor -- move a cursor, represented as a PANEL, in some direction
 * @pan: the cursor should be a PANEL
 * @cur_y: y-coordinate (may be changed)
 * @cur_x: x-coordinate (may be changed)
 * @dir  : 'u', 'd', 'l', 'r' -- the direction to shift the cursor
 * @step : how many times to move the cursor in that direction
 */
void move_cursor(PANEL *pan, int *cur_y, int *cur_x, int dir, int step)
{
        switch (dir) {
        case 'l':
                while (step-->0) dec(*cur_x,0);
                break;
        case 'r':
                while (step-->0) inc(*cur_x, COLS);
                break;
        case 'u':
                while (step-->0) dec(*cur_y,0);
                break;
        case 'd':
                while (step-->0) inc(*cur_y, LINES);
                break;
        }
        move_panel(pan, *cur_y, *cur_x);
        top_panel(pan);
        doupdate();
}

