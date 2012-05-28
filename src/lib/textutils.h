#pragma once
#ifndef __TEXTUTILS_H
#define __TEXTUTILS_H

#include <stdarg.h>
#include "../com/barnacle.h"

char    *cdup(const char *str);
wchar_t *wdup(const wchar_t *wcs);
wchar_t *mbdup(const char *str);

void wclean(wchar_t *wcs, size_t len);
void cclean(   char *str, size_t len);

void pumpf(char **strp, const char *fmt, ...);
void wpumpf(wchar_t **wcsp, const wchar_t *wfmt, ...);
void wpumpw(WINDOW *win, const wchar_t *wfmt, ...);
void mvwpumpw(WINDOW *win, int y, int x, const wchar_t *wfmt, ...);
void mvcwpumpw(WINDOW *win, short pair, int y, int x, const wchar_t *wfmt, ...);
size_t swpumpf(wchar_t *buf, size_t maxlen, const wchar_t *wfmt, ...);


#endif
