// vim:fdm=marker
/*******************************************************************************
 FILENAME:  instruments.c
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"

#include "test.h"

#include "instruments.h"
#include "../gen/dice.h"
#include "../geo/weather.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../mob/mob.h"
#include "../mob/boat.h"

/******************************************************************************/
struct compass_char {
        cchar_t cch;
        int dir;
        struct list_node node;
};

enum gfxtags { DN=0,UP=1,GH=2,VR=0,HZ=1,TL=2,TR=3,BL=4,BR=5 };
enum bordertags { LSIDE=0,RSIDE=1,BOTL=2,BOTR=3,BOT=4 };
enum compass_window_dims { 
        box_wid = 15, /* compass box width */
        box_hlf = 8,  /* compass box half width */
        rib_wid = 13, /* compass ribbon width */
        rib_hlf = 7,  /* compass ribbon half width */
        rib_off = 1,  /* compass ribbon offset */
        cmp_num = 16, /* number of compass elements */
        cmp_hlf = 8   /* half the number of compass elements */
};

 /*▐▂▂▂▌*/
/*▌▐▙▟*/
static wchar_t gfxCBOR[] = L"▌▐▙▟▄";
static wchar_t gfxCBOX[] = L"┃─┎┒┖┚";
static wchar_t gfxMRK[] = L"▾▴";
static wchar_t gfxCMP[] = L"N⋅∙⋅E⋅∙⋅S⋅∙⋅W⋅∙⋅";
static wchar_t gfxCMPBG = L' ';

static cchar_t CMPBG;
static cchar_t CBOR[6]; /* One extra b/c the top is a different color */
static cchar_t CBOX[6];
static cchar_t CMRK[3];

static LIST_HEAD(CMP);

static WINDOW *inst_win;   /* the instrument window */
static PANEL  *inst_pan;   /* the instrument panel */

static WINDOW *cmpbox_win; /* the compass box window */
static WINDOW *cmprib_win; /* the compass ribbon window */
static PANEL  *cmpbox_pan; /* the compass box panel */
static PANEL  *cmprib_pan; /* the compass ribbon panel */

static int offsdn; /* the heading mark offset */
static int offsgh; /* lets the heading mark reel back in */
static int offsup; /* the wind mark offset */
/******************************************************************************/
void toggle_instrument_panel(void)
{
        TOGPAN(cmpbox_pan); 
        vrt_refresh();
        TOGPAN(cmprib_pan);
        vrt_refresh();
        /*TOGPAN(inst_pan);*/
}

void init_instruments(void)
{
        int i;

        /* Init graphics and color palette */
        setcchar(&CMPBG,      &(gfxCMPBG),   0, CMP_SHADOW, NULL);
        setcchar(&(CMRK[DN]), &(gfxMRK[DN]), 0, CMP_GREEN, NULL);
        setcchar(&(CMRK[GH]), &(gfxMRK[DN]), 0, CMP_SHADOW, NULL);
        setcchar(&(CMRK[UP]), &(gfxMRK[UP]), 0, CMP_YELLOW, NULL);

        inst_win = newwin(1, COLS, 0, 0);

        inst_pan = new_panel(inst_win);

        cmpbox_win = newwin(5, box_wid, 0, ((COLS/2)-(box_hlf)));
        cmprib_win = newwin(1, 13, 2, ((COLS/2)-(rib_hlf)));

        wbkgrnd(cmpbox_win, &CMPBG);
        wbkgrnd(cmprib_win, &CMPBG);
        cmpbox_pan = new_panel(cmpbox_win);
        cmprib_pan = new_panel(cmprib_win);

        overlay(cmpbox_win, inst_win);
        overlay(cmprib_win, inst_win);

        offsup = 1;
        offsdn = 1;
        offsgh = 1;

        for (i=0; i<5; i++) {
                setcchar(&(CBOR[i]), &(gfxCBOR[i]), 0, CMP_ORANGE, NULL);
        }
        setcchar(&(CBOR[5]), &(gfxCBOR[BOT]), 0, CMP_PINK, NULL);
        wbkgrnd(inst_win, &CBOR[5]);
        /* Paint the compass box elements */
        for (i=0; i<5; i++) {
                setcchar(&(CBOX[i]), &(gfxCBOX[i]), 0, CMP_ORANGE, NULL);
        }

        /* Paint the compass ribbon elements */
        for (i=0; i<16; i++) {
               struct compass_char *new = MALLOC(struct compass_char);
               if (new == NULL) perror ("compass_char mis-allocated!");
               
               switch (i%4) {
               case 0: /* The cardinal directions */
                       setcchar(&(new->cch), &gfxCMP[i], 0, CMP_WHITE, NULL);
                       break;
               case 1: /* The half winds (small dots) */
               case 3:
                       setcchar(&(new->cch), &gfxCMP[i], 0, CMP_BEIGE, NULL);
                       break;
               case 2: /* The ordinal directions */
                       setcchar(&(new->cch), &gfxCMP[i], 0, CMP_ORANGE, NULL);
                       break;
               }
               new->dir = i;
               list_add_tail(&CMP, &(new->node));
        }
        toggle_instrument_panel();
}
/* Moves the heading marker (top arrow) on the compass, to indicate what the
 * player's orders might be, were he to commit them. Allows the "selection"
 * of a new heading. Accepts a directional parameter */
void mark_hdg(int dir)
{
        switch (dir) {
        case 'L':
                if (offsdn > 1)
                        offsdn--;
                break;
        case 'R':
                if (offsdn < rib_wid)
                        offsdn++;
                break;
        case 'l':
                if (offsgh > 1)
                        offsgh--;
                return;
        case 'r':
                if (offsgh < rib_wid)
                        offsgh++;
                return;
        case 2:
                break;
        }
        struct compass_char *tmp;
        tmp = list_top(&CMP, struct compass_char, node);
        int origin = tmp->dir;
        offsgh = offsdn;
        set_boat(_BOAT, __buf__, __hdg__, (origin+(offsdn-1))%16);

}
/* Calculate the offset of the wind arrow. This is not under the control of
 * the player; the mark will move to indicate the current wind direction on
 * the compass, and so requires no additional parameters */
void mark_wind(void)
{
        /* The direction at the origin */
        struct compass_char *tmp;
        tmp = list_top(&CMP, struct compass_char, node);
        int origin = tmp->dir;
        /* The current wind direction */
        int cur = get_wind(__dir__);
        /* The distance between the origin and the current wind direction */
        int delta = (17+(cur-origin))%16;

        /* If the offset does not equal the delta, move the offset */
        if (offsup != delta) {
                if ((offsup > delta) && (offsup > 1))
                        offsup--;
                else if ((offsup < delta) && (offsup < rib_wid))
                        offsup++;
        }
}
/* Tumble the compass ribbon left or right */
void tumble_compass(int dir)
{
        struct compass_char *tmp;

        switch (dir) {
        case 'L':
                tmp = list_top(&CMP, struct compass_char, node);
                      list_del_from(&CMP, &(tmp->node));
                      list_add_tail(&CMP, &(tmp->node));
                break;
        case 'R':
                tmp = list_tail(&CMP, struct compass_char, node);
                      list_del_from(&CMP, &(tmp->node));
                      list_add(&CMP, &(tmp->node));
                break;
        }
}
/* Tumble the compass ribbon left or right */
int seek_heading(void)
{
        struct compass_char *tmp;
        int req = get_boat(_BOAT, __req__, __hdg__);
        int origin;

        /* Get the value of the first printed direction */
        tmp = list_top(&CMP, struct compass_char, node);
        origin = tmp->dir;

        /* From that, calculate the direction at the mid point */
        int mid = ((origin+6)%cmp_num);

        /* Set the current heading to the midpoint */
        set_boat(_BOAT, __now__, __hdg__, mid); 

        /* If the midpoint is not the required heading, tumble the compass, 
         * which will cause the value of the midpoint to be shifted one way 
         * or another. */
        if (req != mid) {
                /* If the offset of the heading command arrow is on the
                 * left side of the compass, tumble toward the right; if
                 * it's on the right side, tumble toward the left. Cheats. */
                if (offsdn < 7) {
                        tumble_compass('R');
                        mark_hdg('R');
                } else {
                        tumble_compass('L');
                        mark_hdg('L');
                }
        }
        return 0;
}
/* Allow the current wind direction to seek the prevailing wind direction. */
int seek_prevailing(void)
{
        int pre = get_wind(__pre__);
        int cur = get_wind(__dir__);

        if (cur != pre) {
                if (cur < pre)
                        set_wind(__dir__, ((cur+1)%16));
                else if (cur > pre)
                        set_wind(__dir__, ((16+(cur-1))%16)); 
        }
        return 0;
}
/* Draw the compass panel */
void draw_compass(void)
{
        int i = 0;

        struct compass_char *tmp;

        if (!(panel_hidden(cmpbox_pan))) {
        /* Make sure panels are on top */
        top_panel(cmpbox_pan);
        top_panel(cmprib_pan);
        top_panel(inst_pan);
        }

        /* Erase windows */
        werase(cmpbox_win);
        werase(cmprib_win);

        /* LINE 0 */
        mvwhline_set(cmpbox_win, 0, 0, &CBOR[5], box_wid);
        mvwadd_wch(cmpbox_win, 1, offsgh, &(CMRK[GH]));
        mvwadd_wch(cmpbox_win, 1, offsdn, &(CMRK[DN]));
        /*mvwadd_wch(cmpbox_win, 0, 0, &(CBOX[TL]));*/
        mvwadd_wch(cmpbox_win, 1, 0, &(CBOR[LSIDE]));
        /*mvwadd_wch(cmpbox_win, 0, (box_wid-1), &(CBOX[TR]));*/
        mvwadd_wch(cmpbox_win, 1, (box_wid-1), &(CBOR[RSIDE]));

        /* LINE 1 */
        /*mvwadd_wch(cmpbox_win, 1, 0, &(CBOX[VR]));*/
        mvwadd_wch(cmpbox_win, 2, 0, &(CBOR[LSIDE]));
        list_for_each(&CMP, tmp, node) {
                mvwadd_wch(cmprib_win, 0, i++, &(tmp->cch));
        }
        /*mvwadd_wch(cmpbox_win, 1, (box_wid-1), &(CBOX[VR]));*/
        mvwadd_wch(cmpbox_win, 2, (box_wid-1), &(CBOR[RSIDE]));

        /* LINE 2 */
        mvwadd_wch(cmpbox_win, 3, (offsup), &(CMRK[UP]));
        mvwadd_wch(cmpbox_win, 3, 0, &(CBOR[LSIDE]));
        mvwadd_wch(cmpbox_win, 3, (box_wid-1), &(CBOR[RSIDE]));
        /*mvwadd_wch(cmpbox_win, 2, 0, &(CBOX[BL]));*/
        mvwadd_wch(cmpbox_win, 4, 0, &(CBOR[BOTL]));
        mvwhline_set(cmpbox_win, 4, 1, &CBOR[BOT], box_wid-2);
        /*mvwadd_wch(cmpbox_win, 2, (box_wid-1), &(CBOX[BR]));*/
        mvwadd_wch(cmpbox_win, 4, (box_wid-1), &(CBOR[BOTR]));
        vrt_refresh();

        scr_refresh();
}
