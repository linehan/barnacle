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

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"

#include "../cmd/control.h"
#include "../gen/dice.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../pan/test.h"

#include "weather.h"
/******************************************************************************/
ENV *hi; /* Temporary highlight environment list_head */
/* Temporary globals so I can keep track of where the background is. */
PANEL *BG;  
WINDOW *BGW;
/******************************************************************************/
/* Set a background GNODE as the bottom-most layer of the supplied list head. */
void set_gfx_bg(struct list_head *head, int opt)
{
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
 * Generate terrain body; conforms to a three-tier pattern, typically with the 
 * 'edge' panel being animated with two frames. This was an early attempt to do
 * something that might have been superceded by gen_terrain(). */
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
}
/* Generate random terrain, to be drawn with tiles of the indicated type, 
 * within a defined boundary box. */
void gen_terrain(ENV *env, char type, int h, int w, int y0, int x0)
{
        GNODE *gen   = new_gfx(__an__, h, w, y0, x0, 2);
        WNODE *genw1 = TOP_WNODE(gen);
        WNODE *genw2 = NXT_WNODE(gen);

        cchar_t *tile[2], *anim[2], *bg; /* Terrain graphics */

        switch (type) {
        case 'm':
                tile[0] = &MTN[1];
                tile[1] = &MTN[2];
                anim[0] = &OCEAN[2];
                anim[1] = &OCEAN[3];
                bg = &OCEAN[0];
        }
        int i, j, wid[h], ofs[h];
        int rnoise = 8; /* Determines the width */
        int lnoise = 4; /* Determins the left offset */
        int vnoise = 1; /* How "blocky" is the terrain. */
        /* Supply a background */
        wbkgrnd(genw1->window, bg);
        wbkgrnd(genw2->window, bg);
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
        }
        for (i=0; i<(h-2); i++) {
                /* Ensure that the maximum extent of the land is at most
                 * one less than the total window width, so that a shore
                 * graphic can be drawn to its right. */
                wid[i] = ((wid[i]+ofs[i]) >= w) ? (w-ofs[i])-1 : wid[i];
                /* Draw the "top" of the land */
                mvwhline_set(genw1->window, i, ofs[i]+1, tile[0], wid[i]-1);
                mvwhline_set(genw2->window, i, ofs[i]+1, tile[0], wid[i]-1);
                /* Draw the "drop" of the land */
                mvwhline_set(genw1->window, i+1, ofs[i]+1, tile[1], wid[i]-1);
                mvwhline_set(genw2->window, i+1, ofs[i]+1, tile[1], wid[i]-1);
                /* Draw the seashore to the left of the "drop" */
                mvwadd_wch(genw1->window, i+1, ofs[i], anim[0]);
                mvwadd_wch(genw2->window, i+1, ofs[i], anim[1]);
                /* Draw the seashore to the right of the "drop" */
                mvwadd_wch(genw1->window, i+1, ofs[i]+wid[i], anim[0]);
                mvwadd_wch(genw2->window, i+1, ofs[i]+wid[i], anim[1]);
                /* Draw the seashore across the bottom of the "drop" */
                mvwhline_set(genw1->window, i+2, ofs[i], anim[0], wid[i]);
                mvwhline_set(genw2->window, i+2, ofs[i], anim[1], wid[i]);
        }
        add_gfx(gen, &env->wad);
}
/* Allocate a new environment to store the terrain highlights */
void highlights_init(void)
{
        int i, w, wnoise, x0, xnoise, y0, ynoise;
        wnoise = 50;
        xnoise = 50;
        ynoise = 80;

        hi = new_env();

        for (i=0; i<5; i++) {

                 w = roll_fair(wnoise);
                x0 = roll_fair(xnoise);
                y0 = roll_fair(ynoise);

                GNODE *new = new_gfx(__hi__, 1, w, y0, x0, 1);
                WNODE *wnode = TOP_WNODE(new);
                wbkgrnd(wnode->window, &OCEAN[2]);

                bottom_panel(new->pan);
                add_gfx(new, &hi->wad);
        }
        bottom_panel(BG);
        master_refresh();
}
/* Move terrain highlights in a direction determined by the current wind
 * direction. */
void shift_highlights(void)
{
        if ((hi == NULL)) highlights_init();

        int wind = get_wind(__dir__);

        GNODE *tmp;
        list_for_each(&hi->wad, tmp, node) {
                if ((((tmp->dim.x0)+(tmp->dim.w)) > COLS)) 
                        tmp->dim.x0 = 0;
                if (((tmp->dim.x0) < 0)) 
                        tmp->dim.x0 = COLS-(tmp->dim.w);
                if ((((tmp->dim.y0)+(tmp->dim.h)) > LINES))
                        tmp->dim.y0 = 0;
                if (((tmp->dim.y0) < 0))
                        tmp->dim.y0 = LINES;

                switch(wind) {
                case NORTH:
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        break;
                case NE: 
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case EAST:
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case SE:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case SOUTH:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        break;
                case SW:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                case WEST:
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                case NW:
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                }
                move_panel(tmp->pan, tmp->dim.y0, tmp->dim.x0);
        }
        master_refresh();
}
