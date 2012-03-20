#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <locale.h>

#include "gfx/gfx.h"

int arawak_init(void)
{
        /* UTF-8 and font settings 
        ***************************************************/
        setlocale(LC_ALL,""); // UTF-8

        /* Start ncurses 
        ***************************************************/
        initscr();            // Start ncurses
        start_color();        // Initialize color display 
        cbreak();	      // Disable line buffering
        noecho();             // Do not echo input
        keypad(stdscr, TRUE); // Enable special keys
        curs_set(0);          // hide cursor

        /* Start the GeoJug engine
        ***************************************************/
        geojug_start();       // Start the graphics engine

        init_random();      // Mersenne twister
        init_simplex();     // Perlin simplex noise generator
        init_palette(0);    // Set color palette
        init_gfx_colors();  // Apply palette to character set
        init_boats();       // Initialize boat MOBs
        init_weather();     // Start weather engine
        init_instruments(); // Various meters (compass)
        init_menus();       // psh menu and commands
        init_test();        // Start test structures

        terminal_check();     // No xterm allowed

        init_ctrlpanels();

        return 0;
}
