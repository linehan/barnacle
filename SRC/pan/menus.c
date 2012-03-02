// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "test.h"
#include "dialog.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define THIRD_WIDTH COLS/3
#define L_THIRD     0
#define M_THIRD     ONE_THIRD
#define R_THIRD     COLS - ONE_THIRD

WINDOW *ambo_win;
PANEL  *ambo_pan;
MENU   *crew_menu;
PANEL  *crew_menu_pan;

const char *crew_list[] = { "Foo", "Bar", "Baz", "Qux", (char *)NULL };

/*
  Once called, menu_control() assumes all keyboard input until released.
*/
void menu_control(MENU *my_menu)
{
        int c; // Input

        while ((c = getch()) != KEY_F(1)) {
                switch(c) {
                case KEY_DOWN:
                        menu_driver(my_menu, REQ_DOWN_ITEM);
                        break;
                case KEY_UP:
                        menu_driver(my_menu, REQ_UP_ITEM);
                        break;
                }
                scr_refresh();
        }
}

/*
  Initialize the menus.
*/
void init_bottombar(void)
{
        int n; // Number of items in crew menu.
        int i; // Loop increment.
        WINDOW *crew_menu_win;
        ITEM  **crew_menu_items;

        n = ARRAY_SIZE(crew_list);

        // Create menu items.
        crew_menu_items = (ITEM **)calloc(n, sizeof(ITEM *));
        for (i=0; i<n; ++i) 
                crew_menu_items[i] = new_item(crew_list[i], crew_list[i]);
        
        // Create menu.
        crew_menu     = new_menu((ITEM **)crew_menu_items);
        crew_menu_win = newwin(n, THIRD_WIDTH, LINES-n, L_THIRD);
        crew_menu_pan = new_panel(crew_menu_win);

        // Set menu options.
        set_menu_win(crew_menu, crew_menu_win);
        set_menu_mark(crew_menu, "*"); // Mark active item with this string.

        // Post the menu.
        post_menu(crew_menu);   
        vrt_refresh();
}
