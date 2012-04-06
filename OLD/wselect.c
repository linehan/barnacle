// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include "wselect.h"
#include "gfx.h"





WSELECT *new_wselect(const wchar_t *selection, int n, short base, short hi, short sel, short selhi)
{
        int i;
        short pai;

        WSELECT *new = malloc(sizeof(WSELECT));

        new->pair[0] = base;
        new->pair[1] = hi;
        new->pair[2] = sel;
        new->pair[3] = selhi;
        new->n       = n;
        new->curs    = -1;  /* Until selection has focus */
        new->pick    = -1;  /* Until selection is made */
        new->focus   = 0;

        new->len = (wcslen(selection)+1);
        wcsncpy(new->wch, selection, new->len); // Prevent overflow

        for (i=0, pai=new->pair[0]; i<(new->n); i++) {
                pai = (i==new->curs) ? new->pair[1] : new->pair[0];
                setcchar(&new->cch[i], &new->wch[i], 0, pai, NULL);
        }
        return new; 
}

void wselect_paint(WSELECT *sel) 
{
        mvwadd_wchnstr(sel->win, sel->yofs, sel->xofs, sel->cch, sel->n);
        scr_refresh();
}

void wselect(WSELECT *sel)
{
        int i;

        /* Re-set the selection to the base color. */
        for (i=0; i<sel->n; i++) {
                setcchar(&sel->cch[i], &sel->wch[i], 0, sel->pair[0], NULL);
        }
        /* When a cursor exists (selection is in focus) */
        if (sel->focus) {
                setcchar(&sel->cch[sel->curs], &sel->wch[sel->curs], 0, sel->pair[1], NULL);
        }
        /* When a selection exists. */
        if (sel->pick >= 0) {
                setcchar(&sel->cch[sel->pick], &sel->wch[sel->pick], 0, sel->pair[2], NULL);
        }
        /* When the cursor is over the current selection. */
        if (sel->focus && (sel->pick == sel->curs)) {
                setcchar(&sel->cch[sel->pick], &sel->wch[sel->pick], 0, sel->pair[3], NULL);
        }
        wselect_paint(sel); // Post the changes.
}

void wselect_driver(WSELECT *sel, int command)
{
        switch (command) {
        case SEL_FOCUS: 
                sel->focus = 1;
                break;
        case SEL_UNFOCUS:
                sel->focus = 0;
                break;
        case SEL_FIRST:
                sel->curs = 0;
                break;
        case SEL_PREV:
                sel->curs = (sel->curs >0) ? (sel->curs)-1 : (sel->n)-1;
                break;
        case SEL_NEXT:
                sel->curs = (sel->curs+1)%(sel->n);
                break;
        case SEL_SELECT:
                sel->pick = ((sel->pick) == (sel->curs)) ? -1 : (sel->curs);
                break;
        case SEL_DESELECT:
                sel->pick = -1;
                break;
        case SEL_BEGINNING:
                sel->curs = 0;
                break;
        }
        wselect(sel);
}
