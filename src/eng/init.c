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

#define STR(n) #n

#define COLOR_TARGET 256

int arawak_init(void)
{
        setlocale(LC_ALL,""); // UTF-8

        initscr();            // Start ncurses
        start_color();        // Initialize color display 
        cbreak();	      // Disable line buffering
        noecho();             // Do not echo input
        nodelay(stdscr, true);// Calls to getch() are non-blocking 
        keypad(stdscr, TRUE); // Enable special keys
        curs_set(0);          // hide cursor

        if (!has_colors() || !can_change_color()) {
                print_status(FAILURE);
                print_status("Your terminal does not support color!");
        }
        if (COLORS < COLOR_TARGET) {
                print_status(FAILURE);
                print_status("Your terminal only supports" STR(COLORS) 
                             "colors; this is insufficient.");
        }

        else {
                init_palette(0);    // Set color palette
                init_gfx_colors();  // Apply palette to character set

        }

        print_title("Welcome to Arawak!");
        print_status("Setting colors...");
        print_status(SUCCESS);

        geojug_start();     // Start the graphics engine
        print_status("Twisting Mersenne...");
        init_stochastics();      
        print_status(SUCCESS);
        init_surface_flow(5000);

        /*init_menus();       // psh menu and commands*/
        print_status("Test-o-plexing...");
        init_test();        // Start test structures

        print_status(SUCCESS);

        return 0;

}
