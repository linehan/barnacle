#pragma once
#ifndef __STDMENU_H
#define __STDMENU_H

struct stdmenu_t {
        WINDOW *win;
        WINDOW *sub;
        WINDOW *buf;
        PANEL *pan;
        MENU *menu;
        ITEM **item;
        int nitems;
        short fore;
        short back;
        int nrows;
        int ncols;
        void (*die)(void *self);
        void (*vis)(void *self, bool opt);
        void (*build)(void *self, int opt);
        void (*post)(void *self, bool opt);
};


struct stdmenu_t *new_stdmenu(char **name, char **desc, void **usrptr, int n);
void stdmenu_win(struct stdmenu_t *stdmenu, int h, int w, int y, int x);
void stdmenu_color(struct stdmenu_t *stdmenu, short fore, short back);
void stdmenu_cfg(struct stdmenu_t *stdmenu, int opt, bool set, const char *ch);
void stdmenu_buf(struct stdmenu_t *stdmenu, WINDOW *buf);

#endif
