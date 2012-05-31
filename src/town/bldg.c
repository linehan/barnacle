#include "../com/barnacle.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../lib/textutils.h"




/*
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
*/


void bldg_test(void)
{
        const wchar_t *peak=L" ▁▁▁▁▁▁ ";
        const wchar_t *roof=L"⎛⎛⎛⎛⎞⎞⎞⎞";
        const wchar_t *wall=L"ᛞ⋿ᛞᗶᗶᛞ⋿ᛞ";

        const int wallpair = 254;
        const int roofpair = 253;
        const int peakpair = 252;

        init_pair(peakpair, __DGREEN, bgcolor_yx(PLATE(ACTIVE, BGR), 30, 40));
        init_pair(roofpair, __GREEN, __DGREEN);
        /*init_pair(wallpair, _WOOD, BROWN);*/

        mvcwpumpw(PLATE(ACTIVE, BGR), peakpair, 30, 40, L"%ls", peak);
        mvcwpumpw(PLATE(ACTIVE, BGR), roofpair, 31, 40, L"%ls", roof);
        mvcwpumpw(PLATE(ACTIVE, BGR), wallpair, 32, 40, L"%ls", wall);
}




