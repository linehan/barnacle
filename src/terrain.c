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
/******************************************************************************/
/* Set background of GFXLIST (bottom-most layer) */
void set_gfx_bg(struct list_head *head, int opt)
{
        WINNODE *tmp;
        GFXNODE *bg = new_gfx(__bg__, LINES, COLS, 0, 0, 1);

        tmp = list_top(&(bg->winwad), WINNODE, winnode);

        switch (opt) {
        case 0:
                wbkgrnd(tmp->window, &OCEAN);
                break;
        case 1:
                wbkgrnd(tmp->window, &SAND);
                break;
        case 2:
                wbkgrnd(tmp->window, &SHORE);
                break;
        }
        add_gfx(bg, head);
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
        WINNODE *tmp;

        /* Apply landscape style */
        int j, i = 0;
        if ((type=='s')) {
                /* Create nested GFXNODEs */
                GFXNODE *edge = new_gfx(__an__, h, w, y0, x0, 2);
                GFXNODE *mant = new_gfx(__an__, (h-2), (w-2), (y0+1), (x0+1), 1);
                GFXNODE *core = new_gfx(__an__, (h-4), (w-4), (y0+2), (x0+2), 1);

                tmp = list_top(&(core->winwad), WINNODE, winnode);
                wbkgrnd(tmp->window, &SAND);

                tmp = list_top(&(mant->winwad), WINNODE, winnode);
                wbkgrnd(tmp->window, &SHORE);

                tmp = list_top(&(edge->winwad), WINNODE, winnode);
                list_for_each(&(edge->winwad), tmp, winnode) {
                        if (((i++%2) == 0)) wbkgrnd(tmp->window, &SURF0);
                        else                wbkgrnd(tmp->window, &SURF1);
                }
                /* Add the GFXNODEs to the wad */
                add_gfx(edge, head);     
                add_gfx(mant, head);     
                add_gfx(core, head);     
        }
        else if ((type=='f')) {
                GFXNODE *forest = new_gfx(__fg__, h, w, y0, x0, 1);
                tmp = list_top(&(forest->winwad), WINNODE, winnode);
                for (i=0; i<h; i++) {
                        if ((i==0))
                                mvwhline_set(tmp->window, 0, 0, &FOREST[0], w); 
                        else if ((i<(h-2))) {
                                mvwhline_set(tmp->window, i, 0, &FOREST[2], w); 
                                mvwadd_wch(tmp->window, i, 0, &FOREST[1]); 
                                mvwadd_wch(tmp->window, i, (w-1), &FOREST[1]); 
                        }
                        else if ((i==(h-2)))
                                mvwhline_set(tmp->window, i, 0, &FOREST[1], w); 
                        else
                                mvwhline_set(tmp->window, i, 0, &FOREST[3], w); 
                }
                add_gfx(forest, head);   
        }
        else if ((type=='m')) {

                GFXNODE *gen = new_gfx(__fg__, h, w, y0, x0, 1);
                tmp = list_top(&(gen->winwad), WINNODE, winnode);

                wbkgrnd(tmp->window, &OCEAN);

                int rnoise = 8;
                int lnoise = 4;
                int vnoise = 1;
                int znoise = 1;
                int j;

                int wid[h], ofs[h]; /* Line width and x offset arrays */

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
                        mvwhline_set(tmp->window, i, ofs[i], &MTN[1], wid[i]);
                        mvwhline_set(tmp->window, (i+1), ofs[i], &MTN[2], wid[i]);
                }
                add_gfx(gen, head);
        }
}
