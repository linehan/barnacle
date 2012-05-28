/* 
 * effects.c -- The gloss "pretty print" text effects. 
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
#include "../lib/stoc/stoc.h"
#include "../lib/textutils.h"
#include "../gfx/gfx.h"
#include "effects.h"


/* TEXT EFFECTS 
``````````````````````````````````````````````````````````````````````````````*/
/** 
 * wcs_reveal -- "unmask" a string from left to right 
 */
void wcs_reveal(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        int i;
        for (i=0; i<L; i++) {
                sayw(win, &wcs[i]);
        }
}

/** 
 * wcs_unreveal -- "re-mask" a string from right to left
 */
void wcs_unreveal(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        int i;
        wmove(win, 0, L);
        for (i=L; i<len; i++) {
                sayw(win, &wcs[i]);
        }
}

/**
 * wcs_shine -- turn a string from its base color to its highlight color
 */
void wcs_shine(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        mvsayw(win, 0, L-1, &wcs[L-1]);
}

/**
 * wcs_pushr -- "push" a string to the right until it disappears
 */
void wcs_pushr(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        int i;
        wmove(win, 0, L);
        for (i=0; i<(len-L); i++) {
                sayw(win, &wcs[i]);
        }
}

/**
 * wcs_pushl -- "push" a string to the left until it disappears
 */
void wcs_pushl(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        int i;
        for (i=0; i<len; i++) {
                sayw(win, &wcs[i]);
        }
}


/**
 * wcs_dropout -- randomly "dissolve" a string
 */
void wcs_dropout(WINDOW *win, const wchar_t *wcs, size_t len, int L)
{
        #define BUFMAX 100
        static wchar_t buf[BUFMAX];
        static int count;
        int steps;
        int i;

        /* Copy a new string to the buffer on the first loop */
        if (L == 0) {
                wclean(buf, BUFMAX);
                wcscpy(buf, wcs);
                count = len;

                for (i=0; i<len; i++) {
                        if (iswspace(buf[i])) 
                                count--;
                }
        }

        steps = roll_fair(--count); /* Roll how many to skip before removing */
 
        for (i=0;;) {                           ///
                if (!iswspace(buf[i])) steps--;  //
                if (steps>=0) i++;               // 
                else {                           // forgive me
                        buf[i]=L' ';             //
                        break;                   // 
                }                               ///
        }

        /* Print the perforated buffer */
        for (i=0; i<len; i++) {
                sayw(win, &buf[i]);
        }
}


