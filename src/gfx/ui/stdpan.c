/* 
 * stdpan.c -- uniform panel style.
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
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdbool.h>
#include "stdpan.h"
#include "../../test/test.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                         a standardized panel style                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void stdpan(WINDOW *win, WINDOW **sub, WINDOW **buf, PANEL **pan)
{
        #define SUB_H(h) h-2
        #define SUB_W(w) w-2
        #define SUB_Y 1
        #define SUB_X 1

        #define BUF_H 1
        #define BUF_W w 
        #define BUF_Y(h) h-1
        #define BUF_X 0

        int h, w;
        int y, x;

        getmaxyx(win, h, w);
        getbegyx(win, y, x);

        *pan = new_panel(win);

        *sub = derwin(win, SUB_H(h), SUB_W(w), SUB_Y, SUB_X);
        *buf = derwin(win, BUF_H, BUF_W, BUF_Y(h), BUF_X);

        wbkgrnd(win, &PURPLE[2]);
        wbkgrnd(*sub, &PURPLE[2]);
        wbkgrnd(*buf, &PURPLE[2]);
}






