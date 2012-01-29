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
#include <math.h>

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"
#include "../lib/hash/hash.h"

#include "../cmd/control.h"
#include "../gen/dice.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../pan/test.h"
#include "../gen/perlin.h"

#include "weather.h"
/******************************************************************************/
PLATE *hi; /* Temporary highlight environment list_head */
/******************************************************************************/
/******************************************************************************
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
 * something that might have been superceded by gen_terrain().
 ******************************************************************************/
void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0)
{
        /* Apply landscape style */
        if ((type=='s')) {
                /* Create nested GNODEs */
                GNODE *edge = new_gnode(__an__, h, w, y0, x0, 2);
                GNODE *mant = new_gnode(__an__, (h-2), (w-2), (y0+1), (x0+1), 1);
                GNODE *core = new_gnode(__an__, (h-4), (w-4), (y0+2), (x0+2), 1);

                wbkgrnd(core->W->window, &SAND);
                wbkgrnd(mant->W->window, &SHORE);

                wbkgrnd(edge->W->window, &OCEAN[2]);
                edge->nextw(edge);
                wbkgrnd(edge->W->window, &OCEAN[1]);

                /* Add the GNODEs to the wad */
                list_add(head, &edge->node);
                list_add(head, &mant->node);
                list_add(head, &core->node);
        }
}
/******************************************************************************
 * Generates a GNODE containing a pseudo-random terrane, and adds it to the
 * supplied PLATE. 
 *
 * The supplied dimensions of height and width define the bounding box which
 * contains the terrane, not the actual dimensions of the terrane itself, as
 * seen on the screen. In particular, when there is a transition between
 * the terrane and the background, such as a coast, or a z-level, the terrane
 * will measure at most (width-2) and (height-2). The composition of the
 * bounded area is up to the "random" aspect, and the same bounding box will
 * produce many different-looking shapes, of course.
 *
 * The option 'type' defines the tileset to be used.
 ******************************************************************************/
void gen_terrain(PLATE *pl, char type, int h, int w, int y0, int x0)
{
        GNODE *gen   = new_gnode(__an__, h, w, y0, x0, 2);
        WNODE *genw1 = gen->W;
        gen->nextw(gen);
        WNODE *genw2 = gen->W;

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
        vrt_refresh();
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
        int wSUB2 = w-2;
        int hSUB2 = h-2;

        /* These must be set every loop */
        int iADD1, iSUB1, iADD2; 
        int ofsADD1, widSUB1, widADDofs;

        for (i=0; i<(h-2); i++) {
                /* Ensure that the maximum extent of the land is at most
                 * one less than the total window width, so that a shore
                 * graphic can be drawn to its right. */
                wid[i] = (wid[i] > (wSUB2-ofs[i])) ? (wSUB2-ofs[i]) : wid[i];
                /* Pre-compute repeated values to trim 
                 * some arithmetic in the loop. (19 ops -> 5 ops) */
                iADD1 = i+1;
                iADD2 = i+2;
                ofsADD1 = ofs[i]+1;
                widSUB1 = wid[i]-1;
                widADDofs = wid[i]+ofs[i];
                /* Draw the "top" of the land */
                mvwhline_set(genw1->window, i, ofsADD1, tile[0], widSUB1);
                mvwhline_set(genw2->window, i, ofsADD1, tile[0], widSUB1);
                /* Draw the "drop" of the land */
                mvwhline_set(genw1->window, iADD1, ofsADD1, tile[1], widSUB1);
                mvwhline_set(genw2->window, iADD1, ofsADD1, tile[1], widSUB1);
                /* Draw the seashore to the left of the "drop" */
                mvwadd_wch(genw1->window, iADD1, ofs[i], anim[0]);
                mvwadd_wch(genw2->window, iADD1, ofs[i], anim[1]);
                /* Draw the seashore to the right of the "drop" */
                mvwadd_wch(genw1->window, iADD1, widADDofs, anim[0]);
                mvwadd_wch(genw2->window, iADD1, widADDofs, anim[1]);
                /* Draw the seashore across the bottom of the "drop" */
                mvwhline_set(genw1->window, iADD2, ofs[i], anim[0], wid[i]);
                mvwhline_set(genw2->window, iADD2, ofs[i], anim[1], wid[i]);
                vrt_refresh();
        }
        list_add(pl->gfx, &gen->node);
}
/* Allocate a new environment to store the terrain highlights */
void highlights_init(PLATE *pl)
{
        int i, w, wnoise, x0, xnoise, y0, ynoise;
        wnoise = 50;
        xnoise = 50;
        ynoise = 80;

        hi = new_plate(__blank__);

        cchar_t *windowtile = bg_tile(__ocean__);
        for (i=0; i<5; i++) {

                 w = roll_fair(wnoise);
                x0 = roll_fair(xnoise);
                y0 = roll_fair(ynoise);

                GNODE *new = new_gnode(__hi__, 1, w, y0, x0, 1);

                wbkgrnd(new->W->window, windowtile);
                bottom_panel(new->pan);
                list_add(hi->gfx, &new->node);
                vrt_refresh();
        }
        /*bottom_panel(env->bgp);*/
}
/* Move terrain highlights in a direction determined by the current wind
 * direction. */
void shift_highlights(void)
{
        int wind = get_wind(__dir__);

        GNODE *tmp;
        list_for_each(hi->gfx, tmp, node) {
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
                vrt_refresh();
        }
        scr_refresh();
}
        
        
typedef struct blob_t { 
        int parent; 
        int n; /* the number of members in the blob */
        int y0;
        int x0;
        int y;
        int x;
        struct list_node node;
} BLOB;

/*{{{1*/
/*int merge_sets(BLOB *a, BLOB *b)*/
/*{*/
        /*[> Whichever has the least members rides bitch <]*/
        /*BLOB *dom = (a->n   >  b->n) ? a : b;*/
        /*BLOB *sub = (dom->n == b->n) ? a : b;*/

        /*[> Add elements of sub to dom <]*/
        /*dom->n += sub->n;*/
        /*[> Adjust ymin, xmin, xmax (ymax is always progressing)<]*/
        /*dom->y0 = (dom->y0 > sub->y0) ? sub->y0 : dom->y0;*/
        /*dom->x0 = (dom->x0 > sub->x0) ? sub->x0 : dom->x0;*/
        /*dom->x = (dom->x < sub->x) ? sub->x : dom->x;*/

        /*[> free the subsumed data <]*/
        /*free(sub);*/

        /*[> Set the 'sub' set's pointer to the 'dom' set <]*/
        /*sub = dom;*/

        /*return dom->blob_id;*/
/*}*/

/*BLOB *new_blob(int x)*/
/*{*/
        /*BLOB *new = malloc(sizeof(BLOB));*/
        /*new->parent = x;*/
        /*new->rank = 0;*/

        /*return new;*/
/*}*/

/*BLOB *look_up(int x)*/
/*{*/
        /*if (blob[x]->parent != x)*/
                /*blob[x] = look_up(blob[x]->parent);*/
        /*return blob[x];*/
/*}*/



/*void union_blob(BLOB *a, BLOB *b)*/
/*{*/



 /*function Union(x, y)*/
     /*xRoot := Find(x)*/
     /*yRoot := Find(y)*/
     /*if xRoot == yRoot*/
         /*return*/

     /*if xRoot.rank < yRoot.rank*/
         /*xRoot.parent := yRoot*/
     /*else if xRoot.rank > yRoot.rank*/
         /*yRoot.parent := xRoot*/
     /*else*/
         /*yRoot.parent := xRoot*/
         /*xRoot.rank := xRoot.rank + 1*/
/*}}}1*/

void merge_um(BLOB *a, BLOB *b)
{
        /*a->n += b->n;*/
        a->x0 = (a->x0 > b->x0) ? b->x0 : a->x0;
        a->y0 = (a->y0 > b->y0) ? b->y0 : a->y0;

        b = a;
}


void new_gen_terrain(PLATE *pl, char type)
{

        wprintw(BIGWIN, "LATER...\nh = %d\n w = %d\n len = %d\n\n", 
                        pl->noise->h, pl->noise->w, pl->noise->len);

        int x, y;
        /* The width of one row of the plate */
        int w = pl->noise->w;
        int h = pl->noise->h;

        int tot = 0;
        int lft = 0;

        int *buf = calloc(w, sizeof(int));

        pl->noise->bound = -0.8;

        LIST_HEAD(bhead);

        /* A lookup table (array of pointers to BLOBS) */
        /*BLOB *blob[w*h];*/

        for (y=0; y<h; y++) {
                lft = 0;
                for (x=0; x<w; x++) {

                        /* If it's land */
                        if (pl->noise->box[y][x] > pl->noise->bound) {

                                /* If we have no neighbors W or N, we are a new set */
                                if ((lft == 0) && (buf[x] == 0)) {
                                        tot += 1;

                                        blob[tot] = malloc(sizeof(BLOB));
                                        blob[tot] = &(BLOB){tot, 1, y, x, y, x};

                                        buf[x] = tot;

                                        lft = tot;
                                } 
                                /* Neither are zero but they're both different */
                                else if ((lft!=0)&&(buf[x]!=0)&&(lft!=buf[x])) {

                                        merge_um(blob[lft], blob[(buf[x])]);
                                        
                                        /*blob[lft]->n += 1;*/
                                        blob[lft]->x = x;

                                        buf[x] = lft;
                                }

                                /* If the one to the west is a set */                
                                else if (lft != 0) {

                                        /*blob[lft]->n += 1;*/
                                        blob[lft]->x = x;

                                        buf[x] = lft;

                                }
                                /* If the one on the north is a set */
                                else if (buf[x] != 0) {

                                        /*blob[buf[x]]->n += 1;*/
                                        blob[buf[x]]->y += 1;

                                        lft = buf[x];
                                }
                        }
                        /* If it isn't land */
                        else {
                                lft = 0;                 
                                buf[x] = 0;
                         }
                }
        }
        wprintw(BIGWIN, "Height: %d\n"
                        "Width:  %d\n"
                        "%d blobs, after %d cells scanned.\n", 
                        pl->noise->h, pl->noise->w, tot, w*h); 
}




        

        

