#include <stdlib.h>
#include "../gfx.h"
#include "stdmenu.h"
#include "../../lib/textutils.h"

#define STDMENU(x) (struct stdmenu_t *)x
#define STD_BACKGROUND &PURPLE[2]

/* Method forward references
``````````````````````````````````````````````````````````````````````````````*/
void stdmenu_open(void *self);
void stdmenu_close(void *self);
void stdmenu_tog(void *self);
void stdmenu_focus(void *self);
void stdmenu_unfocus(void *self);
void stdmenu_post(void *self);
void stdmenu_unpost(void *self);
void stdmenu_prev(void *self);
void stdmenu_next(void *self);
void stdmenu_pgup(void *self);
void stdmenu_pgdn(void *self);
void stdmenu_print_icons(void *self, int yofs, int xofs);

/* Creators and Destructors
``````````````````````````````````````````````````````````````````````````````*/
void stdmenu_destroy(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        int i;

        del_panel(smenu->pan);
        delwin(smenu->buf);
        delwin(smenu->sub);
        delwin(smenu->win);

        unpost_menu(smenu->menu);
        free_menu(smenu->menu);

        for (i=0; i<smenu->nitem; i++) {
                free_item(smenu->item[i]);
                free(smenu->icon[i]);
        }

        free(smenu);
}

struct stdmenu_t *new_stdmenu(char **name, char **desc, wchar_t **icon, void **usrptr, int n)
{
        struct stdmenu_t *new;
        int i;

        new = malloc(sizeof(struct stdmenu_t));

        new->item = calloc(n+1, sizeof(ITEM *));
        new->icon = calloc(n+1, sizeof(wchar_t *));

        for (i=0; i<n; i++) {
                new->item[i] = new_item(name[i], desc[i]);
                if (usrptr != NULL)
                        set_item_userptr(new->item[i], usrptr[i]);
                if (icon != NULL)
                        new->icon[i] = wcdup(icon[i]);
        }
        new->item[n] = (ITEM *)NULL;

        new->menu = new_menu((ITEM **)new->item);

        new->del     = &stdmenu_destroy;
        new->post    = &stdmenu_post;
        new->unpost  = &stdmenu_unpost;
        new->open    = &stdmenu_open;
        new->close   = &stdmenu_close;
        new->tog     = &stdmenu_tog;
        new->focus   = &stdmenu_focus;
        new->unfocus = &stdmenu_unfocus;
        new->icons   = &stdmenu_print_icons;
        new->next    = &stdmenu_next;
        new->prev    = &stdmenu_prev;
        new->pgup    = &stdmenu_pgup;
        new->pgdn    = &stdmenu_pgdn;

        return (new);
}


/* Initialization and Configuration 
``````````````````````````````````````````````````````````````````````````````*/
/* create the windows */
void stdmenu_win(struct stdmenu_t *smenu, int h, int w, int y, int x,
                 int tpad, int bpad, int lpad, int rpad)
{
        #define subh(h) (h - (tpad+bpad))
        #define subw(w) (w - (lpad+rpad)) 

        smenu->win = newwin(h, w, y, x);
        smenu->sub = derwin(smenu->win, subh(h), subw(w), tpad, lpad);

        wbkgrnd(smenu->win, STD_BACKGROUND);
        wbkgrnd(smenu->sub, STD_BACKGROUND);

        smenu->pan = new_panel(smenu->win);

        hide_panel(smenu->pan);

        set_menu_win(smenu->menu, smenu->win);
        set_menu_sub(smenu->menu, smenu->sub);

        smenu->nrows = h-2;
        smenu->ncols = 1;

        set_menu_format(smenu->menu, smenu->nrows, smenu->ncols);
}

void stdmenu_move(struct stdmenu_t *smenu, int y, int x)
{
        move_panel(smenu->pan, y, x);
}

/* assign a buffer window */
void stdmenu_buf(struct stdmenu_t *smenu, WINDOW *buf)
{
        smenu->buf = buf;
}

/* Assign the three item colorings */
void stdmenu_color_item(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr)
{
        smenu->pair_item_lo = pairlo;
        smenu->pair_item_hi = pairhi;
        smenu->pair_item_gr = pairgr;

        set_menu_fore(smenu->menu, COLOR_PAIR(pairlo));
        set_menu_back(smenu->menu, COLOR_PAIR(pairhi));
        set_menu_grey(smenu->menu, COLOR_PAIR(pairgr));
}

/* Assign the three item name colorings */
void stdmenu_color_name(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr)
{
        smenu->pair_name_lo = pairlo;
        smenu->pair_name_hi = pairhi;
        smenu->pair_name_gr = pairgr;
        wcolor_set(smenu->buf, pairlo, NULL);
}

/* Assign the three icon colorings */
void stdmenu_color_icon(struct stdmenu_t *smenu, short pairlo, short pairhi, short pairgr)
{
        smenu->pair_icon_lo = pairlo;
        smenu->pair_icon_hi = pairhi;
        smenu->pair_icon_gr = pairgr;
}

/* Miscellaneous configuration */
void stdmenu_cfg(struct stdmenu_t *smenu, int opt, bool set, const char *ch)
{
        switch (opt) {
        case DESC:
                if (set == false)
                        menu_opts_off(smenu->menu, O_SHOWDESC);
                break;
        case MARK:
                if (set == false)
                        set_menu_mark(smenu->menu, "");
                if (ch != NULL)
                        set_menu_mark(smenu->menu, ch);
                break;
        }
}


/* Methods
``````````````````````````````````````````````````````````````````````````````*/
/* NOT_METHOD Assignments to be completed on cursor movement */
void stdmenu_onmove(struct stdmenu_t *smenu)
{
        smenu->cur_item = current_item(smenu->menu);
        smenu->cur_ptr  = item_userptr(smenu->cur_item);
        smenu->cur_row  = item_index(smenu->cur_item);
        smenu->cur_top  = top_row(smenu->menu);
}

/* "Open" the menu, i.e. make the panel visible */
void stdmenu_open(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        show_panel(smenu->pan);
        smenu->is_open = true;
}

/* "Close" the menu, i.e. hide the panel */
void stdmenu_close(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        hide_panel(smenu->pan);
        smenu->is_open = false;
}

/* Toggle between open and closed states */
void stdmenu_tog(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        if (smenu->is_open) smenu->close(smenu);
        else                smenu->open(smenu);
}

/* Focus the menu; changes item coloration */
void stdmenu_focus(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        set_menu_fore(smenu->menu, smenu->pair_name_lo);
        smenu->has_focus = true;
}

/* Unfocus the menu; changes item coloration */
void stdmenu_unfocus(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        set_menu_fore(smenu->menu, smenu->pair_name_gr);
        smenu->has_focus = false;
}



/* Post the menu associated with the stdpan (see man 3 curs_menu) */
void stdmenu_post(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        post_menu(smenu->menu);
        smenu->is_posted = true;

}

/* Unpost the menu associated with the stdpan (see man 3 curs_menu) */
void stdmenu_unpost(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        unpost_menu(smenu->menu);
        smenu->is_posted = false;
}

/* Select the previous item in the menu */
void stdmenu_prev(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        menu_driver(smenu->menu, REQ_PREV_ITEM);
        stdmenu_onmove(smenu);
}

/* Select the next item in the menu */
void stdmenu_next(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        menu_driver(smenu->menu, REQ_NEXT_ITEM);
        stdmenu_onmove(smenu);
}

/* Page up through the menu */
void stdmenu_pgup(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        int err;

        err = menu_driver(smenu->menu, REQ_SCR_UPAGE);
        if (err == E_REQUEST_DENIED)
                menu_driver(smenu->menu, REQ_FIRST_ITEM);
}

/* Page down through the menu */
void stdmenu_pgdn(void *self)
{
        struct stdmenu_t *smenu = STDMENU(self);
        int err;

        err = menu_driver(smenu->menu, REQ_SCR_DPAGE);
        if (err == E_REQUEST_DENIED)
                menu_driver(smenu->menu, REQ_LAST_ITEM);
}

/* Print the icon array along the corresponding rows */
void stdmenu_print_icons(void *self, int yofs, int xofs)
{
        struct stdmenu_t *smenu = STDMENU(self);
        short pair;          
        cchar_t cch;         
        int i;

        for (i=(smenu->cur_top); i<(smenu->nitem); i++) {

                pair = (i == smenu->cur_row) ? PUR_GREY : PUR_PURPLE;
                setcchar(&cch, smenu->icon[i], 0, pair, NULL);
                
                mvwadd_wch(smenu->win, yofs+(i-smenu->cur_top), xofs, &cch);
        }
}

