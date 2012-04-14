/* 
 * noun_widgets.c 
 *
 * Various subroutines to display information about an operand in one of 
 * the noun menus. This operand is usually the noun currently selected in
 * the noun menu, although there may be other entry points.
 *
 */

#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdpan.h"
#include "../gfx/ui/stdmenu.h"
#include "../gfx/ui/dock.h"
#include "../test/test.h"
#include "noun_model.h"
#include "noun_view.h"


/* -------------------------------------------------------------------------- */
#define IDWIN(op) (op == SUBJECT) ? (dock_window(SUBJ_ID_WIN)) : (dock_window(OBJ_ID_WIN))
#define WIWIN(op) (op == SUBJECT) ? (dock_window(SUBJ_WI_WIN)) : (dock_window(OBJ_WI_WIN))
/* -------------------------------------------------------------------------- */


/*
 * noun_print_highlight -- apply a highlight attribute to the operand
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void noun_print_highlight(int op)
{
        struct noun_t *noun;
        attr_t attr;
        short pair;

        assert(op == SUBJECT || op == OBJECT);

        noun = current_noun(op);
        wattr_get(mob_win(&noun->mob), &attr, &pair, NULL);

        /* 
         * ncurses doesn't really provide an easy way to compare
         * attr_t, which is just a typedef'd int. If you don't
         * believe me, change the if statement to test whether
         * attr == WA_REVERSE, and see what happens. Anyway, we
         * have to play ball, and the ballgame also includes
         * seemingly redundant calls to wattr_off/on and wchgat...
         * try factoring it and see what happens. But why? Why?!
         */
        if ((attr & WA_REVERSE) == WA_REVERSE) {
                /*wprintw(CONSOLE_WIN, "It's reversed!\n");*/
                wattr_off(mob_win(&noun->mob), WA_REVERSE, NULL);
                wchgat(mob_win(&noun->mob), -1, WA_NORMAL, pair, NULL);
        } else {
                /*wprintw(CONSOLE_WIN, "It's not reversed!\n");*/
                wattr_on(mob_win(&noun->mob), WA_REVERSE, NULL);
                wchgat(mob_win(&noun->mob), -1, WA_REVERSE, pair, NULL);
        }
        scr_refresh();
}


/*
 * noun_print_name -- print the name of the currently selected noun
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 * @pair: the color pair of the printed text
 */
void noun_print_name(int op, short pair)
{
        assert(op == SUBJECT || op == OBJECT);

        werase(IDWIN(op));
        wcolor_set(IDWIN(op), pair, NULL);

        if (op==SUBJECT) 
                wprintw(IDWIN(op), "%-*s", ID_W,  fullname(request_id(op)));
        else
                wprintw(IDWIN(op), "%*s", ID_W, fullname(request_id(op)));

        win_refresh(IDWIN(op));
}



/*
 * put_nblocks -- draw some boxes... this is pretty old code and should
 * probably be factored out 
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */

//           dark   medium  light   filled   large vert.    large hoz.        //
//              \     |     /         |         |              /              //
enum box_styles{DBOX,MBOX,LBOX,XBOX,FBOX,EBOX,LGVBAR,SMVBAR,LGHBAR,SMHBAR};   //
//                              /         |            /              \       //
//                        checkerboard   empty  small vert.        small hoz. //


static const short   VIT_PAIR[]={PUR_RED, PUR_BRZ, PUR_BLU, PUR_GRE};
static const char   *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};
static const wchar_t  BOX_WCH[]=L"▩▨▧▦▣□ⲾⲿⲺⲻ";
static cchar_t BOX_CCH;


inline void put_nblocks(WINDOW *win, int y, int x, int block, short pair, int n)
{
        setcchar(&BOX_CCH, &BOX_WCH[block], 0, pair, NULL);
        mvwhline_set(win, y, x, &BOX_CCH, n);
}



/*
 * noun_print_vitals -- draw the vitals bar from the noun's vitals word 
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void noun_print_vitals(int op)
{
        int vital; /* Value of each vital byte */ 
        int xofs;  /* Offset for put_nblocks */
        int i;

        assert(op == SUBJECT || op == OBJECT);

        op   = (op!=SUBJECT && op!=OBJECT) ? SUBJECT : op;
        xofs = (op==SUBJECT) ? 0 : VIT_MAXLEN;

        werase(WIWIN(op));
        put_nblocks(WIWIN(op), 0, 0, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        key_noun(request_id(op));

        for (i=0; i<4; i++) {
                vital = get_vital(request_id(op), i);
                if (op==OBJECT)  xofs -= vital;
                put_nblocks(WIWIN(op), 0, xofs, DBOX, VIT_PAIR[i], vital);
                if (op==SUBJECT) xofs += vital;
        }
        win_refresh(WIWIN(op));
}

/*
 * noun_print_attributes -- draw the attributes with some fanciness
 * @op: the operand, either SUBJECT (left menu) or OBJECT (right menu)
 */
void noun_print_attributes(int op)
{
        #define ATTR_STANDOUT PUR_GRE
        #define ATTR_ORIGINAL PUR_PUR

        const int STRIDE=5;
        int val[8];
        int i;

        assert(op == SUBJECT || op == OBJECT);

        werase(WIWIN(op));
        wcolor_set(WIWIN(op), ATTR_STANDOUT, NULL);
        waddwstr(WIWIN(op), L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(WIWIN(op), ATTR_ORIGINAL, NULL);

        unpack_attributes(request_id(SUBJECT), val);
        for (i=0; i<8; i++) {
                mvwprintw(WIWIN(op), 0, (i*STRIDE)+1, "%02u", val[i]);
        }
        win_refresh(WIWIN(op));
}

