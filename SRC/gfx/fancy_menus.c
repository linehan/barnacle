// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>

#include "../gfx/gfx.h"
#include "fancy_menus.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))


enum fancy_slider_enum{ 
        SLIDER_BOX, 
        SLIDER_BLANK,
        SLIDER_UP_SOLID, 
        SLIDER_UP_OUTLINE, 
        SLIDER_DN_SOLID,
        SLIDER_DN_OUTLINE
};



struct fancy_menu *new_fancy_menu(const char **list, const char **desc, short fg, short bg)
{
        int i;
        int n = 0;

        struct fancy_menu *new = malloc(sizeof(struct fancy_menu));

        while (list[n++]);
        n++;

        ITEM **menu_items = calloc(n+1, sizeof(ITEM *));

        for (i=0; i<n; i++) {
                menu_items[i] = new_item(list[i], list[i]);
        }
        menu_items[n] = (ITEM *)NULL;

        new->detail = desc;
        
        new->menu = new_menu((ITEM **)menu_items);

	menu_opts_off(new->menu, O_SHOWDESC);
        set_menu_fore(new->menu, COLOR_PAIR(fg));
        set_menu_back(new->menu, COLOR_PAIR(bg));
        set_menu_mark(new->menu, "");

        return (new);
}

void put_fancy_menu(struct fancy_menu *fancy, WINDOW *win, int h, int w, int y, int x)
{
        set_menu_win(fancy->menu, win);
        set_menu_sub(fancy->menu, derwin(win, h, w/3, y, x));

        fancy->slider_win = derwin(menu_win(fancy->menu), h, 1, y, x);

        set_menu_format(fancy->menu, h, 1);
}

void post_fancy_menu(struct fancy_menu *fancy)
{
        post_menu(fancy->menu);
        scr_refresh();
}

void set_fancy_select(struct fancy_menu *fancy, short pair)
{
        fancy->select_color = pair;
}

void tog_fancy_select(struct fancy_menu *fancy)
{
        short fore, save;

        save = PAIR_NUMBER(menu_fore(fancy->menu));
        fore = fancy->select_color; 

        set_menu_fore(fancy->menu, COLOR_PAIR(fore));
        fancy->select_color = save;
}

void set_fancy_detail(struct fancy_menu *fancy, int h, int w, int y, int x, short pair)
{
        fancy->detail_color = pair;
        fancy->detail_win = derwin(menu_win(fancy->menu), h, w, y, x);
}

void set_fancy_slider(struct fancy_menu *fancy, const wchar_t *slider_parts, short pair)
{
        #define N_SLIDER_PARTS 6
        
        int i;
        for (i=0; i<N_SLIDER_PARTS; i++) {
                setcchar(&fancy->slider[i], &slider_parts[i], 0, pair, NULL);
        }
}

void post_fancy_slider(struct fancy_menu *fancy)
{
        int i;
        int h = getmaxy(fancy->slider_win);

        werase(fancy->slider_win);
        mvwadd_wch(fancy->slider_win, 0, 0, &fancy->slider[SLIDER_UP_SOLID]);
        mvwadd_wch(fancy->slider_win, 1, 0, &fancy->slider[SLIDER_BOX]);
        for (i=2; i<h; i++) {
                mvwadd_wch(fancy->slider_win, i, 0, &fancy->slider[SLIDER_BLANK]);
        }
        mvwadd_wch(fancy->slider_win, h, 0, &fancy->slider[SLIDER_DN_SOLID]);
        win_refresh(fancy->slider_win);
}

void unpost_fancy_slider(struct fancy_menu *fancy)
{
        werase(fancy->slider_win);
        win_refresh(fancy->slider_win);
}

void post_fancy_detail(struct fancy_menu *fancy)
{
        int ind = item_index(current_item(fancy->menu));
        werase(fancy->detail_win);
        wattr_on(fancy->detail_win, COLOR_PAIR(fancy->detail_color), NULL);
        wwrapstr(fancy->detail_win, fancy->detail[ind]);
        win_refresh(fancy->detail_win);
}
