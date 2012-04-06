#pragma once
#ifndef __HEXCOLOR_H
#define __HEXCOLOR_H

/* 
 * Macros and inline functions for converting hexadecimal values to
 * component RGB values.
 */

#include <stdint.h>

#define RGB        3
#define MAXBYTE  256  // Maximum value of an 8-bit byte
#define MAXRGB  1000  // Maximum value of a component in an RGB tuple 

enum {R_BYTE, G_BYTE, B_BYTE};

static const uint32_t bytemask[RGB]={0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF };



/* Expand to the desired byte, corresponding to an RGB component */
#define GETBYTE(word, byte) word ^ bytemask[byte]

/* Expand to the rounded RGB component value of an RGB component byte */
#define RGB1000(byte) (uint32_t)(((byte) / MAXBYTE) * MAXRGB)


/* 
 * Given a hexadecimal triplet, return the RGB component value
 * corresponding to the capitalized letter.
 */
#define Rgb(triplet) RGB1000(GETBYTE(triplet, R_BYTE))
#define rGb(triplet) RGB1000(GETBYTE(triplet, G_BYTE))
#define rgB(triplet) RGB1000(GETBYTE(triplet, B_BYTE))


#endif
