#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>

#include "../../pan/test.h"
#include "../../gfx/palette.h"
#include "../../gfx/gfx.h"
#include "../model/guy.h"
#include "../model/vitals.h"



static const short VIT_PAIR[]={PUR_RED, PUR_BRZ, PUR_BLU, PUR_GRE};
static const char *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};
static wchar_t VIT_WCH[]=L"▩▨▧▦▤▥▣□ⲾⲿⲺⲻ";
cchar_t VIT_CCH;

enum VIT_WCH_ENUM {BOX_DSTIP, BOX_MSTIP, BOX_LSTIP, 
                   BOX_CHECK, BOX_HLINE, BOX_VLINE, 
                   BOX_FILLED,BOX_EMPTY, 
                   LG_VBAR,   SM_VBAR,   LG_HBAR,  SM_HBAR};



WINDOW *PULSE_WIN;
WINDOW *VITALS_WIN;

uint32_t active_key;



void init_vitals(WINDOW *vitalswin, WINDOW *pulsewin)
{
        PULSE_WIN  = pulsewin;
        VITALS_WIN = vitalswin;
}


inline void put_nblocks(WINDOW *win, int y, int x, int id_wch, short pair, int n)
{
        setcchar(&VIT_CCH, &VIT_WCH[id_wch], 0, pair, NULL);
        mvwhline_set(win, y, x, &VIT_CCH, n);
}



void vit_print_blocks(WINDOW *win, int xofs, uint32_t key)
{
        uint32_t vpkg;         // ptr to packaged vitals @ key
        int vital[NBYTES]; // array to hold unpacked vitals
        int i;
        uint8_t xpulse; // x-offset of pulse 

        if (PULSE_WIN == NULL || VITALS_WIN == NULL) init_vitals(win, win);

        focus(key);
        vpkg   = focused->vitals;
        xpulse = focused->xpulse;

        unpack_vitals(key, vital);

        // Base layer 
        put_nblocks(VITALS_WIN, 0, xofs, SM_HBAR, TITLE_SCREEN, VIT_MAXLEN);

        // Print blocks for vitals
        for (i=0; i<4; i++) {
                put_nblocks(VITALS_WIN, 0, xofs, BOX_DSTIP, VIT_PAIR[i], vital[i]);
                xofs += vital[i];
        }

        active_key = key;
}


void vit_dopulse(void)
{
        if (PULSE_WIN == NULL || VITALS_WIN == NULL) return;

        uint32_t key;
        int xmin;
        int pulse;
        uint32_t action;
        int i;
        bool positive;
        int max = nguys();

        for (i=0; i<max; i++) {
                key         = muster[i];
                focus(key);
                pulse       = focused->xpulse;
                xmin        = vit_blocklen(key);
                positive    = focused->positive;
                action      = focused->action;

                if (key == active_key) {
                        put_nblocks(PULSE_WIN, 0, xmin, SM_HBAR, TITLE_SCREEN, (VIT_MAXLEN-xmin));
                        if (action != 0) {
                                if (positive == true)
                                        put_nblocks(PULSE_WIN, 0, pulse+1, SM_HBAR, PUR_BLU, 1);
                                else
                                        put_nblocks(PULSE_WIN, 0, pulse-1, SM_HBAR, PUR_BLU, 1);
                        }
                        put_nblocks(PULSE_WIN, 0, pulse, LG_HBAR, TITLE_SCREEN, 1);
                        win_refresh(PULSE_WIN);
                }

                if (positive == false) pulse--;
                if (positive == true)  pulse++; 

                if (pulse>VIT_MAXLEN-2) positive=false;
                if (pulse<=xmin)        positive=true;

                if (pulse == VIT_MAXLEN-1 || pulse == xmin) action = 0;

                focused->xpulse   = pulse;
                focused->positive = positive;
                focused->action = action;
        }
}
