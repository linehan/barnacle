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

#include "../lib/llist/list.h"

#include "test.h"

#include "instruments.h"
#include "../gen/dice.h"
#include "../gfx/gfx.h"
#include "../geo/weather.h"
#include "../mob/mob.h"
#include "../mob/boat.h"
#include "../eng/state.h"
//##############################################################################
enum boat_options {
        __now__ = 0, 
        __req__ = 1, 
        __buf__ = 2, 

        __hdg__ = 3
};

static int HDG_FOR_DA_BOAT;

// Circularly linked list of compass characters
struct compass_char {
        cchar_t cch; // one character of the compass ribbon display
        int dir;     // the direction that character represents
        struct list_node node;
};

enum gfxtags {DN=0,UP=1,GH=2,VR=0,HZ=1,TL=2,TR=3,BL=4,BR=5};
enum bordertags {LSIDE=0,RSIDE=1,BOTL=2,BOTR=3,BOTT=4};
enum compass_window_dims { 
        box_wid = 15, // width of entire compass window
        box_hlf = 8,  // "half" of the width of the compass window
        rib_wid = 13, // width of the compass ribbon (dial)
        rib_hlf = 7,  // "half" of the ribbon width
        rib_off = 1,  // offset of the ribbon from the box edge
        cmp_num = 16, // number of ribbon elements
        cmp_hlf = 8   // half the number of ribbon elements
};

static wchar_t gfxCBOR[] = L"▌▐▙▟▄";
static wchar_t gfxCBOX[] = L"┃─┎┒┖┚";
static wchar_t gfxMRK[] = L"▾▴";
static wchar_t gfxCMP[] = L"N⋅∙⋅E⋅∙⋅S⋅∙⋅W⋅∙⋅";
static wchar_t gfxCMPBG = L' ';

static cchar_t CMPBG;   // The background character
static cchar_t CBOR[6]; // One extra b/c the top is a different color
static cchar_t CBOX[6]; // Complex characters to draw the box
static cchar_t CMRK[3]; // Comprex characters to draw the ribbon

static LIST_HEAD(CMP);  // Head of the circularly linked list

static WINDOW *inst_win;   
static PANEL  *inst_pan;

static WINDOW *cmpbox_win; // Where to draw the compass box
static WINDOW *cmprib_win; // Where to draw the compass ribbon
static PANEL  *cmpbox_pan;
static PANEL  *cmprib_pan;

static int offsdn; // Offset of the heading marker
static int offsgh; // Lets the heading marker "reel" in
static int offsup; // Offset of the wind marker
//##############################################################################

void toggle_instrument_panel(void)
{
        TOGPAN(cmpbox_pan); 
        TOGPAN(cmprib_pan);
        vrt_refresh();
}

void init_instruments(void)
{
        int i;

        // Init all graphics and complex characters
        setcchar(&CMPBG,      &(gfxCMPBG),   0, PUR_GRE, NULL);
        setcchar(&(CMRK[DN]), &(gfxMRK[UP]), 0, CMP_GREEN,  NULL);
        setcchar(&(CMRK[GH]), &(gfxMRK[UP]), 0, PUR_GRE, NULL);
        setcchar(&(CMRK[UP]), &(gfxMRK[UP]), 0, CMP_YELLOW, NULL);

        cmpbox_win = newwin(3, box_wid, LINES-3, ((COLS/2)-(box_hlf)));
        cmprib_win = newwin(1, 13, LINES-2, ((COLS/2)-(rib_hlf)));
        cmpbox_pan = new_panel(cmpbox_win);
        cmprib_pan = new_panel(cmprib_win);

        wbkgrnd(cmpbox_win, &CMPBG);
        wbkgrnd(cmprib_win, &CMPBG);

        overlay(cmpbox_win, inst_win);
        overlay(cmprib_win, inst_win);

        offsdn = offsgh = offsup = 1; // offsets

        // Box and border characters
        for (i=0; i<5; i++) {
                setcchar(&(CBOX[i]), &(gfxCBOX[i]), 0, PUR_GRE, NULL);
                setcchar(&(CBOR[i]), &(gfxCBOR[i]), 0, PUR_GRE, NULL);
        }
        setcchar(&(CBOR[5]), &(gfxCBOR[BOTT]), 0, CMP_PINK, NULL); // extra
        wbkgrnd(inst_win, &CBOR[5]);

        // Paint the compass ribbon
        for (i=0; i<16; i++) {
                struct compass_char *new = malloc(sizeof(struct compass_char));
               
                switch (i%4) {
                case 0: // Cardinal directions
                        setcchar(&(new->cch), &gfxCMP[i], 0, PUR_GRE, NULL);
                        break;
                case 1: // Half winds (small dots)
                case 3:
                        setcchar(&(new->cch), &gfxCMP[i], 0, PUR_PUR, NULL);
                        break;
                case 2: // Ordinal directions (big dots)
                        setcchar(&(new->cch), &gfxCMP[i], 0, PUR_PUR, NULL);
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
        case 'L':       if (offsdn > 1) offsdn--;
                        break;
        case 'R':       if (offsdn < rib_wid) offsdn++;
                        break;
        case 'l':       if (offsgh > 1) offsgh--;
                        return;
        case 'r':       if (offsgh < rib_wid) offsgh++;
                        return;
        case 2:         break;
        }
        struct compass_char *tmp;
        tmp = list_top(&CMP, struct compass_char, node);
        int origin = tmp->dir;
        offsgh = offsdn;
        /*HDG_FOR_DA_BOAT = ((origin+(offsdn-1))%16);*/
        set_state(BOAT_TREE, BOAT_NODE->key, 0, HDG, (origin+(offsdn-1))%16);
}
void take_hdg(void)
{
        set_state(BOAT_TREE, BOAT_NODE->key, 0, HDG, HDG_FOR_DA_BOAT);
}
/* Calculate the offset of the wind arrow. This is not under the control of
 * the player; the mark will move to indicate the current wind direction on
 * the compass, and so requires no additional parameters */
void mark_wind(void)
{
        struct compass_char *tmp;
        int origin; // Wind direction at the origin of ribbon
        int cur;    // Current wind direction
        int delta;  // Distance between the origin and current wind direction

        tmp    = list_top(&CMP, struct compass_char, node);
        origin = tmp->dir;
        cur    = get_wind(__dir__);
        delta  = (17+(cur-origin))%16;

        // If the offset doesn't equal the delta, increment the marker offset
        if (offsup != delta) {
                if      ((offsup>delta) && (offsup>1))       offsup--;
                else if ((offsup<delta) && (offsup<rib_wid)) offsup++;
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
        int req = get_state(BOAT_TREE, BOAT_NODE->key, 0, HDG);
        int origin; // direction at origin
        int mid;    // direction at midpoint

        tmp = list_top(&CMP, struct compass_char, node);

        origin = tmp->dir; // value of first printed direction
        mid    = ((origin+6)%cmp_num); // compute direction @ midpoint

        /* Set the current heading to the midpoint */
        set_state(BOAT_TREE, BOAT_NODE->key, 0, HDG, mid);

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
        if (panel_hidden(cmpbox_pan)) return;

        int i = 0;
        struct compass_char *tmp;

        top_panel(cmpbox_pan);
        top_panel(cmprib_pan);

        /* Erase windows */
        werase(cmpbox_win);
        werase(cmprib_win);

        /* LINE 0 */
        mvwadd_wch(cmpbox_win, 2, offsgh, &(CMRK[GH]));
        mvwadd_wch(cmpbox_win, 2, offsdn, &(CMRK[DN]));
        mvwadd_wch(cmpbox_win, 0, 0, &(CBOR[LSIDE]));
        mvwadd_wch(cmpbox_win, 0, (box_wid-1), &(CBOR[RSIDE]));

        /* LINE 1 */
        mvwadd_wch(cmpbox_win, 1, 0, &(CBOR[LSIDE]));
        list_for_each(&CMP, tmp, node) {
                mvwadd_wch(cmprib_win, 0, i++, &(tmp->cch));
        }
        mvwadd_wch(cmpbox_win, 1, (box_wid-1), &(CBOR[RSIDE]));

        /* LINE 2 */
        mvwadd_wch(cmpbox_win, 2, (offsup), &(CMRK[UP]));
        mvwadd_wch(cmpbox_win, 2, 0, &(CBOR[LSIDE]));
        mvwadd_wch(cmpbox_win, 2, (box_wid-1), &(CBOR[RSIDE]));

        vrt_refresh();
        scr_refresh();
}
