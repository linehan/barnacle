#pragma once
#ifndef __COLLOC_H
#define __COLLOC_H
#include "../com/arawak.h"

#define ERR_NOCOLORS -1
#define ERR_NOPAIRS  -2

void init_colloc(void);
int colloc(int r, int g, int b);
int palloc(int fg, int bg);
bool free_color(int id);
bool free_pair(int id);

#endif
