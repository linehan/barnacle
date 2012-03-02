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
const wchar_t gfxSOLID = L'█';

cchar_t HERO;
cchar_t OCEAN[4];
cchar_t _OCEAN[4];
cchar_t __OCEAN[4];
cchar_t SURF0, SURF1, SURF2;
cchar_t _SURF0, _SURF1, _SURF2;
cchar_t __SURF0, __SURF1, __SURF2;
cchar_t WSCK[8];
cchar_t MTN[3];
cchar_t _MTN[3];
cchar_t __MTN[3];
cchar_t BLANK;
cchar_t DIRT[2];
cchar_t BORDER;
cchar_t WARNBG, WARNSH, WARNPIP;
cchar_t PLAIN;
cchar_t TREE[2];
cchar_t _TREE[2];
cchar_t __TREE[2];
cchar_t GRASS[5];
cchar_t _GRASS[5];
cchar_t __GRASS[5];
cchar_t UNDERSEA;
cchar_t DAYSKY;
/******************************************************************************/
void init_gfx_colors(void)
{
        setcchar(&OCEAN[0], &gfxOCEAN[0], 0, SEA_MED,  NULL);
        setcchar(&OCEAN[1], &gfxOCEAN[1], 0, SEA_MED,  NULL);
        setcchar(&OCEAN[2], &gfxOCEAN[0], 0, SEA_LIG,  NULL);
        setcchar(&OCEAN[3], &gfxOCEAN[1], 0, SEA_LIG,  NULL);

        setcchar(&_OCEAN[0], &gfxOCEAN[0], 0, _SEA_MED,  NULL);
        setcchar(&_OCEAN[1], &gfxOCEAN[1], 0, _SEA_MED,  NULL);
        setcchar(&_OCEAN[2], &gfxOCEAN[0], 0, _SEA_LIG,  NULL);
        setcchar(&_OCEAN[3], &gfxOCEAN[1], 0, _SEA_LIG,  NULL);

        setcchar(&__OCEAN[0], &gfxOCEAN[0], 0, __SEA_MED,  NULL);
        setcchar(&__OCEAN[1], &gfxOCEAN[1], 0, __SEA_MED,  NULL);
        setcchar(&__OCEAN[2], &gfxOCEAN[0], 0, __SEA_LIG,  NULL);
        setcchar(&__OCEAN[3], &gfxOCEAN[1], 0, __SEA_LIG,  NULL);

        setcchar(&SURF0, &WAVE_BREAK_GFX, 0, SEA_LIG,  NULL);
        setcchar(&SURF1, &WAVE_FLOW_GFX,  0, SEA_LIG,  NULL);
        setcchar(&_SURF0, &WAVE_BREAK_GFX, 0, _SEA_LIG,  NULL);
        setcchar(&_SURF1, &WAVE_FLOW_GFX,  0, _SEA_LIG,  NULL);
        setcchar(&__SURF0, &WAVE_BREAK_GFX, 0, __SEA_LIG,  NULL);
        setcchar(&__SURF1, &WAVE_FLOW_GFX,  0, __SEA_LIG,  NULL);

        setcchar(&BLANK, &gfxBLANK, 0, 0, NULL);
        setcchar(&PLAIN, &gfxBLANK, 0, COLOR_PAIR(WHITE), NULL);

        setcchar(&BORDER, &gfxBORDER, 0, 0, NULL);

        setcchar(&WARNBG, &gfxBLANK, 0, WARNING, NULL);
        setcchar(&WARNSH, &gfxBLANK, 0, WARNSHADOW, NULL);
        setcchar(&WARNPIP, &gfxBLANK, 0, WARNING, NULL);

        setcchar(&TREE[0], &gfxTREE[0], 0, TREETRUNK, NULL);
        setcchar(&TREE[1], &gfxTREE[1], 0, TREETOP, NULL);
        setcchar(&_TREE[0], &gfxTREE[0], 0, _TREETRUNK, NULL);
        setcchar(&_TREE[1], &gfxTREE[1], 0, _TREETOP, NULL);
        setcchar(&__TREE[0], &gfxTREE[0], 0, __TREETRUNK, NULL);
        setcchar(&__TREE[1], &gfxTREE[1], 0, __TREETOP, NULL);

        setcchar(&GRASS[0], &gfxSHADE[0], 0, GRASSY, NULL);
        setcchar(&GRASS[1], &gfxSHADE[1], 0, GRASSY, NULL);
        setcchar(&GRASS[2], &gfxSHADE[2], 0, GRASSY, NULL);
        setcchar(&_GRASS[0], &gfxSHADE[0], 0, _GRASSY, NULL);
        setcchar(&_GRASS[1], &gfxSHADE[1], 0, _GRASSY, NULL);
        setcchar(&_GRASS[2], &gfxSHADE[2], 0, _GRASSY, NULL);
        setcchar(&__GRASS[0], &gfxSHADE[0], 0, __GRASSY, NULL);
        setcchar(&__GRASS[1], &gfxSHADE[1], 0, __GRASSY, NULL);
        setcchar(&__GRASS[2], &gfxSHADE[2], 0, __GRASSY, NULL);

        setcchar(&UNDERSEA, &gfxSOLID, 0, UND_SEA, NULL);
        setcchar(&DAYSKY, &gfxSOLID, 0, DAY_SKY, NULL);

        setcchar(&DIRT[0], &gfxBLANK, 0, LAND, NULL);

        int i;
        for (i=0; i<8; i++) {
                setcchar(&WSCK[i], &gfxWSCK[i], 0, BOAT_DEEP, NULL);
        }
        for (i=0; i<3; i++) {
                setcchar(&MTN[i], &gfxMTN[i], 0, LAND, NULL);
                setcchar(&_MTN[i], &gfxMTN[i], 0, _LAND, NULL);
                setcchar(&__MTN[i], &gfxMTN[i], 0, __LAND, NULL);
        }
                setcchar(&MTN[2], &gfxSHADE[2], 0, LAND, NULL);
                setcchar(&_MTN[2], &gfxSHADE[2], 0, _LAND, NULL);
                setcchar(&__MTN[2], &gfxSHADE[2], 0, __LAND, NULL);
}

cchar_t *swap_shade(cchar_t *cch, int mylvl, int tolvl)
{
        if (mylvl == 0) {
                while (tolvl-->0) cch++;
                return cch;
        }
        else if (mylvl == 1)
                return (tolvl == 0) ? --cch : ++cch;
        else if (mylvl == 2) {
                while (tolvl++<2) cch--;
                return cch;
        }
        else return cch;
}


cchar_t *get_tile(int type)
{
        switch (type) {
        case __non__:   break;
        case __sea__:   return &OCEAN[0];
        case __mtn__:   return &MTN[1];
        /*case __san__:   return &SAND;*/
        case __gra__:   return &GRASS[0];
        case __Gra__:   return &GRASS[1];
        case __GRA__:   return &GRASS[2];
        case __dirt__:   return &MTN[2];
        }
        return NULL;
}
