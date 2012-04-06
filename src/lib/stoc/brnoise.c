#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include "stoc.h"

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof(a[0])))
#define BRAILLE_BLOCK 0x2800
#define DOTSETS 9 


static int blocksize[DOTSETS]={ 1, 8, 28, 58, 72, 58, 28, 8, 1 };


wchar_t *brndot[DOTSETS]={
L"⠀"
L"⠀",
L"⠁⠂⠄⡀⠈⠐⠠⢀"
L"⠁⠂⠄⡀⠈⠐⠠⢀",
L"⠃⠅⠆⠉⠑⠒⠔⠘⠡⠡⠢⠤⠨⠰⡁⡂⡄⡈⡐⡠⢁⢂⢂⢄⢈⢐⢠⣀", 
L"⠃⠅⠆⠉⠑⠒⠔⠘⠡⠡⠢⠤⠨⠰⡁⡂⡄⡈⡐⡠⢁⢂⢂⢄⢈⢐⢠⣀"
L"⠇⠋⠍⠎⠓⠕⠖⠙⠚⠜⠣⠥⠦⠩⠪⠬⠱⠲⠴⠸⡃⡅⢃⡆⡉⡊⡌⡑⡒⡔⡘⡡⡢⡤⡨⡰⢃⢅⢆⢉⢊⢌⢑⢒⢔⢘⢡⢢⢤⢨⢰⣁⣂⣄⣈⣐⣠"
L"⠇⠋⠍⠎⠓⠕⠖⠙⠚⠜⠣⠥⠦⠩⠪⠬⠱⠲⠴⠸⡃⡅⢃⡆⡉⡊⡌⡑⡒⡔⡘⡡⡢⡤⡨⡰⢃⢅⢆⢉⢊⢌⢑⢒⢔⢘⢡⢢⢤⢨⢰⣁⣂⣄⣈⣐⣠", 
L"⠏⠗⠛⠝⠞⠫⠭⠮⠳⠴⠶⠹⠺⠼⡇⡋⡍⡎⡓⡕⡖⡙⡚⡜⡣⡥⡦⡩⡪⡬⡱⡲⡴⡸⢇⢋⢍⢎⢓⢕⢕⢖⢙⢚⢜⢣⢥⢦⢩⢪⢬⢱⢲⢴⢸⣃⣅⣆⣉⣊⣋⣌⣍⣑⣒⣔⣘⣡⣢⣤⣨⣰"
L"⠏⠗⠛⠝⠞⠫⠭⠮⠳⠴⠶⠹⠺⠼⡇⡋⡍⡎⡓⡕⡖⡙⡚⡜⡣⡥⡦⡩⡪⡬⡱⡲⡴⡸⢇⢋⢍⢎⢓⢕⢕⢖⢙⢚⢜⢣⢥⢦⢩⢪⢬⢱⢲⢴⢸⣃⣅⣆⣉⣊⣋⣌⣍⣑⣒⣔⣘⣡⣢⣤⣨⣰",
L"⠟⠯⠷⠻⠽⠾⡏⡗⡛⡝⡞⡧⡫⡭⡮⡳⡵⡶⡹⡺⡼⢏⢗⢗⢛⢝⢞⢧⢫⢭⢮⢳⢵⢶⢹⢺⢼⣇⣋⣍⣎⣓⣕⣖⣙⣚⣜⣥⣦⣩⣪⣬⣬⣱⣲⣴⣸"
L"⠟⠯⠷⠻⠽⠾⡏⡗⡛⡝⡞⡧⡫⡭⡮⡳⡵⡶⡹⡺⡼⢏⢗⢗⢛⢝⢞⢧⢫⢭⢮⢳⢵⢶⢹⢺⢼⣇⣋⣍⣎⣓⣕⣖⣙⣚⣜⣥⣦⣩⣪⣬⣬⣱⣲⣴⣸",
L"⠿⡟⡯⡷⡻⡽⡾⢟⢯⢷⢻⢽⢾⣏⣗⣛⣝⣞⣧⣫⣭⣮⣳⣵⣶⣹⣺⣼"
L"⠿⡟⡯⡷⡻⡽⡾⢟⢯⢷⢻⢽⢾⣏⣗⣛⣝⣞⣧⣫⣭⣮⣳⣵⣶⣹⣺⣼",
L"⢿⣻⣽⣾⡿⣟⣯⣷"
L"⢿⣻⣽⣾⡿⣟⣯⣷",
L"⣿"
L"⣿" 
};


/*
 * Accepts a hexadecimal value representing the pattern of the braille
 * character being requested.
 */
wchar_t br_hex(unsigned char hex)
{
        return (hex <= 0xFF) ? BRAILLE_BLOCK + hex : BRAILLE_BLOCK;
}


/*
 * Accepts an integer number (<=8) and returns a braille character with
 * that many dots set
 */
wchar_t *br_dis(int n)
{
        n %= DOTSETS;

        return (&brndot[n][roll1d(blocksize[n])]);
}



/*
 * Accepts an integer number (0<=n<=8) and returns a braille character with
 * AT MOST that many dots set 
 */
wchar_t *br_atm(int n)
{
        n %= DOTSETS;
        return br_dis(roll1d(n));
}


/*
 * Accepts an integer number (0<=n<=8) and returns a braille character with
 * AT LEAST that many dots set 
 */
wchar_t *br_atl(int n)
{
        n %= DOTSETS;
        return br_dis((DOTSETS - roll1d(n)) + n);
}


/*
 * Returns a random braille character, any one.
 */
wchar_t *br_any(void)
{
        return br_dis(roll1d(8));
}

