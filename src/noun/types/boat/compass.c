#include "boat.h"
#include "../../../map/weather.h"


enum gfxtags {DN=0,UP=1,GH=2,VR=0,HZ=1,TL=2,TR=3,BL=4,BR=5};
enum bordertags {LSIDE=0,RSIDE=1,BOTL=2,BOTR=3,BOTT=4};

#define BOX_W      15 // width of entire compass window
#define BOX_CENTER  8 // "half" of the width of the compass window
#define RIB_W      13 // width of the compass ribbon (dial)
#define RIB_CENTER  7 // "half" of the ribbon width
#define RIB_LEN    16 
#define RIB_WIN_W  15

wchar_t gfxRIB[] = L"N⋅∙⋅E⋅∙⋅S⋅∙⋅W⋅∙⋅";
cchar_t RIB[RIB_LEN];
wchar_t gfxCBOR[] = L"▌▐";
wchar_t gfxMRK[] = L"▾▴";
wchar_t gfxCMPBG = L' ';

cchar_t CMPBG;   // The background character
cchar_t CBOR[2]; // One extra b/c the top is a different color
cchar_t CMRK[3]; // Comprex characters to draw the ribbon

WINDOW *inst_win, *cmpbox_win, *cmprib_win;   
PANEL  *inst_pan, *cmpbox_pan, *cmprib_pan;

static int offsdn; // Offset of the heading marker
static int offsgh; // Lets the heading marker "reel" in
static int offsup; // Offset of the wind marker

static uint32_t current_heading;
static uint32_t middle_heading;
static uint32_t current_helm;
static uint32_t make_helm;
static uint32_t middle_helm;





void set_compass_heading(uint32_t hdg)
{
        current_heading = hdg;
        middle_heading = ((RIB_LEN+hdg)-RIB_CENTER) % RIB_LEN;
}




void set_compass_helm(uint32_t helm)
{
        make_helm = helm;
}




void init_instruments(void)
{
        int i;

        setcchar(&CMPBG,   &gfxCMPBG, 0, PUR_GRE, NULL);
        setcchar(&CBOR[0], &gfxCBOR[0], 0, PUR_GRE, NULL);
        setcchar(&CBOR[1], &gfxCBOR[1], 0, PUR_GRE, NULL);
        setcchar(&(CMRK[DN]), &(gfxMRK[UP]), 0, CMP_GREEN,  NULL);
        setcchar(&(CMRK[GH]), &(gfxMRK[UP]), 0, PUR_GRE, NULL);
        setcchar(&(CMRK[UP]), &(gfxMRK[UP]), 0, CMP_YELLOW, NULL);

        for (i=0; i<RIB_LEN; i++) {
                if (gfxRIB[i] == L'⋅' || gfxRIB[i] == L'∙')
                        setcchar(&RIB[i], &gfxRIB[i], 0, PUR_PURPLE, NULL);
                else
                        setcchar(&RIB[i], &gfxRIB[i], 0, PUR_GRE, NULL);
        }

        cmpbox_win = newwin(3, BOX_W, LINES-3, ((COLS/2)-(BOX_CENTER)));
        cmprib_win = newwin(1, 13, LINES-2, ((COLS/2)-(RIB_CENTER)));
        cmpbox_pan = new_panel(cmpbox_win);
        cmprib_pan = new_panel(cmprib_win);

        wbkgrnd(cmpbox_win, &CMPBG);
        wbkgrnd(cmprib_win, &CMPBG);

        overlay(cmpbox_win, inst_win);
        overlay(cmprib_win, inst_win);

        current_heading = offsgh = offsup = 1; // offsets

        toggle_instrument_panel();
}


void toggle_instrument_panel(void)
{
        if (cmpbox_pan == NULL) init_instruments();
        else {
                TOGPAN(cmpbox_pan); 
                TOGPAN(cmprib_pan);
                vrt_refresh();
        }
}


/* Calculate the offset of the wind arrow. This is not under the control of
 * the player; the mark will move to indicate the current wind direction on
 * the compass, and so requires no additional parameters */
void mark_wind(void)
{
        int origin; // Wind direction at the origin of ribbon
        int cur;    // Current wind direction
        int delta;  // Distance between the origin and current wind direction

        origin = middle_heading;
        cur    = 3; /*get_wind(__dir__);*/
        delta  = (17+(cur-origin))%16;

        // If the offset doesn't equal the delta, increment the marker offset
        if (offsup != delta) {
                if      ((offsup>delta) && (offsup>1))     offsup--;
                else if ((offsup<delta) && (offsup<RIB_W)) offsup++;
        }
}


void approach_helm(void)
{
        #define DELAY_FACTOR 0.02
        #define DELAY_MAX 0.04
        #define BUF_FULL 3.0 
        #define BUF_STEP 0.8 
        #define BUF_REST 0.5
        #define BUF_MIN 0.5

        static float buffer;
        int offset = 0;
        static int dir;

        if (make_helm > current_helm) current_helm++;
        if (make_helm < current_helm) current_helm--;

        if (current_helm>RIB_CENTER) {
                offset = current_helm - RIB_CENTER;
                dir = TURN_L;
        }
        if (current_helm<RIB_CENTER) {
                offset = RIB_CENTER - current_helm;
                dir = TURN_R;
        }

        if (offset!=0) buffer -= (buffer > BUF_MIN) ? BUF_STEP : 0;

        if (buffer < BUF_FULL) buffer = (buffer+BUF_REST);

                if (buffer < BUF_MIN && offset != 0 ) {
                        change_course(get_boat("Afarensis"), dir^1);
                        turn_helm(get_boat("Afarensis"), dir);
                }
}

        




/* Allow the current wind direction to seek the prevailing wind direction. */
int seek_prevailing(void)
{
        /*int pre = get_wind(__pre__);*/
        /*int cur = get_wind(__dir__);*/
        /*int pre = 3;*/
        /*int cur = 3;*/

        /*if (cur != pre) {*/
                /*if (cur < pre)*/
                        /*set_wind(__dir__, ((cur+1)%16));*/
                /*else if (cur > pre)*/
                        /*set_wind(__dir__, ((16+(cur-1))%16)); */
        /*}*/
        return 0;
}



/* Draw the compass panel */
void draw_compass(void)
{
        int i;

        if (panel_hidden(cmpbox_pan)) return;

        top_panel(cmpbox_pan);
        top_panel(cmprib_pan);

        werase(cmprib_win);
        werase(cmpbox_win);

        /* LINE 0 */
        mvwadd_wch(cmpbox_win, 0, 0, &CBOR[LSIDE]);
        mvwadd_wch(cmpbox_win, 0, BOX_W-1, &CBOR[RSIDE]);
        mvwadd_wch(cmpbox_win, 2, offsup, &(CMRK[DN]));

        /* LINE 1 */
        mvwadd_wch(cmpbox_win, 1, 0, &CBOR[LSIDE]);
        for (i=0; i<RIB_W; i++) {
                wadd_wch(cmprib_win, &RIB[((middle_heading+i)%RIB_LEN)]);
        }
        mvwadd_wch(cmpbox_win, 1, (BOX_W-1), &CBOR[RSIDE]);

        /* LINE 2 */
        mvwadd_wch(cmpbox_win, 2, current_helm, &CMRK[UP]);
        mvwadd_wch(cmpbox_win, 2, 0, &CBOR[LSIDE]);
        mvwadd_wch(cmpbox_win, 2, (BOX_W-1), &CBOR[RSIDE]);

        vrt_refresh();
        scr_refresh();
}

