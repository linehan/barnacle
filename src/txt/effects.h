#pragma once
#ifndef __GLOSS_EFFECTS_H
#define __GLOSS_EFFECTS_H
#include "../com/arawak.h"


void wcs_reveal(WINDOW *win, const wchar_t *wcs, size_t len, int L);
void wcs_unreveal(WINDOW *win, const wchar_t *wcs, size_t len, int L);
void wcs_shine(WINDOW *win, const wchar_t *wcs, size_t len, int L);
void wcs_pushr(WINDOW *win, const wchar_t *wcs, size_t len, int L);
void wcs_pushl(WINDOW *win, const wchar_t *wcs, size_t len, int L);
void wcs_dropout(WINDOW *win, const wchar_t *wcs, size_t len, int L);




#endif
