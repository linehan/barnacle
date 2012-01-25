// vim:fdm=marker
/*******************************************************************************
 FILENAME: test.c
 Provides a diagnostic window for diagnostic output, and connects it to
 a panel which can be toggled.
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
/******************************************************************************/
WINDOW *DIAGNOSTIC_WIN;
PANEL *DIAGNOSTIC_PAN;

static int counter = 0;
/******************************************************************************/
/* Create the diagnostic window */
void test_init(void)
{
	DIAGNOSTIC_WIN = newwin((LINES-30), (COLS/3), 15, 0);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);
        hide_panel(DIAGNOSTIC_PAN);
}
/* Toggle the diagnostic panel's visibility */
void toggle_dpanel(int opt)
{
        if ((panel_hidden(DIAGNOSTIC_PAN)))                 
                show_panel(DIAGNOSTIC_PAN);
        else 
                hide_panel(DIAGNOSTIC_PAN);
}
/* show stuff */
/*void instrument_panel(void)*/
/*{*/
        /*int hdg = get_boat("now", "hdg");*/
        /*int bhdg = get_boat("buf", "hdg");*/
        /*int rhdg = get_boat("req", "hdg");*/
        /*int rig = get_boat("now", "rig");*/
        /*int anc = get_boat("now", "anc");*/

        /*int wind = get_wind("dir");*/
        /*int pre = get_wind("pre");*/

        /*werase(DIAGNOSTIC_WIN);*/
        /*wprintw(DIAGNOSTIC_WIN, "NOW_HDG: %d\nBUF_HDG:%d\nREQ_HDG:%d\n"*/
                                /*"RIG: %d\nANC: %d\nWIN: %d\nPRE: %d", */
                                 /*hdg, bhdg, rhdg, rig, anc, wind, pre*/
                                 /*);*/
/*}*/

