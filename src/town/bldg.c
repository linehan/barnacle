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


// ᗯᗰᗼᗻᕰᕱᗵᗶᗷᗸᗹᗺ⦀⟱∰∭ᛤᛥᛞᛦ༊ᓬ⧎⋿ⰞⱎⲶⲷ∎⑉⬢⫹⫺⎱⎰⎠⎛⎝⎞⎠
//   ▁▂▃▄▅▆▇█▉▊▋▌▍▎▏░▒▓ ▐ ▔▕ ▖▗ ▘▙ ▚ ▛ ▜▝ ▞ ▟


struct cursor_t *bldg;

void draw_bldg(WINDOW *win, int y, int x)
{
        static const wchar_t *roof=L"▅▅▅";
        static const wchar_t *wall=L"█▅█";
        static const int wallpair = 254;
        static const int roofpair = 253;

        init_pair(roofpair, HOUSEROOF, CDGRASS_BG);
        init_pair(wallpair, HOUSEWALL, HOUSEROOF);

        mvcwpumpw(win, roofpair, y, x, L"%ls", roof);
        mvcwpumpw(win, wallpair, y+1, x, L"%ls", wall);

        mvwchgat(win, y+1, x+1, 1, A_REVERSE, wallpair, NULL);
}



int place_building(int dir)
{
        if (!bldg) {
                bldg = new_cursor(2, 3, 0, 0, LINES, COLS, 0, 0, HJKL);
                draw_bldg(bldg->win, 0, 0);
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
        default:
                bldg->Move(bldg, dir);
                break;
        }

        return MODE_PERSIST;
}

        

/*void bldg_test(void)*/
/*{*/
        /*const wchar_t *roof=L"▅▅▅";*/
        /*const wchar_t *wall=L"█▅█";*/

        /*const int wallpair = 254;*/
        /*const int roofpair = 253;*/

        /*init_pair(roofpair, HOUSEROOF, bgcolor_yx(PLATE(ACTIVE, BGR), 30, 40));*/
        /*init_pair(wallpair, HOUSEWALL, HOUSEROOF);*/

        /*mvcwpumpw(PLATE(ACTIVE, BGR), roofpair, 31, 40, L"%ls", roof);*/
        /*mvcwpumpw(PLATE(ACTIVE, BGR), wallpair, 32, 40, L"%ls", wall);*/

        /*mvwchgat(PLATE(ACTIVE, BGR), 32, 41, 1, A_REVERSE, wallpair, NULL);*/
/*}*/









