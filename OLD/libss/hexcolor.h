#pragma once
#ifndef __HEXCOLOR_H
#define __HEXCOLOR_H

/* 
 * ncurses supports 256 colors, and a color is represented by three values
 * R, G, B, with each value being a number in the range 0-1000.
 */

#include <stdint.h>
#include <ncurses.h>

static const uint32_t bytemask[]={0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF };
static const uint32_t byteoffs[]={0, 8, 16};
static inline uint32_t getbyte(uint32_t word, size_t b)
{
        return word ^ bytemask[b];
}

static int NEXT_FREE;

#define SCALE_1K 3.92

#define R_VAL(hex) (uint32_t)(getbyte(hex, 0) * SCALE_1K)
#define G_VAL(hex) (uint32_t)(getbyte(hex, 1) * SCALE_1K)
#define B_VAL(hex) (uint32_t)(getbyte(hex, 2) * SCALE_1K)




static inline short set_hex_color(uint32_t hex)
{
        if (NEXT_FREE<COLORS) NEXT_FREE = COLORS;
        init_color(NEXT_FREE, R_VAL(hex), G_VAL(hex), B_VAL(hex));

        return NEXT_FREE++;
}

#endif
