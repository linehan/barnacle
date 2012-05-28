/* 
 * looptest.c -- Print spinners and the game tick. 
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
#include "../eng/tick.h"
#include "looptest.h"
#include "test.h"

/**
 * If the boolean value loop_test_active is set 'true', a diagnostic view
 * of the loop progress and the game tick will be printed to CONSOLE_WIN.
 */
bool loop_test_active;

/******************************************************************************/

/**
 * spinloop -- turn a spinner and print a message
 * @y  : y-coordinate to begin printing at
 * @x  : x-coordinate to begin printing at
 * @str: string to accompany spinner
 * @sp : iterator that will turn the spinner
 */
inline void spinloop(int y, int x, const char *str, int sp)
{
        mvwprintw(CONSOLE_WIN, y, x, "(%c) %s\n", SPINNER(sp), str);
}

/**
 * tickloop -- print the current game tick 
 * @y  : y-coordinate to begin printing at
 * @x  : x-coordinate to begin printing at
 * @str: string to accompany spinner
 */
inline void tickloop(int y, int x, const char *str)
{

        mvwprintw(CONSOLE_WIN, y, x, "%s %u\n", str, get_tick());
}

/**
 * loop_test -- prints loop diagnostic information to CONSOLE_WIN */
void loop_test(bool opt)
{
        loop_test_active = opt;

        if (loop_test_active) {
                werase(CONSOLE_WIN);
                mvwprintw(CONSOLE_WIN, 0, 0, "Event loop status\n");
        } else {
                werase(CONSOLE_WIN);
        }
}

/******************************************************************************/

/* called by the render_cb */
void spin_render_loop(void)
{
        static uint32_t sp;
        if (loop_test_active) {
                spinloop(2, 0, "render_cb", sp++);
                tickloop(7, 0, "Game tick:");
                mvwprintw(CONSOLE_WIN, 8, 0, "Maxi tick: %u", UINT32_MAX);
        }
}

/* called by the animate_cb */
void spin_animate_loop(void)
{
        static uint32_t sp;
        if (loop_test_active) {
                spinloop(4, 0, "animate_cb", sp++);
        }
}

/* called by the flow_cb */
void spin_flow_loop(void)
{
        static uint32_t sp;
        if (loop_test_active) {
                spinloop(3, 0, "flow_cb", sp++);
        }
}

/* called by the iolisten_cb */
void spin_io_loop(void)
{
        static uint32_t sp;
        if (loop_test_active) {
                spinloop(5, 0, "iolisten_cb", sp++);
        }
}

