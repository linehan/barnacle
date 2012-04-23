#pragma once
#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "map.h"


static const int   OCTAVES =  6;      /* Number of octaves to smooth */
static const float PERSIST =  0.99;   /* Persistence factor */
static const float SHOAL   = -0.003;  /* Shoal isoline */
static const float BEACH   =  0.00;   /* Beach isoline */
static const float TERRA   =  0.45;   /* Ground isoline */


#endif
