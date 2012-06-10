/*                                 
 *  ▁▁▁▁▁▁                       
 * ⎛⎛⎛⎛⎞⎞⎞⎞  bldg.c:  huts and buildings 
 * ᛞ⋿ᛞᗶᗶᛞ⋿ᛞ  ----------------------------
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "../com/barnacle.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../lib/textutils.h"
#include "../gfx/ui/cursor.h"


// ᗯᗰᗼᗻᕰᕱᗵᗶᗷᗸᗹᗺ⦀⟱∰∭ᛤᛥᛞᛦ༊ᓬ⧎⋿ⰞⱎⲶⲷ∎⑉⬢⫹⫺⎱⎰⎠⎛⎝⎞⎠╱╲
//   ▁▂▃▄▅▆▇█▉▊▋▌▍▎▏░▒▓ ▐ ▔▕ ▖▗ ▘▙ ▚ ▛ ▜▝ ▞ ▟


struct cursor_t *bldg;
static const wchar_t *roof[]={L"   ", L"▁▁▁", L"▂▂▂", L"▃▃▃", L"▄▄▄", L"▅▅▅", L"▆▆▆", L"▔▔▔"};
static const wchar_t *wall[]={L"▁▁▁", L"▂▂▂", L"▃▃▃", L"▄▄▄", L"▅▅▅", L"▆▆▆", L"▇▇▇", L"█▅█"};

static const int wallpair = 254;
static const int roofpair = 253;
static const int doorpair = 252;

void draw_bldg(WINDOW *win, int y, int x, int r, int w)
{
        init_pair(roofpair, HOUSEROOF, CDGRASS_BG);
        init_pair(wallpair, HOUSEWALL, HOUSEROOF);
        init_pair(doorpair, HOUSEDOOR, HOUSEWALL);

        werase(win);

        mvcwpumpw(win, roofpair, y, x, L"%ls", roof[r]);
        mvcwpumpw(win, wallpair, y+1, x, L"%ls", wall[w]);

        if (w == 7) {
                mvwchgat(win, y, x, 3, A_REVERSE, roofpair, NULL);
                mvwchgat(win, y+1, x+1, 1, 0, doorpair, NULL);
        }
}

void draw_well(WINDOW *win, int y, int x)
{
        static const wchar_t *well[]={L"▛▜"};

        werase(win);
        mvcwpumpw(win, wallpair, y, x, L"%ls", well[0]);
}



int place_building(int dir)
{
        if (!bldg) {
                bldg = new_cursor(2, 3, 0, 0, LINES, COLS, 0, 0, HJKL);
                draw_bldg(bldg->win, 0, 0, 0, 0);
        }

        switch (dir) {
        case MODE_STARTED:
                bldg->Show(bldg);
                break;
        case 'm':
                bldg->Hide(bldg); 
                return MODE_RELEASE;
        case ' ':
                bldg->Stamp(bldg, MAPBOOK->active->L[BGR]->peek);
                break;
        case 'w':
                draw_well(bldg->win, 0, 0);
                break;
        case '0':
                draw_bldg(bldg->win, 0, 0, 0, 0);
                break;
        case '1':
                draw_bldg(bldg->win, 0, 0, 1, 1);
                break;
        case '2':
                draw_bldg(bldg->win, 0, 0, 2, 2);
                break;
        case '3':
                draw_bldg(bldg->win, 0, 0, 3, 3);
                break;
        case '4':
                draw_bldg(bldg->win, 0, 0, 4, 4);
                break;
        case '5':
                draw_bldg(bldg->win, 0, 0, 5, 5);
                break;
        case '6':
                draw_bldg(bldg->win, 0, 0, 6, 6);
                break;
        case '7':
                draw_bldg(bldg->win, 0, 0, 7, 7);
                break;
        default:
                bldg->Move(bldg, dir);
                break;
        }

        return MODE_PERSIST;
}


