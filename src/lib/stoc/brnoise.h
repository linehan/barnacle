#pragma once
#ifndef __BRNOISE_H
#define __BRNOISE_H

#include <wchar.h>

wchar_t *br_random(int n);

wchar_t br_hex(unsigned char hex);
wchar_t *br_dis(int ndots);
wchar_t *br_atl(int n);
wchar_t *br_atm(int n);
wchar_t *br_any(void);

#endif

