#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <locale.h>
#include <stdarg.h>

#include "rgb_nc.h"

int main(int argc, char *argv[])
{
        /*************************************************** init */
        /* UTF-8 and font settings */
        setlocale(LC_ALL,""); // UTF-8

        /* Start ncurses */
        initscr();            // Start ncurses
        start_color();        // Initialize color display 
        cbreak();	      // Disable line buffering
        noecho();             // Do not echo input
        curs_set(0);          // hide cursor
        /*************************************************** end init */

        PALETTE *mypal = new_pal();

        pal_load(mypal, argv[1], 30);
        pal_swatch(mypal, L"█");

        pal_printw(stdscr, mypal);
        doupdate();

        while ((getch()) != 'q');

        endwin(); /* end curses mode */
        return 0;
}
