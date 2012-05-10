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


/* Do not fear ! */
struct stdmenu_t {
        /*---------------------------- Windows and panels */
        WINDOW *win;  /* Main MENU window */
        WINDOW *sub;  /* Embedded MENU window */
        WINDOW *buf;  /* Pattern or item name/description buffer */
        WINDOW *nfo;  /* Multipurpose "info" window */
        PANEL *pan;   /* Holds all the windows */
        PANEL *bufpan;
        PANEL *nfopan;
        /*---------------------------- Dimensions */
        int nitem;       /* Number of items in stdmenu */
        int nrows;       /* Number of rows at a time in the menu */
        int ncols;       /* Number of columns at a time in the menu */
        /*---------------------------- Menu components */
        MENU *menu;     
        ITEM **item;  
        wchar_t **icon;  /* Wide-character icons (optional) */
        /*---------------------------- Rendering and display */
        short pair_item_lo;
        short pair_item_hi;
        short pair_item_gr;
        short pair_icon_lo;
        short pair_icon_hi;
        short pair_icon_gr;
        short pair_name_lo;
        short pair_name_hi;
        short pair_name_gr;
        /*---------------------------- Book-keeping */
        bool is_posted;
        bool is_open;
        bool has_focus;
        /*---------------------------- Iterated quantities */
        ITEM *cur_item;
        ITEM *old_item;
        void *cur_ptr;
        int cur_top;
        int cur_row;
        /*---------------------------- Methods */
        void (*die)(void *self);
        void (*post)(void *self);
        void (*unpost)(void *self);
        void (*open)(void *self);
        void (*close)(void *self);
        void (*tog)(void *self);
        void (*focus)(void *self);
        void (*unfocus)(void *self);
        void (*icons)(void *self, int yofs, int xofs);
        void (*prev)(void *self);
        void (*next)(void *self);
        void (*pgup)(void *self);
        void (*pgdn)(void *self);
};

struct stdmenu_t *new_stdmenu(char **name, char **desc, wchar_t **icon, void **usrptr, int n);

void stdmenu_win(struct stdmenu_t *stdmenu, 
                 int h, int w, int y, int x,
                 int tpad, int bpad, int lpad, int rpad);

void stdmenu_color(struct stdmenu_t *smenu, short fore, short back, short grey);
void stdmenu_color_item(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr);
void stdmenu_color_name(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr);
void stdmenu_color_icon(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr);

void stdmenu_cfg(struct stdmenu_t *smenu, int opt, bool set, const char *ch);
void stdmenu_buf(struct stdmenu_t *smenu, WINDOW *buf);
void stdmenu_move(struct stdmenu_t *smenu, int y, int x);

#endif
