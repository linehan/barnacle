#pragma once
#ifndef __BRNOISE_H
#define __BRNOISE_H

#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include <wchar.h>
#include <ncurses.h>
#include "../gen/dice.h"

static const int DOTN[]={ 0, 7, 26, 54, 68, 53, 36};

#define DOT0 DOTN[0]
#define DOT1 DOTN[1]
#define DOT2 DOTN[2]
#define DOT3 DOTN[3]
#define DOT4 DOTN[4]
#define DOT5 DOTN[5]
#define DOT6 DOTN[6]

cchar_t brnoise_cch;

static wchar_t *BRDOT[7]={
L"",
L"⠁⠂⠄⠈⠐⠠⡀⢀",
L"⠃⠅⠆⠉⠑⠒⠔⠘⠡⠢⠤⠨⠰⡁⡂⡄⡈⡐⡠⢁⢂⢃⢄⢈⢐⢠⣀", 
L"⠇⠋⠍⠎⠓⠕⠖⠙⠚⠜⠣⠥⠦⠩⠪⠬⠱⠲⠴⠸⡃⡅⡆⡉⡊⡌⡑⡒⡔⡘⡡⡢⡤⡨⡰⢃⢅⢆⢉⢊⢌⢑⢒⢔⢘⢡⢢⢤⢨⢰⣁⣂⣄⣈⣐⣠", 
L"⠏⠗⠛⠝⠞⠫⠭⠮⠳⠴⠶⠹⠺⠼⡇⡋⡍⡎⡓⡕⡖⡙⡚⡜⡣⡥⡦⡩⡪⡬⡱⡲⡴⡸⢇⢋⢍⢎⢓⢕⢖⢙⢚⢜⢣⢥⢦⢩⢪⢬⢱⢲⢴⢸⣃⣅⣆⣉⣊⣋⣌⣍⣑⣒⣔⣘⣡⣢⣤⣨⣰",
L"⠟⠯⠷⠻⠽⠾⡏⡗⡛⡝⡞⡧⡫⡭⡮⡳⡵⡶⡹⡺⡼⢏⢗⢛⢝⢞⢧⢫⢭⢮⢳⢵⢶⢹⢺⢼⣇⣋⣍⣎⣓⣕⣖⣙⣚⣜⣥⣦⣩⣪⣬⣱⣲⣴⣸",
L"⠿⡟⡯⡷⡻⡽⡾⡿⢟⢯⢷⢻⢽⢾⢿⣏⣗⣛⣝⣞⣟⣧⣫⣭⣮⣯⣳⣵⣶⣷⣹⣺⣻⣼⣽⣾⣿"};


#define BRCCH(n, co) mkcch(&BRDOT[n][roll_fair(DOTN[n])], 0, co)


#endif
