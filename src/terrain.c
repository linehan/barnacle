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
/******************************************************************************/
/* Set background of GFXLIST (bottom-most layer) */
void set_gfx_bg(struct list_head *head, int opt)
{
        WINNODE *tmp;
        GFXNODE *bg = new_gfx(LINES, COLS, 0, 0, 1);

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

        /* Create nested GFXNODEs */
        GFXNODE *edge = new_gfx(h, w, y0, x0, 2);
        GFXNODE *mant = new_gfx((h-2), (w-2), (y0+1), (x0+1), 1);
        GFXNODE *core = new_gfx((h-4), (w-4), (y0+2), (x0+2), 1);

        /* Apply landscape style */
        switch(type) {
        case 's':
                tmp = list_top(&(core->winwad), WINNODE, winnode);
                wbkgrnd(tmp->window, &SAND);

                tmp = list_top(&(mant->winwad), WINNODE, winnode);
                wbkgrnd(tmp->window, &SHORE);

                tmp = list_top(&(edge->winwad), WINNODE, winnode);
                list_for_each(&(edge->winwad), tmp, winnode) {
                        wbkgrnd(tmp->window, &SURF0);
                }
        }
        /* Add the GFXNODEs to the wad */
        add_gfx(edge, head);     
        add_gfx(mant, head);     
        add_gfx(core, head);     
}
/* Simple collision hit test */
int hit_test(int y, int x, GFXNODE *gfx)
{
        if ((x >= gfx->dim->x0) && 
           (x <= (gfx->dim->x0 + gfx->dim->w)) &&
           (y >= gfx->dim->y0) &&
           (y <= (gfx->dim->y0 + gfx->dim->h)))
        return 0;
        else return 1;
}
