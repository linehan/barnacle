// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "test.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define W_THIRD COLS/3           // one-third width of stdscr
#define L_THIRD 0                // offset for the leftmost one-third
#define M_THIRD W_THIRD          // offset for the middle one-third
#define R_THIRD COLS - W_THIRD   // offset for the rightmost one-third

WINDOW *ambo_win;
PANEL  *ambo_pan;
MENU   *crew_menu;
PANEL  *crew_menu_pan;

const char *crew_list[] = { "FOO", "BAR", "BAZ", "QUX", (char *)NULL };
const char *crew_desc[] = { "on you, foo I say, foo!", 
                            "soom is a ruddy old ball.", 
                            "turd in a baskit, bazturd in a baskit", 
                            "Magnus, Grace à Dieu, esq.", (char *)NULL };

/*
  Once called, menu_control() assumes all keyboard input until released.
*/
void menu_control(MENU *my_menu)
{
        int c; // Input

        show_panel(crew_menu_pan);

        while ((c = getch()) != 'o') {
                switch(c) {
                case 'j':
                        menu_driver(my_menu, REQ_DOWN_ITEM);
                        break;
                case 'k':
                        menu_driver(my_menu, REQ_UP_ITEM);
                        break;
                }
                scr_refresh();
        }
        hide_panel(crew_menu_pan); // Re-hide the panel on exit.
        vrt_refresh();
}

/*
  Initialize the menus.
*/
void init_crew_menu(void)
{
        int n; // Number of items in crew menu.
        int i; // Loop increment.
        WINDOW *crew_menu_win;
        ITEM  **crew_menu_items;

        n = ARRAY_SIZE(crew_list);

        // Create menu items.
        crew_menu_items = (ITEM **)calloc(n, sizeof(ITEM *));
        for (i=0; i<n; ++i) 
                crew_menu_items[i] = new_item(crew_list[i], crew_desc[i]);
        
        // Create menu.
        crew_menu     = new_menu((ITEM **)crew_menu_items);
        crew_menu_win = newwin(n, W_THIRD, LINES-n, L_THIRD);
        crew_menu_pan = new_panel(crew_menu_win);

        // Set menu options.
        set_menu_win(crew_menu, crew_menu_win);
        set_menu_mark(crew_menu, ""); // Mark active item with this string.

        // Post the menu.
        post_menu(crew_menu);   
        vrt_refresh();
        hide_panel(crew_menu_pan); // Stash it.
}
