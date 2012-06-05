#include "../com/barnacle.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../lib/textutils.h"
#include "../gfx/ui/cursor.h"




/*
 ▁▁▁▁▁▁
⎛⎛⎛⎛⎞⎞⎞⎞
ᛞ⋿ᛞᗶᗶᛞ⋿ᛞ

ᗯᗰ
⦀
▁ ▂
⟱⟱⟱⟱⟱⟱⟱⟱⟱
⬢⬢⬢⬢⬢⬢⬢⬢⬢
▕༊༊༊༊༊༊༊༊༊▏
∰∰∰∰∰∰∰∰∰
ὡὢὣὤὤὡὥὦὧ
ᛦᛦᛦᛦᛦᛦᛦᛦᛦ
⎠⎛⎝⎞⎠
ἘἙἚἛἜἝ
ᾐᾑᾒᾓᾔᾕᾖᾗ
ὠὡὢὣὤὥὦὧ
ᾚᾛᾜᾝᾞᾟ∎⧎
⋳⋴⋵⋶⋷⋸⋹⋺⋻⋼⋽⋾⋿
∭ᛤᓬⰞⱎⰞⱎⰞⱎⰞⱎⰞⱎⰞᗼᗻᗻᗼ
ᕰᕱᗵᗶᗷᗸᗹᗺᗻᗼᗽ
ⲷⲷⲷⲷⲷⲷⲷⲷⲷⲷⲷⲷⲷⲷ
ⲶⲶⲶⲶⲶⲶⲶⲶⲶⲶⲶⲶⲶⲶ
▏▕⑉⑉
Ⲿ⑉Ⲿ༊
ⲾⲿⲾⲿ
▛ ▜
⫹⫺
⎱∰⎰
ᗻᗻᗻᗻᗻᗻᗻᗻ
ᗼᗼᗼᗼᗼᗼᗼᗼ
ᗻᗻᗻᗻᗻᗻᗻᗻ
ᛥᛥᛥᛥᛥᛥᛥᛥᛥ
ᛞ
▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏

▐ ░ ▒ ▓ ▔▕ ▖ ▗ ▘ ▙ ▚ ▛ ▜ ▝ ▞ ▟
*/
/*void mv_bldg_cursor(PANEL *pan, struct pos_t *pos, int dir)*/
/*{*/
        /*switch (dir) {*/
        /*case 'h':*/
                /*pos_l(pos);*/
                /*break;*/
        /*case 'H':*/
                /*pos_lstep(pos, 4);*/
                /*break;*/
        /*case 'l':*/
                /*pos_r(pos);*/
                /*break;*/
        /*case 'L':*/
                /*pos_rstep(pos, 4);*/
                /*break;*/
        /*case 'k':*/
                /*pos_u(pos);*/
                /*break;*/
        /*case 'K':*/
                /*pos_ustep(pos, 4);*/
                /*break;*/
        /*case 'j':*/
                /*pos_d(pos);*/
                /*break;*/
        /*case 'J':*/
                /*pos_dstep(pos, 4);*/
                /*break;*/
        /*}*/
        /*move_panel(pan, pos_y(pos), pos_x(pos));*/
/*}*/

/*WINDOW *bldg_win;*/
/*PANEL  *bldg_pan;*/
/*struct pos_t *bldg_pos;*/
struct cursor_t *bldg;


int place_building(int dir)
{
        if (!bldg)
                bldg = new_cursor(2, 3, 0, 0, LINES, COLS, 0, 0, HJKL);

        switch (dir) {
        case MODE_STARTED:
                bldg->Show(bldg);
                break;
        case 'm':
                bldg->Hide(bldg); 
                return MODE_RELEASE;
        default:
                bldg->Move(bldg, dir);
                break;
        }

        return MODE_PERSIST;
}



        

void bldg_test(void)
{


        const wchar_t *roof=L"▅▅▅";
        const wchar_t *wall=L"█▅█";

        const int wallpair = 254;
        const int roofpair = 253;

        init_pair(roofpair, HOUSEROOF, bgcolor_yx(PLATE(ACTIVE, BGR), 30, 40));
        init_pair(wallpair, HOUSEWALL, HOUSEROOF);

        mvcwpumpw(PLATE(ACTIVE, BGR), roofpair, 31, 40, L"%ls", roof);
        mvcwpumpw(PLATE(ACTIVE, BGR), wallpair, 32, 40, L"%ls", wall);

        mvwchgat(PLATE(ACTIVE, BGR), 32, 41, 1, A_REVERSE, wallpair, NULL);
}









