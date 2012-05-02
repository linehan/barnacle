#pragma once
#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "map.h"

/*
 * Simplified accessors for comparing the value of multiple labels in
 * the same block. See the documentation for or_nibble in "../eng/bytes.h"
 */
#define SET_TILE(map,y,x,val) (set_byte(mx_get((map)->tile, y, x), LAB, val))
#define TILE(map,y,x)     (get_byte(mx_val((map)->tile, y, x), LAB))
#define IS_TILE(map,y,x,val)  (val == TILE(map,y,x)) ? true : false

#define SET_ELEV(map,y,x,val) (set_byte(mx_get((map)->tile, y, x), ALT, v))
#define ELEV(map,y,x) (get_byte(mx_val((map)->tile, y, x), ALT))
#define IS_ELEV(map,y,x,val)  (val == ELEV(map,y,x)) ? true : false


#define LAYER(val,n,...) or_byte(val, LAB, n, __VA_ARGS__)
//#define  ELEV(val,n,...) or_byte(val, ALT, n, __VA_ARGS__)

static const int   OCTAVES =  6;      /* Number of octaves to smooth */
static const float PERSIST =  0.99;   /* Persistence factor */
static const float SHOAL   = -0.003;  /* Shoal isoline */
static const float BEACH   =  0.00;   /* Beach isoline */
static const float TERRA   =  0.45;   /* Ground isoline */


#endif
