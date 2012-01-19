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

#include "lib/list/list.h"
#include "lib/gfx.h"
#include "palette.h"
/******************************************************************************/
const wchar_t HERO_GFX = L'ж';
const wchar_t gfxOCEAN[] = L"◟◝◞◜";
/*const wchar_t WAVE_FLOW_GFX = L'◞';*/
const wchar_t WAVE_BREAK_GFX = L'⟱';
const wchar_t WAVE_FLOW_GFX = L'∭';
const wchar_t SHORE_GFX = L'∵';
const wchar_t SAND_GFX = L'⚋';
const wchar_t gfxWSCK[] = L"⬒⬔◨◪⬓⬕◧◩";
const wchar_t gfxFOREST[] = L"▁▓▒⤋⟱";
const wchar_t gfxMTN[] = L"▕▒▓";

cchar_t HERO;
cchar_t OCEAN[4];
cchar_t SURF0, SURF1, SURF2;
cchar_t SHORE, SAND;
cchar_t WSCK[8];
cchar_t FOREST[5];
cchar_t MTN[3];
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
