// vim:fdm=marker
/*******************************************************************************
  FILENAME: sprite.c
  A dumping ground for wchar_t's and their cchar_t inits.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>

#include "../lib/llist/list.h"
#include "gfx.h"
#include "palette.h"
/******************************************************************************/
enum tiletype { __blank__ = 0,
                __ocean__ = 1,
                __sand__  = 2,
                __mtn__   = 3
              };

const wchar_t HERO_GFX = L'ж';
const wchar_t gfxOCEAN[] = L"◟◝◞◜";
/*const wchar_t WAVE_FLOW_GFX = L'◞';*/
const wchar_t WAVE_BREAK_GFX = L'⟱';
const wchar_t WAVE_FLOW_GFX = L'∭';
const wchar_t SHORE_GFX = L'∵';
const wchar_t SAND_GFX = L'⚋';
const wchar_t gfxBLANK = L' ';
const wchar_t gfxWSCK[] = L"⬒⬔◨◪⬓⬕◧◩";
const wchar_t gfxFOREST[] = L"▁▓▒⤋⟱";
const wchar_t gfxMTN[] = L"▕▒▓";
const wchar_t gfxBORDER = L'▔';

const wchar_t gfxSHADOW = L'░';
const wchar_t gfxWARNPIP = L'⋇';

cchar_t HERO;
cchar_t OCEAN[4];
cchar_t SURF0, SURF1, SURF2;
cchar_t SHORE, SAND;
cchar_t WSCK[8];
cchar_t FOREST[5];
cchar_t MTN[3];
cchar_t BLANK;
cchar_t BORDER;
cchar_t WARNBG, WARNSH, WARNPIP;
cchar_t PLAIN;
/******************************************************************************/
void init_gfx_colors(void)
{
        setcchar(&OCEAN[0], &gfxOCEAN[0], 0, SEA_MED,  NULL);
        setcchar(&OCEAN[1], &gfxOCEAN[1], 0, SEA_MED,  NULL);
        setcchar(&OCEAN[2], &gfxOCEAN[0], 0, SEA_LIGHT,  NULL);
        setcchar(&OCEAN[3], &gfxOCEAN[1], 0, SEA_LIGHT,  NULL);

        setcchar(&SURF0, &WAVE_BREAK_GFX, 0, SEA_LIGHT,  NULL);
        setcchar(&SURF1, &WAVE_FLOW_GFX,  0, SEA_LIGHT,  NULL);
        setcchar(&SAND,  &SAND_GFX,       0, SEA_SHORE, NULL);
        setcchar(&SHORE, &SHORE_GFX,      0, SEA_SHORE, NULL);

        setcchar(&BLANK, &gfxBLANK, 0, MENU, NULL);
        setcchar(&PLAIN, &gfxBLANK, 0, COLOR_PAIR(WHITE), NULL);

        setcchar(&BORDER, &gfxBORDER, 0, MENU, NULL);

        setcchar(&WARNBG, &gfxBLANK, 0, WARNING, NULL);
        setcchar(&WARNSH, &gfxBLANK, 0, WARNSHADOW, NULL);
        setcchar(&WARNPIP, &gfxBLANK, 0, WARNING, NULL);

        int i;
        for (i=0; i<8; i++) {
                setcchar(&WSCK[i], &gfxWSCK[i], 0, BOAT_DEEP, NULL);
        }
        for (i=0; i<5; i++) {
                setcchar(&FOREST[i], &gfxFOREST[i], 0, TREES, NULL);
        }
        for (i=0; i<3; i++) {
                setcchar(&MTN[i], &gfxMTN[i], 0, LAND, NULL);
        }
}


cchar_t *get_tile(int type)
{
        switch (type) {
        case __ocean__:
                return &OCEAN[0];
                break;
        case __mtn__:
                return &MTN[1];
                break;
        case __sand__:
                return &SAND;
                break;
        }
        return NULL;
}

cchar_t *bg_tile(int type)
{
        switch (type) {
        case __blank__:
                break;
        case __ocean__:
                return &OCEAN[0];
                break;
        }
        return NULL;
}
