#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "stdgfx.h"
#include "colortest.h"


int main(int argc, char *argv[])
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


        int palnum=0;

        int c;

        ggrtest(argv[1]);

        while (c = getch(), c != 'q') {
                switch (c)
                {
                case 'n':
                        paint_ramp(palnum);
                        palnum = ((palnum+1) % NRAMPS);
                        goto dopaint;
                case 'p':
                        palnum = (palnum > 0) ? (palnum-1) : NRAMPS-1;
                        goto dopaint;
                case '3':
                        paint_ramp(SPECTRAL3);
                        break;
                case '4':
                        paint_ramp(SPECTRAL4);
                        break;
                case '5':
                        paint_ramp(SPECTRAL5);
                        break;
                case '6':
                        paint_ramp(SPECTRAL6);
                        break;
                case '7':
                        paint_ramp(SPECTRAL7);
                        break;
                case '8':
                        paint_ramp(SPECTRAL8);
                        break;
                case '9':
                        paint_ramp(SPECTRAL9);
                        break;
                case '0':
                        paint_ramp(SPECTRAL10);
                        break;
                case '1':
                        paint_ramp(SPECTRAL11);
                        break;
                case 't':
                        werase(panel_window(colortest_panel()));
                        update_panels();
                        doupdate();
                        break;
                }

                continue;

                dopaint: 
                        paint_ramp(palnum);
        }


        endwin(); /* end curses mode */
        return 0;
}
