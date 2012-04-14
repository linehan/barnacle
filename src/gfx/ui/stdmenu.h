#pragma once
#ifndef __STDMENU_H
#define __STDMENU_H

/*
 * +-----------------+
 * |+---------------+| '
 * || Menu item 0   ||  ` 
 * || Menu item 1   ||  `
 * || Menu item 2   ||  `
 * || Menu item 3   ||  `
 * || Menu item 4   ||  `-- win/sub
 * || Menu item 5   ||  `
 * || Menu item 6   ||  `
 * || Menu item 7   ||  `
 * || ...           ||  `
 * |+---------------+| ,`
 * +-----------------+
 * |                 | }-- buf
 * |                 | }-- nfo
 * +-----------------+
 */




struct stdmenu_t {
        WINDOW *win;  /* Main MENU window */
        WINDOW *sub;  /* Embedded MENU window */
        WINDOW *buf;  /* Pattern or item name/description buffer */
        WINDOW *nfo;  /* Multipurpose "info" window */
        PANEL *pan;   /* Holds all the windows */
        PANEL *bufpan;
        PANEL *nfopan;
        MENU *menu;   /* Stores items */
        ITEM **item;
        int nitems;
        short fore;   /* FG color */
        short back;   /* BG color */
        short grey;   /* Non-selectable items color */
        int nrows;
        int ncols;
        void (*die)(void *self);
        void (*vis)(void *self, bool opt);
        bool (*isvis)(void *self);
        void (*build)(void *self, int opt);
        void (*post)(void *self, bool opt);
};


struct stdmenu_t *new_stdmenu(char **name, char **desc, void **usrptr, int n);
void stdmenu_win(struct stdmenu_t *stdmenu, int h, int w, int y, int x);
void stdmenu_color(struct stdmenu_t *stdmenu, short fore, short back, short grey);
void stdmenu_cfg(struct stdmenu_t *stdmenu, int opt, bool set, const char *ch);
void stdmenu_buf(struct stdmenu_t *stdmenu, WINDOW *buf);

#endif
