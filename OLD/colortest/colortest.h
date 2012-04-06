#pragma once
#ifndef __COLORTEST_H
#define __COLORTEST_H
#include <stdint.h>
#include "stdgfx.h"
#include "hexcolor.h"

#define MAXRAMP 11 

enum {_R_, _G_, _B_};

enum ramptags { SPECTRUM6, SPECTRUM11 };
enum rampleng { SPECT6LEN=6, SPECT11LEN=11 };

/*
 * This product includes color specifications and designs developed by 
 * Cynthia Brewer (http://colorbrewer.org/).
 *
 * "Spectral" diverging color palette
 * 
 *      Cynthia Brewer (Pennsylvania State University)
 *      Mark Harrower (University of Wisconsin-Madison)
 *
 */
static const uint32_t spect6[SPECT6LEN][RGB]={
        {835,   243,  310},
        {988,   553,  349},
        {996,   878,  545},
        {902,   961,  596},
        {600,   835,  580},
        {196,   533,  741} 
};

static const uint32_t spect11[SPECT11LEN][RGB]={
        { 620,    4,  259},
        { 835,  243,  310},
        { 957,  427,  263},
        { 992,  682,  380},
        { 996,  878,  545},
        {1000, 1000,  749},
        { 902,  961,  596},
        { 671,  867,  643},
        { 400,  761,  647},
        { 196,  533,  741},
        { 369,  310,  635} 
};


static int ramplen[2]={ SPECT6LEN, SPECT11LEN };


/* Expands to triplet value, suitable for passing to init_color() */
#define TRIPLET(id, co) \
        id[co][_R_], id[co][_G_], id[co][_B_]


void init_colortest(int id, short startnumber);
void paint_ramp(int id);
PANEL *colortest_panel(void);


#endif
