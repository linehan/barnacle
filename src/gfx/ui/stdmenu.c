#include <stdlib.h>
#include "../gfx.h"
#include "stdmenu.h"

void stdmenu_destroy(void *self)
{
        int i;
        struct stdmenu_t *stdmenu = (struct stdmenu_t *)self;

        del_panel(stdmenu->pan);
        delwin(stdmenu->buf);
        delwin(stdmenu->sub);
        delwin(stdmenu->win);

        unpost_menu(stdmenu->menu);
        free_menu(stdmenu->menu);

        for (i=0; i<stdmenu->nitems; i++)
                free_item(stdmenu->item[i]);

        free(stdmenu);
}


bool stdmenu_is_visible(void *self)
{
        struct stdmenu_t *stdmenu = (struct stdmenu_t *)self;

        return (panel_hidden(stdmenu->pan)) ? false : true;
}


void stdmenu_visible(void *self, bool opt)
{
        struct stdmenu_t *stdmenu = (struct stdmenu_t *)self; 

        if (opt == true)
                show_panel(stdmenu->pan);
        else
                hide_panel(stdmenu->pan);
}


void stdmenu_post(void *self, bool opt)
{
        struct stdmenu_t *stdmenu = (struct stdmenu_t *)self; 

        if (opt == true)
                post_menu(stdmenu->menu);
        else
                unpost_menu(stdmenu->menu);
}

struct stdmenu_t *new_stdmenu(char **name, char **desc, void **usrptr, int n)
{
        struct stdmenu_t *new;
        int i;

        new = malloc(sizeof(struct stdmenu_t));

        new->item = calloc(n+1, sizeof(ITEM *));

        for (i=0; i<n; i++) {
                new->item[i] = new_item(name[i], desc[i]);
                if (usrptr != NULL)
                        set_item_userptr(new->item[i], usrptr[i]);
        }
        new->item[n] = (ITEM *)NULL;

        new->menu = new_menu((ITEM **)new->item);

        new->die = &stdmenu_destroy;
        new->vis = &stdmenu_visible;
        new->post = &stdmenu_post;
        new->isvis = &stdmenu_is_visible;

        return (new);
}


void stdmenu_win(struct stdmenu_t *stdmenu, int h, int w, int y, int x)
{
        #define winh(h) (h)
        #define winw(w) (w)
        #define subh(h) (winh(h)-2)
        #define subw(w) ((winw(w))-2) 

        stdmenu->win = newwin(winh(h), winw(w), y, x);
        stdmenu->sub = derwin(stdmenu->win, subh(h), subw(w), 1, 1);

        wbkgrnd(stdmenu->win, &PURPLE[2]);
        wbkgrnd(stdmenu->sub, &PURPLE[2]);

        stdmenu->pan = new_panel(stdmenu->win);

        hide_panel(stdmenu->pan);

        set_menu_win(stdmenu->menu, stdmenu->win);
        set_menu_sub(stdmenu->menu, stdmenu->sub);

        stdmenu->nrows = h-2;
        stdmenu->ncols = 1;

        set_menu_format(stdmenu->menu, stdmenu->nrows, stdmenu->ncols);
}


void stdmenu_buf(struct stdmenu_t *stdmenu, WINDOW *buf)
{
        stdmenu->buf = buf;
}


void stdmenu_color(struct stdmenu_t *stdmenu, short fore, short back, short grey)
{
        stdmenu->fore = fore;
        stdmenu->back = back;
        stdmenu->grey = grey;

        set_menu_fore(stdmenu->menu, COLOR_PAIR(stdmenu->fore));
        set_menu_back(stdmenu->menu, COLOR_PAIR(stdmenu->back));
        set_menu_grey(stdmenu->menu, COLOR_PAIR(stdmenu->grey));
}

void stdmenu_cfg(struct stdmenu_t *stdmenu, int opt, bool set, const char *ch)
{
        switch (opt) {
        case DESC:
                if (set == false)
                        menu_opts_off(stdmenu->menu, O_SHOWDESC);
                break;
        case MARK:
                if (set == false)
                        set_menu_mark(stdmenu->menu, "");
                if (ch != NULL)
                        set_menu_mark(stdmenu->menu, ch);
                break;
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
