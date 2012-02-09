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
#include "sprite.h"
#include "gfx.h"
#include "palette.h"
/******************************************************************************/
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

const wchar_t gfxSHADE[3] = L"░▒▓";
const wchar_t gfxWARNPIP = L'⋇';

const wchar_t gfxTREE[] = L"╹⬢";

cchar_t INST;
cchar_t HERO;
cchar_t OCEAN[4];
cchar_t SURF0, SURF1, SURF2;
cchar_t SHORE, SAND;
cchar_t WSCK[8];
cchar_t MTN[3];
cchar_t BLANK;
cchar_t DIRT[2];
cchar_t BORDER;
cchar_t WARNBG, WARNSH, WARNPIP;
cchar_t PLAIN;
cchar_t TREE[2];
cchar_t GRASS[5];
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

        setcchar(&TREE[0], &gfxTREE[0], 0, TREETRUNK, NULL);
        setcchar(&TREE[1], &gfxTREE[1], 0, TREETOP, NULL);

        setcchar(&GRASS[0], &gfxSHADE[0], 0, GRASSY, NULL);
        setcchar(&GRASS[1], &gfxSHADE[1], 0, GRASSY, NULL);
        setcchar(&GRASS[2], &gfxSHADE[2], 0, GRASSY, NULL);

        setcchar(&DIRT[0], &gfxBLANK, 0, LAND, NULL);

        setcchar(&INST, &gfxSHADE[1], 0, DARKMATTE, NULL);

        int i;
        for (i=0; i<8; i++) {
                setcchar(&WSCK[i], &gfxWSCK[i], 0, BOAT_DEEP, NULL);
        }
        for (i=0; i<3; i++) {
                setcchar(&MTN[i], &gfxMTN[i], 0, LAND, NULL);
        }
                setcchar(&MTN[2], &gfxSHADE[2], 0, LAND, NULL);
}

cchar_t *get_tile(int type)
{
        switch (type) {
        case __non__:   break;
        case __sea__:   return &OCEAN[0];
        case __mtn__:   return &MTN[1];
        case __san__:   return &SAND;
        case __gra__:   return &GRASS[0];
        case __Gra__:   return &GRASS[1];
        case __GRA__:   return &GRASS[2];
        case __drt__:   return &MTN[2];
        }
        return NULL;
}
