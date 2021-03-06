/* 
 * titlecard.c -- The startup sequence that plays when the game starts.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../gfx/gfx.h"

WINDOW *wwin, *hwin, *bwin;
PANEL *ppan;
PANEL *hpan;
PANEL *bpan;
cchar_t bgtile;

inline void borderdraw(WINDOW *win, int h, int w, cchar_t *cch)
{
        int i, j;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {
                if (i==0 || i==(h-1))
                        mvwadd_wch(win, i, j, cch);
                else if (j==0 || j==(w-1))
                        mvwadd_wch(win, i, j, cch);
        }
        }
}

inline void put_xright(WINDOW *win, int y, const char *str)
{
        int len;
        int h, w;

        getmaxyx(win, h, w);
       
        len = strlen(str);

        if (len > w) return;
        else
                mvwprintw(win, y, (w-len)+1, "%s", str);
}

inline void put_xcenter(WINDOW *win, int y, const char *str)
{
        int len;
        int h, w;

        getmaxyx(win, h, w);
       
        len = strlen(str);

        if (len > w) return;
        else
                mvwprintw(win, y, (w-len)/2, "%s", str);
}


void print_title(const char *title)
{
        #define HOZ_PAD 10
        #define VER_PAD 3
        #define TITLE_H 5

        #define BODY_PAD (TITLE_H+VER_PAD)+VER_PAD
        #define BODY_H (LINES-(TITLE_H+VER_PAD))/2
        #define BODY_W COLS/2

        int len;

        len = strlen(title);
        if (ppan == NULL) {
                wwin = newwin(LINES, COLS, 0, 0);

                ppan = new_panel(wwin);
                setcchar(&bgtile, L"༕", A_REVERSE, TITLE_COLOR, NULL);
                wbkgrnd(wwin, &bgtile);

                hwin = newwin(TITLE_H, len+HOZ_PAD, VER_PAD, (COLS-(len+HOZ_PAD))/2);
                hpan = new_panel(hwin);

                borderdraw(hwin, TITLE_H, len+HOZ_PAD, &bgtile);
                put_xcenter(hwin, 2, title);

                bwin = newwin(BODY_H, BODY_W, BODY_PAD, (COLS-BODY_W)/2);
                bpan = new_panel(bwin);
                borderdraw(hwin, BODY_H, BODY_W, &bgtile);

                scrollok(bwin, true);


                scr_refresh();
        }
        else {
                top_panel(ppan);
                top_panel(hpan);
                top_panel(bpan);
                scr_refresh();
        }
}

bool nomoretitle = false;

void show_title(void)
{
        if (nomoretitle)
                return;

        show_panel(ppan);
        show_panel(hpan);
        show_panel(bpan);
        doupdate();
}


void print_status(const char *status)
{
        if (nomoretitle)
                return;

        show_title();
        wprintw(bwin, " %s", status);
        wrefresh(bwin);
        update_panels();
}



void hide_title(void)
{
        if (ppan) {
                hide_panel(hpan);
                hide_panel(ppan);
                hide_panel(bpan);
                scr_refresh();
        }
        nomoretitle = true;
}


void print_hold(const char *status)
{
        print_status("\n");
        print_status(status);
        update_panels();
        nodelay(stdscr, false);
        getch();
        nodelay(stdscr, true);

        hide_title();
}

