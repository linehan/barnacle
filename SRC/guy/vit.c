#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "../lib/redblack/rb.h"
#include "../eng/state.h"
#include "../lib/hash.h"
#include "../pan/test.h"
#include "../gfx/palette.h"
#include "../gfx/gfx.h"
#include "guy.h"
#include "vit.h"

static const short VIT_PAIR[]={PUR_RED, PUR_BRZ, PUR_BLU, PUR_GRE};
static const char *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};

static wchar_t VIT_WCH[]=L"▩▨▧▦▤▥▣□ⲾⲿⲺⲻ";
cchar_t VIT_CCH;

enum VIT_WCH_ENUM {VIT_BOX_DSTIP, VIT_BOX_MSTIP, VIT_BOX_LSTIP, 
                   VIT_BOX_CHECK, VIT_BOX_HLINE, VIT_BOX_VLINE, 
                   VIT_BOX_FILLED,VIT_BOX_EMPTY,
                   VIT_LG_VBAR, VIT_SM_VBAR, 
                   VIT_LG_HBAR, VIT_SM_HBAR};


inline void put_nblocks(WINDOW *win, int y, int x, int id_wch, short pair, int n)
{
        setcchar(&VIT_CCH, &VIT_WCH[id_wch], 0, pair, NULL);
        mvwhline_set(win, y, x, &VIT_CCH, n);
}


WINDOW *PULSE_WIN;
uint32_t display_this_key;

void vit_print_blocks(WINDOW *win, int xofs, uint32_t key)
{
        uint32_t vpkg;         // ptr to packaged vitals @ key
        int vital[VIT_NVITALS]; // array to hold unpacked vitals
        int i;
        uint8_t xpulse; // x-offset of pulse 

        if (PULSE_WIN == NULL) PULSE_WIN = win;

        focus(key);
        vpkg   = focused->vitals;
        xpulse = focused->xpulse;

        vit_unpack(&vpkg, vital, VIT_NVITALS);

        // Base layer 
        put_nblocks(win, 0, xofs, VIT_SM_HBAR, TITLE_SCREEN, VIT_MAXLEN);

        // Print blocks for vitals
        for (i=0; i<4; i++) {
                put_nblocks(win, 0, xofs, VIT_BOX_DSTIP, VIT_PAIR[i], vital[i]);
                xofs += vital[i];
        }

        display_this_key = key;
}


void vit_dopulse(void)
{
        uint32_t key;
        int xmin;
        int pulse;
        int i;
        bool positive;

        for (i=0; i<npersons(); i++) {
                if (muster[i] == NULL) continue;

                key         = muster[i];
                focus(key);
                pulse       = focused->xpulse;
                xmin        = vit_blocklen(focused->vitals);
                positive    = focused->positive;

                if (key == display_this_key) {
                        put_nblocks(PULSE_WIN, 0, xmin, VIT_SM_HBAR, TITLE_SCREEN, (VIT_MAXLEN-xmin));
                        put_nblocks(PULSE_WIN, 0, pulse, VIT_LG_HBAR, TITLE_SCREEN, 1);
                        win_refresh(PULSE_WIN);
                }

                if (positive == false) pulse--;
                if (positive == true)  pulse++; 

                if (pulse>VIT_MAXLEN-2) positive=false;
                if (pulse<=xmin)      positive=true;

                focused->xpulse   = pulse;
                focused->positive = positive;
        }
}
