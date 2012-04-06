#pragma once
#ifndef __BRNOISE_H
#define __BRNOISE_H

#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include <wchar.h>
#include <ncurses.h>
#include "../stoc/stoc.h"

/*
 * Unicode uses the standard dot-numbering 1 to 8. Historically only the 
 * 6-dot cell was used in Braille. The lower two dots were added later, 
 * which explains the irregular numbering 1-2-3-7 in the left column and 
 * 4-5-6-8 in the right column. Where dots 7 and 8 are not raised, there 
 * is no distinction between 6-dot and 8-dot definitions.
 *
 * The Unicode name of a specific pattern mentions the raised dots: 
 * U+2813 ⠓ braille pattern dots-125 has dots 1, 2 and 5 raised. By 
 * exception, the zero dot raised pattern is named U+2800 ⠀ braille 
 * pattern blank.[2]
 *
 * In the 8-dot cell each dot individually can be raised or not. That 
 * creates 2^8=256 different patterns. By mapping each of the eight dots 
 * to a bit in a byte (in a low endian order), and by defining "0"/"1" 
 * for not raised/raised per bit, every specific pattern generates a 
 * identifying binary number. So the pattern with dots 1-2-5 raised would 
 * yield (00010011)2, equivalent to (13)16 or (19)10.
 *
 * The mapping can also be computed by adding together the hexadecimal 
 * values, of the dots raised. So the pattern with dots 1-2-5 raised 
 * would yield 0x1+0x2+0x10 = 0x13. Whether computed directly in 
 * hexadecimal, or indirectly via binary, the result is added to 0x2800
 * the offset for the Braille Patterns Unicode block.
 */


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


//static wchar_t BRGRADL[]={L"⠁⠂⠄⡀⡁⠅⠃⠇⡃⡅⡆⡇⡎⢇"};
//static wchar_t BRGRADR[]={L"⠈⠐⠠⢀⢈⠨⠘⠸⢘⢨⢰⢸⢱⡸"};
//static wchar_t BRGRADU[]={L"⠁⠈⠂⠐⠄⠠⡀⢀⠉⠑⠒⠤⡠⣀"};


#define BRCCH(n, co) mkcch(&BRDOT[n][roll_fair(DOTN[n])], 0, co)


#endif

