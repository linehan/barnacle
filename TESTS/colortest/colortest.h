#pragma once
#ifndef __COLORTEST_H
#define __COLORTEST_H

#include <stdint.h>
#include "stdgfx.h"
#include "hexcolor.h"

#define MAXRAMPLEN 32

enum {_R_, _G_, _B_};

#define NRAMPS 47

struct triplet_t { uint32_t r; uint32_t g; uint32_t b; };

struct palette_t {
        char name[100];
        int len;
        struct triplet_t *rgb;
};

enum ramptags { 
        SPECTRAL3,
        SPECTRAL4,
        SPECTRAL5,
        SPECTRAL6,
        SPECTRAL7,
        SPECTRAL8,
        SPECTRAL9,
        SPECTRAL10,
        SPECTRAL11,
        BRBG3,
        BRBG4,
        BRBG5,
        BRBG6,
        BRBG7,
        BRBG8,
        BRBG9,
        BRBG10,
        BRBG11,
        WIKI20,
        NORDISK,
        GMT_HAXBY,
        GREY3,
        GREY4,
        GREY5,
        GREY6,
        GREY7,
        GREY8,
        GREY9,
        HUMIDITY,
        RAINFALL,
        MEYERS,
        PRECIP11,
        PRECIP12,
        RYB3,
        RYB4,
        RYB5,
        RYB6,
        RYB7,
        RYB8,
        RYB9,
        RYB10,
        RYB11,
        SEMIHAX,
        TEMP18,
        TEMP19,
        WIKIICE,
        VOXPOP
};


#define TRIPLET(id, co) \
        ramp[0]->rgb[co].r, ramp[0]->rgb[co].g, ramp[0]->rgb[co].b

struct palette_t *ramp[3];

void paint_ramp(int id);
PANEL *colortest_panel(void);
struct palette_t *ggrparse(const char *path);
void ggrtest(const char *path);


#endif
