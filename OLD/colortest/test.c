#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "stdgfx.h"
#include "colortest.h"


int main(void)
{
        /* UTF-8 and font settings 
        ***********************************************************/
        setlocale(LC_ALL,""); // UTF-8

        /* Start ncurses 
        ***********************************************************/
        initscr();            // Start ncurses
        start_color();        // Initialize color display 
        cbreak();	      // Disable line buffering
        noecho();             // Do not echo input
        keypad(stdscr, TRUE); // Enable special keys
        curs_set(0);          // hide cursor


        int c;

        init_colortest(SPECTRUM11, 30);

        while (c = getch(), c != 'q') {
                switch (c)
                {
                case 't':
                        paint_ramp(SPECTRUM6);
                        break;
                case 'r':
                        paint_ramp(SPECTRUM11);
                        break;
                }
        }


        endwin(); /* end curses mode */
        return 0;
}
