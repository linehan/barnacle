// vim:fdm=marker
/*******************************************************************************
    FILENAME:  terrain.c
    Functions for creating landscapes.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>

#include "lib/list/list.h"
#include "lib/gfx.h"
#include "lib/memmacro.h"

#include "sprite.h"
#include "palette.h"
#include "test.h"
#include "dice.h"

ENV *hi;

PANEL *BG;
WINDOW *BGW;
/******************************************************************************/
/* Set background of GFXLIST (bottom-most layer) */
void set_gfx_bg(struct list_head *head, int opt)
{
        /*WNODE *tmp;*/
        /*GNODE *bg = new_gfx(__bg__, LINES, COLS, 0, 0, 1);*/

        /*tmp = TOP_WNODE(bg);*/
        if ((BGW == NULL)) {
                BGW = newwin(LINES, COLS, 0, 0);
                BG = new_panel(BGW);
        }

        switch (opt) {
        case 0:
                wbkgrnd(BGW, &OCEAN[0]);
                break;
        case 1:
                wbkgrnd(BGW, &SAND);
                break;
        case 2:
                wbkgrnd(BGW, &SHORE);
                break;
        }
        /*add_gfx(bg, head);*/
}
/* 
 *  +----------------+ 
 *  | +------------+ <---- edge       These are, essentially, a stacked
 *  | | +--------+ | |                set of panels.
 *  | | |        | <------ mantle
 *  | | |        | | |
 *  | | |        <-------- core
 *  | | +--------+ | |
 *  | +------------+ |
 *  +----------------+
 *
 * Generate terrain body; conforms to a three-tier pattern: */
void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0)
{
        WNODE *tmp;

        /* Apply landscape style */
        int i = 0;
        if ((type=='s')) {
                /* Create nested GNODEs */
                GNODE *edge = new_gfx(__an__, h, w, y0, x0, 2);
                GNODE *mant = new_gfx(__an__, (h-2), (w-2), (y0+1), (x0+1), 1);
                GNODE *core = new_gfx(__an__, (h-4), (w-4), (y0+2), (x0+2), 1);

                tmp = TOP_WNODE(core);
                wbkgrnd(tmp->window, &SAND);

                tmp = TOP_WNODE(mant);
                wbkgrnd(tmp->window, &SHORE);

                tmp = TOP_WNODE(edge);
                list_for_each(edge->wins, tmp, node) {
                        if (((i++%2) == 0)) wbkgrnd(tmp->window, &OCEAN[2]);
                        else                wbkgrnd(tmp->window, &OCEAN[1]);
                }
                /* Add the GNODEs to the wad */
                add_gfx(edge, head);     
                add_gfx(mant, head);     
                add_gfx(core, head);     
        }
        else if ((type=='m')) {

                GNODE *cliff  = new_gfx(__fg__, h, w, y0, x0, 1);
                WNODE *cliffw = TOP_WNODE(cliff);

                int rnoise = 8;
                int lnoise = 4;
                int vnoise = 1;
                int j;

                int wid[h], ofs[h]; /* Line width and x offset arrays */
                wbkgrnd(cliffw->window, &OCEAN[0]);
                /* Generate offset array. To prevent an unconvincing appearance, it is
                 * necessary that any increase in the offset be maintained for at least
                 * one additional unit of height. */
                for (i=0; i<h; i++) {

                        wid[i] = (i==0)        ? w-rnoise          : ((w-roll_fair(rnoise)));
                        ofs[i] = (lnoise != 0) ? roll_fair(lnoise) : lnoise;    

                        /* Here there is a complicated-looking check to duplicate a line's
                         * width and offset if either its width (with offset factored in) is
                         * less than the previous line, or its offset is greater. This is to
                         * prevent the landform from drawing in a way that strains the illusion
                         * of perspective. Tweak the test a little and you'll see what I mean.
                         *
                         * Note that the array pointer is moved along as well, so that at
                         * the end of the while loop, the for loop continues from the end
                         * of the "chunk". */
                        if (((wid[i]-ofs[i-1] <= wid[i-1]+ofs[i]))||(ofs[i]>ofs[i-1])) {
                                j = vnoise;
                                while (j-->0) {
                                        wid[i+1] = wid[i];
                                        ofs[i+1] = ofs[i];
                                        i++;
                                }
                        }
                        wprintw(DIAGNOSTIC_WIN, "off: %d\n", wid[i]);
                }
                /* First, print a sunward character, then a line of dark, translate down, then
                 * a line of brighter contrast. */
                for (i=0; i<(h-1); i++) {
                        /*mvwadd_wch(tmp->window, i, 1, &MTN[0]);*/
                        mvwhline_set(cliffw->window, i, ofs[i], &MTN[1], wid[i]);
                        mvwhline_set(cliffw->window, (i+1), ofs[i], &MTN[2], wid[i]);
                }
                add_gfx(cliff, head);
        }

}
void highlights_init(void)
{
        int i, w, wnoise, x0, xnoise, y0, ynoise;
        wnoise = 50;
        xnoise = 80;
        ynoise = 80;

        hi = new_env();

        for (i=0; i<5; i++) {

                 w = roll_fair(wnoise);
                x0 = roll_fair(xnoise);
                y0 = roll_fair(ynoise);

                GNODE *new = new_gfx(__hi__, 1, w, y0, x0, 1);
                WNODE *wnode = TOP_WNODE(new);
                wbkgrnd(wnode->window, &OCEAN[1]);

                bottom_panel(new->pan);
                add_gfx(new, &hi->wad);
        }
        bottom_panel(BG);
        master_refresh();
}

void shift_highlights(void)
{
        if ((hi == NULL)) highlights_init();

        GNODE *tmp;
        list_for_each(&hi->wad, tmp, node) {
                if ((tmp->dim.x0 >= COLS)) 
                        tmp->dim.x0 = 0;
                else
                        tmp->dim.x0 = (tmp->dim.x0+1);
                if ((tmp->dim.y0 >= LINES))
                        tmp->dim.y0 = 0;
                else
                        tmp->dim.y0 = (tmp->dim.y0+1);
                move_panel(tmp->pan, tmp->dim.y0, tmp->dim.x0);
        }
        master_refresh();
}
