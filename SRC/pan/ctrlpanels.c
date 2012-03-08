// vim:fdm=marker
/*******************************************************************************
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../gen/name/name.h"

#define W_BORD COLS
#define H_BORD 25 

#define X_PADD 2
#define Y_PADD 1
#define W_PADD W_BORD-(2*X_PADD)
#define H_PADD H_BORD-(2*Y_PADD) 

#define X_BORD 0
#define Y_BORD 0

#define X_BODY 2 
#define Y_BODY 3 
#define W_BODY W_PADD-(2*X_BODY)
#define H_BODY H_PADD-(2*Y_BODY)

#define X_HEAD 2
#define Y_HEAD 1
#define W_HEAD W_PADD-(2*X_HEAD)
#define H_HEAD H_PADD-(2*Y_HEAD)

WINDOW *INFOW[4];
PANEL  *INFOP[4];

enum info_panel_enum { BORD, PADD, HEAD, BODY };


void init_ctrlpanels(void)
{
        INFOW[BORD] = newwin(H_BORD, W_BORD, Y_BORD, X_BORD);
        INFOW[PADD] = derwin(INFOW[BORD], H_PADD, W_PADD, Y_PADD, X_PADD);
        INFOW[HEAD] = derwin(INFOW[PADD], H_HEAD, W_HEAD, Y_HEAD, X_HEAD);
        INFOW[BODY] = derwin(INFOW[PADD], H_BODY, W_BODY, Y_BODY, X_BODY);
        keypad(INFOW[BODY], 1);

        wbkgrnd(INFOW[BORD], &PURPLE[1]);
        wbkgrnd(INFOW[PADD], &PURPLE[2]);
        wbkgrnd(INFOW[HEAD], &PURPLE[2]);
        wbkgrnd(INFOW[BODY], &PURPLE[2]);

        INFOP[BORD] = new_panel(INFOW[BORD]);
        INFOP[PADD] = new_panel(INFOW[PADD]);
        INFOP[HEAD] = new_panel(INFOW[HEAD]);
        INFOP[BODY] = new_panel(INFOW[BODY]);

        hide_panel(INFOP[BORD]);
        hide_panel(INFOP[PADD]);
        hide_panel(INFOP[HEAD]);
        hide_panel(INFOP[BODY]);
}


PANEL *make_purple_panel(WINDOW *win[4], int h, int w, int y, int x)
{
        #define X_PADD 2
        #define Y_PADD 1
        #define PADD_W(w) w-(2*X_PADD)
        #define PADD_H(h) h-(2*Y_PADD) 

        #define X_BODY 2 
        #define Y_BODY 3 
        #define BODY_W(w) PADD_W(w)-(2*X_BODY)
        #define BODY_H(h) PADD_H(h)-(2*Y_BODY)

        #define X_HEAD 2
        #define Y_HEAD 1
        #define HEAD_W(w) PADD_W(w)-(2*X_HEAD)
        #define HEAD_H(h) PADD_H(h)-(2*Y_HEAD)

        win[BORD] = newwin(h, w, y, x);
        win[PADD] = derwin(win[BORD], PADD_H(h), PADD_W(w), Y_PADD, X_PADD);
        win[HEAD] = derwin(win[PADD], HEAD_H(h), HEAD_W(w), Y_HEAD, X_HEAD);
        win[BODY] = derwin(win[PADD], BODY_H(h), BODY_W(w), Y_BODY, X_BODY);
        keypad(win[BODY], 1);

        wbkgrnd(win[BORD], &PURPLE[1]);
        wbkgrnd(win[PADD], &PURPLE[2]);
        wbkgrnd(win[HEAD], &PURPLE[2]);
        wbkgrnd(win[BODY], &PURPLE[2]);

        PANEL *new = new_panel(win[BORD]);
        hide_panel(new);

        return (new);
}
