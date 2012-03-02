// vim:fdm=marker
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../geo/terrain.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../pan/test.h"

/*struct recipe {*/
        /*int n;*/
        /*int ofs[4];*/
        /*short pair[64];*/
        /*const wchar_t **wch;*/
        /*const char **bar;*/
        /*int *len;*/
        /*cchar_t **cch;*/
/*};*/


#define GS GRE_SKY
#define WS WHI_SKY


#define CLOUDH 6 

static struct recipe _cloud = { 6, 10, 10, {WS, WS},

{L"     ㍇㌫㌨   ",
 L"     ㌨㌡㌽㌷ ",
 L"  ㍇㌡㍋㌫㍊㍖",
 L"㍊㍖㌫㌽㍊㍖㌷",
 L"㍐㌠㍖㍀㌫㍋  ",
 L" ㌞           "},
{ "00000000000000",
  "00000000000000",
  "00000000000000",
  "00000000000000",
  "00000000000000",
  "00000000000000"}};



int ready;
PANEL *CLOUDS;

void init_cloud(void)
{
        illuminate(&_cloud); 
        _cloud.paint(&_cloud);
        CLOUDS = new_panel(_cloud.win);
}
