/* 
 * multiwin.c -- A multiple-window object that can be cycled (animated).
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
#include "multiwin.h"


/*
 * Method for struct multiwin_t -- increment WINDOW pointer and set
 * the result in member 'peek'.
 */
void next_window(void *self)
{
        struct multiwin_t *multi = (struct multiwin_t *)self;

        multi->i = (multi->i + 1) % multi->n;

        if (multi->win[multi->i]) {
                multi->peek = multi->win[multi->i];
                wrefresh(multi->peek);
        }
}


/*
 * Method for struct multiwin_t -- decrement WINDOW pointer and set
 * the result in member 'peek'.
 */
void prev_window(void *self)
{
        struct multiwin_t *multi = (struct multiwin_t *)self;

        multi->i = (multi->i - 1) % multi->n;

        if (multi->win[multi->i]) {
                multi->peek = multi->win[multi->i];
                wrefresh(multi->peek);
        }
}






/*
 * Allocate and initialize a new struct multiwin_t.
 */
struct multiwin_t *new_multiwin(int h, int w, int y0, int x0, int n)
{
        struct multiwin_t *new = malloc(sizeof(struct multiwin_t));

        new->win  = malloc(n * sizeof(WINDOW *));
        new->n    = n;
        new->i    = 0;
        new->next = &next_window;
        new->prev = &prev_window;

        while (n-->0) {
                if (h > LINES || w > COLS) new->win[n] = newpad(h, w);
                else                       new->win[n] = newwin(h, w, y0, x0);
        }

        new->peek = new->win[0]; // So peek points to something initially.

        return (new);
}

