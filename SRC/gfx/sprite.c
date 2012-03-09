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
const wchar_t gfxOCEAN[]  = L"◟◝◞◜";
const wchar_t gfxMTN[]    = L"▕▒▓";
const wchar_t gfxSHADE[3] = L"░▒▓";
const wchar_t gfxWARNPIP  = L'⋇';
const wchar_t gfxTREE[]   = L"╹⬢";
const wchar_t gfxSOLID    = L'█';
const wchar_t gfxBLANK    = L' ';

enum tree_enum { _TRUNK_, _CANOP_ };
enum dither_enum { _1Q_, _2Q_, _3Q_ };
enum ocean_elem { _DL_, _UR_, _DR_, _UL_ };

cchar_t   OCEAN[4], _OCEAN[4], __OCEAN[4];
cchar_t     MTN[3],   _MTN[3],   __MTN[3];
cchar_t    TREE[2],  _TREE[2],  __TREE[2];
cchar_t   GRASS[5], _GRASS[5], __GRASS[5];

cchar_t WARNBG, WARNSH, WARNPIP;
cchar_t BLANK, PLAIN;
cchar_t DIRT[2];
cchar_t UNDERSEA;
cchar_t DAYSKY;
cchar_t PURPLE[4];
/******************************************************************************/
void init_gfx_colors(void)
{
        // Ocean colors --------------------------------------------
        setcchar(  &OCEAN[0], &gfxOCEAN[_DL_], 0,   SEA_MED,  NULL);
        setcchar(  &OCEAN[1], &gfxOCEAN[_UR_], 0,   SEA_MED,  NULL);
        setcchar(  &OCEAN[2], &gfxOCEAN[_DL_], 0,   SEA_LIG,  NULL);
        setcchar(  &OCEAN[3], &gfxOCEAN[_UR_], 0,   SEA_LIG,  NULL);

        setcchar( &_OCEAN[0], &gfxOCEAN[_DL_], 0,  _SEA_MED,  NULL);
        setcchar( &_OCEAN[1], &gfxOCEAN[_UR_], 0,  _SEA_MED,  NULL);
        setcchar( &_OCEAN[2], &gfxOCEAN[_DL_], 0,  _SEA_LIG,  NULL);
        setcchar( &_OCEAN[3], &gfxOCEAN[_UR_], 0,  _SEA_LIG,  NULL);

        setcchar(&__OCEAN[0], &gfxOCEAN[_DL_], 0, __SEA_MED,  NULL);
        setcchar(&__OCEAN[1], &gfxOCEAN[_UR_], 0, __SEA_MED,  NULL);
        setcchar(&__OCEAN[2], &gfxOCEAN[_DL_], 0, __SEA_LIG,  NULL);
        setcchar(&__OCEAN[3], &gfxOCEAN[_UR_], 0, __SEA_LIG,  NULL);
        // ---------------------------------------------------------


        // Tree colors -----------------------------------------------
        setcchar(  &TREE[0], &gfxTREE[_TRUNK_], 0,   TREETRUNK, NULL);
        setcchar(  &TREE[1], &gfxTREE[_CANOP_], 0,   TREETOP,   NULL);
        setcchar( &_TREE[0], &gfxTREE[_TRUNK_], 0,  _TREETRUNK, NULL);
        setcchar( &_TREE[1], &gfxTREE[_CANOP_], 0,  _TREETOP,   NULL);
        setcchar(&__TREE[0], &gfxTREE[_TRUNK_], 0, __TREETRUNK, NULL);
        setcchar(&__TREE[1], &gfxTREE[_CANOP_], 0, __TREETOP,   NULL);
        // -----------------------------------------------------------


        // Grass colors ------------------------------------------
        setcchar(  &GRASS[0], &gfxSHADE[_1Q_], 0,   GRASSY, NULL);
        setcchar(  &GRASS[1], &gfxSHADE[_2Q_], 0,   GRASSY, NULL);
        setcchar(  &GRASS[2], &gfxSHADE[_3Q_], 0,   GRASSY, NULL);
        setcchar( &_GRASS[0], &gfxSHADE[_1Q_], 0,  _GRASSY, NULL);
        setcchar( &_GRASS[1], &gfxSHADE[_2Q_], 0,  _GRASSY, NULL);
        setcchar( &_GRASS[2], &gfxSHADE[_3Q_], 0,  _GRASSY, NULL);
        setcchar(&__GRASS[0], &gfxSHADE[_1Q_], 0, __GRASSY, NULL);
        setcchar(&__GRASS[1], &gfxSHADE[_2Q_], 0, __GRASSY, NULL);
        setcchar(&__GRASS[2], &gfxSHADE[_3Q_], 0, __GRASSY, NULL);
        // -------------------------------------------------------


        // Mountain colors --------------------------------        
        setcchar(  &MTN[0],   &gfxMTN[0], 0,   LAND, NULL);
        setcchar(  &MTN[1],   &gfxMTN[1], 0,   LAND, NULL);
        setcchar(  &MTN[2], &gfxSHADE[2], 0,   LAND, NULL);
        setcchar( &_MTN[0],   &gfxMTN[0], 0,  _LAND, NULL);
        setcchar( &_MTN[1],   &gfxMTN[1], 0,  _LAND, NULL);
        setcchar( &_MTN[2], &gfxSHADE[2], 0,  _LAND, NULL);
        setcchar(&__MTN[0],   &gfxMTN[0], 0, __LAND, NULL);
        setcchar(&__MTN[1],   &gfxMTN[1], 0, __LAND, NULL);
        setcchar(&__MTN[2], &gfxSHADE[2], 0, __LAND, NULL);
        // ------------------------------------------------


        // Warning dialog ---------------------------------
        setcchar( &WARNBG, &gfxBLANK, 0,    WARNING, NULL);
        setcchar( &WARNSH, &gfxBLANK, 0, WARNSHADOW, NULL);
        setcchar(&WARNPIP, &gfxBLANK, 0,    WARNING, NULL);
        // ------------------------------------------------


        setcchar(&DIRT[0], &gfxBLANK, 0, LAND, NULL);
        setcchar(&BLANK, &gfxBLANK, 0, 0, NULL);
        setcchar(&PLAIN, &gfxBLANK, 0, COLOR_PAIR(WHITE), NULL);
        setcchar(&UNDERSEA, &gfxSOLID, 0, UND_SEA, NULL);
        setcchar(&DAYSKY, &gfxSOLID, 0, DAY_SKY, NULL);
        setcchar(&PURPLE[0], &gfxBLANK, 0, PUR_WHI, NULL);
        setcchar(&PURPLE[1], &gfxBLANK, 0, PUR_DPU, NULL);
        setcchar(&PURPLE[2], &gfxBLANK, 0, PUR_PUR, NULL);
        setcchar(&PURPLE[3], &gfxBLANK, 0, PUR_DDP, NULL);
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
