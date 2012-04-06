#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdbool.h>
#include "stdpan.h"
#include "../../test/test.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                         a standardized panel style                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void stdpan(WINDOW *win, WINDOW **sub, WINDOW **buf, PANEL **pan)
{
        #define SUB_H(h) h-2
        #define SUB_W(w) w-2
        #define SUB_Y 1
        #define SUB_X 1

        #define BUF_H 1
        #define BUF_W w 
        #define BUF_Y(h) h-1
        #define BUF_X 0

        int h, w;
        int y, x;

        getmaxyx(win, h, w);
        getbegyx(win, y, x);

        *pan = new_panel(win);

        *sub = derwin(win, SUB_H(h), SUB_W(w), SUB_Y, SUB_X);
        *buf = derwin(win, BUF_H, BUF_W, BUF_Y(h), BUF_X);

        wbkgrnd(win, &PURPLE[2]);
        wbkgrnd(*sub, &PURPLE[2]);
        wbkgrnd(*buf, &PURPLE[2]);
}


MENU *make_new_menu(char **name, char **description, void **usrptr, int n)
{
        MENU *new;
        ITEM **item;
        int i;

        item = calloc(n+1, sizeof(ITEM *));

        for (i=0; i<n; i++) {
                item[i] = new_item(name[i], description[i]);
                if (usrptr != NULL)
                        set_item_userptr(item[i], usrptr[i]);
        }
        item[n] = (ITEM *)NULL;

        new = new_menu((ITEM **)item);

        return (new);
}

void menu_wins(MENU *menu, WINDOW *win, WINDOW *sub)
{
        set_menu_win(menu, win);
        set_menu_sub(menu, sub);

}


void menu_pair(MENU *menu, short fore, short back)
{
        set_menu_fore(menu, COLOR_PAIR(fore));
        set_menu_back(menu, COLOR_PAIR(back));
}

void menu_look(MENU *menu, int option, bool setting, const char *ch)
{
        switch (option) {
        case DESC:
                if (setting == false)
                menu_opts_off(menu, O_SHOWDESC);
                break;
        case MARK:
                if (setting == false)
                        set_menu_mark(menu, "");
                if (ch != NULL)
                        set_menu_mark(menu, ch);
                break;
        case POST:
                if (setting == true)
                        post_menu(menu);
        }
}


void menu_control(MENU *menu, int esc)
{
        int h;
        while ((h = getch()), (h != esc)) {
                switch (h) {
                case 'k':
                        menu_driver(menu, REQ_PREV_ITEM);
                        break;
                case 'j':
                        menu_driver(menu, REQ_NEXT_ITEM);
                        break;
                case 'n':
                        menu_driver(menu, REQ_PREV_MATCH);
                        break;
                case 'p':
                        menu_driver(menu, REQ_NEXT_MATCH);
                        break;
                }
                scr_refresh();
        }
}



