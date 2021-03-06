/* 
 * init.c -- initialize the engine 
 * Performs checks on the user's terminal and operating environment.
 * If something is wrong, execution is terminated and a report printed.
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
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <locale.h>

#include "../gfx/gfx.h"
#include "../gfx/ui/titlecard.h"
#include "../map/tile.h"
#include "error.h"


/*
 * Sets terminal to color mode and checks that it supports 256 
 * programmable colors; if it does, initializes the palette. 
 */
int prep_termcolors(void)
{
        /* Ensure terminal supports color */
        TRY {
                if (has_colors())
                        start_color();
                else
                        THROW(1);
        } CATCH("Terminal does not support color!", 0);

        /* Ensure terminal supports programmable colors */
        TRY {
                if (can_change_color()) {
                        init_palette(0);
                        init_gfx_colors();
                } else 
                        THROW(1);

        } CATCH("Terminal does not support programmable colors", 0);

        /* Ensure terminal supports 256 colors */
        TRY {
                if (COLORS < 256) 
                        THROW(1);

        } CATCH("Terminal does not support 256 colors", 0);
        
        return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * Configures the terminal's input-output modes
 */
int prep_termio(void)
{
        cbreak();	       /* Do not wait for newline (line buffering) */
        noecho();              /* Do not echo input to screen */
        nodelay(stdscr, true); /* Calls to getch() are non-blocking */ 
        keypad(stdscr, TRUE);  /* Enable special keys */
        curs_set(0);           /* Do not display cursor */

        /* 
         * ncurses sets the ESC key delay at 100ms by default, and this
         * is way too slow. According to Wolfram Alpha, the duration of
         * a typical human blink is 100ms-400ms.
         */
        if (getenv("ESCDELAY") == NULL) ESCDELAY = 25;

        return 1;
}

/* -------------------------------------------------------------------------- */

/*
 * Put the terminal in curses mode and enable/disable the appropriate
 * settings
 */
int prep_terminal(void)
{
        initscr();            /* Start ncurses */

        prep_termcolors();   
        prep_termio();        

        return 1;
}

/* -------------------------------------------------------------------------- */

/*
 * The main initialization function for the program
 */
int barnacle_init(void)
{
        setlocale(LC_ALL,""); /* Must be set for UTF-8 support */

        init_signal_handlers();
        prep_terminal();      /* Set modes and perform checks... */

        /* Print welcome */
        print_title("Welcome to Barnacle!");
        print_status("Setting colors...");
        print_status(SUCCESS);

        geojug_start();       /* Start the graphics engine */
        print_status("Twisting Mersenne...");
        init_stochastics();   /* Start the RNG and helpers */   
        print_status(SUCCESS);
        /*init_tiles();*/

        print_status("Tuning tests...");
        init_test();          /* Start test structures */

        print_status(SUCCESS);

        return 0;
}

