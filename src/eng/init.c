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


/*
 * Sets terminal to color mode and checks that it supports 256 
 * programmable colors; if it does, initializes the palette. 
 */
int prep_termcolors(void)
{
        if (has_colors()) {
                start_color();        /* Enable terminal colors */
        } else {
                printf("Terminal does not support color");
                goto fail;
        }

        if (can_change_color()) {
                init_palette(0);      /* Build our color palette */ 
                init_gfx_colors();    /* Apply the palette to certain graphics */ 
        } else {
                printf("Terminal does not support programmable colors");
                goto fail;
        }

        if (COLORS < 256) {
                printf("Terminal does not support 256 colors");
                goto fail;
        }

        return 1;

        fail:
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
int arawak_init(void)
{
        setlocale(LC_ALL,""); /* Must be set for UTF-8 support */

        prep_terminal();      /* Set modes and perform checks... */

        /* Print welcome */
        print_title("Welcome to Arawak!");
        print_status("Setting colors...");
        print_status(SUCCESS);

        geojug_start();       /* Start the graphics engine */
        print_status("Twisting Mersenne...");
        init_stochastics();   /* Start the RNG and helpers */   
        print_status(SUCCESS);
        init_surface_flow(1);

        print_status("Test-o-plexing...");
        init_test();          /* Start test structures */

        print_status(SUCCESS);

        return 0;
}

