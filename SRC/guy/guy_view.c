////////////////////////////////////////////////////////////////////////////////
//              _                                                             //
//            /(_))                                                           //
//          _/   /                                                            //
//         //   /                                                             //
//        //   /                                                              //
//        /\__/                                                               //
//        \O_/=-.                                                             //
//    _  / || \  ^o                                                           //
//    \\/ ()_) \.                        guy_view.c                           //
//     ^^ <__> \()                                                            //
//       //||\\                                                               //
//      //_||_\\                                                              //
//     // \||/ \\                                                             //
//    //   ||   \\                                                            //
//   \/    |/    \/                                                           //
//   /     |      \                                                           //
//  /      |       \                                                          //
//         |                                                                  //
////////////////////////////////////////////////////////////////////////////////
#define _XOPEN_SOURCE_EXTENDED = 1
#include <stdbool.h>
#include <menu.h>
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../pan/test.h"
#include "guy_model.h"
#include "guy_view.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                  keys                                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint32_t active_key[2];

void install_key(uint32_t key, int option)
{
        if (option!=OBJECT && option!=SUBJECT) return;
        else
                active_key[option] = key;
}


inline uint32_t request_key(int option)
{
        if (option!=OBJECT && option!=SUBJECT) return 0;
        else
                return (active_key[option]);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                dock window                                 //
//                                                                            //
//                                                                            //
/*//////////////////////////////////////////////////////////////////////////////
+------------------------------------------------------------------------------+
|                               dock parent window                             |
+------------------------------------------------------------------------------+
+-----------------------------------+      +-----------------------------------+
|          subject window           |      |           object window           |
+-----------------------------------+      +-----------------------------------+
+-----------+ +---------------------+      +---------------------+ +-----------+
| id window | |    widget window    |      |    widget window    | | id window |
+-----------+ +---------------------+      +---------------------+ +-----------+
*///////////////////////////////////////////////////////////////////////////////
#define DOCK_H 1
#define DOCK_W COLS
#define DOCK_Y LINES-DOCK_H
#define DOCK_X 0

#define OPERAND_W 54
#define ID_W 20
#define WIDGET_W 30

#define SUBJECT_X 2
#define OBJECT_X COLS-OPERAND_W-2

WINDOW *dock_win;
WINDOW *subject_win, *subj_id_win, *subj_wi_win;
WINDOW *object_win, *obj_id_win, *obj_wi_win;
PANEL  *dock_pan;


inline void buildview_dock(void)
{
        dock_win = newwin(DOCK_H, DOCK_W, DOCK_Y, DOCK_X);

        subject_win = derwin(   dock_win , DOCK_H , OPERAND_W , 0 , SUBJECT_X);
        subj_id_win = derwin(subject_win , DOCK_H , ID_W      , 0 ,         0);
        subj_wi_win = derwin(subject_win , DOCK_H , WIDGET_W  , 0 ,      ID_W);

        object_win  =  derwin(  dock_win , DOCK_H , OPERAND_W , 0 ,  OBJECT_X);
        obj_id_win  =  derwin(object_win , DOCK_H , ID_W      , 0 ,  OPERAND_W-ID_W);
        obj_wi_win  =  derwin(object_win , DOCK_H , WIDGET_W  , 0 ,         0);

        wbkgrnd(dock_win, &PURPLE[2]);
        wbkgrnd(subject_win, &PURPLE[2]);
        wbkgrnd(subj_id_win, &PURPLE[2]);
        wbkgrnd(subj_wi_win, &PURPLE[2]);
        wbkgrnd(object_win, &PURPLE[2]);
        wbkgrnd(obj_id_win, &PURPLE[2]);
        wbkgrnd(obj_wi_win, &PURPLE[2]);
        dock_pan = new_panel(dock_win);
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                noun menu                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#define MENU_W 20
#define MENU_H (LINES/3)
#define MENU_X 1
#define MENU_Y LINES-MENU_H-1
#define SUB_H MENU_H-2
#define SUB_W MENU_W-2
#define SUB_Y 1
#define SUB_X 1
#define BUF_H 1
#define BUF_W MENU_W
#define BUF_Y MENU_H-1
#define BUF_X 0
WINDOW *subj_menu_win, *obj_menu_win;
WINDOW *subj_menu_sub, *obj_menu_sub;
WINDOW *subj_menu_buf, *obj_menu_buf;
PANEL  *subj_menu_pan, *obj_menu_pan;
MENU   *subj_menu, *obj_menu;


void buildview_nouns(void)
{
        subj_menu_win = newwin(MENU_H, MENU_W, MENU_Y, MENU_X);
        subj_menu_pan = new_panel(subj_menu_win); 
        hide_panel(subj_menu_pan);

        subj_menu_sub = derwin(subj_menu_win, SUB_H, SUB_W, SUB_Y, SUB_X);
        subj_menu_buf = derwin(subj_menu_win, BUF_H, BUF_W, BUF_Y, BUF_X);

        wbkgrnd(subj_menu_win, &PURPLE[2]);
        wbkgrnd(subj_menu_sub, &PURPLE[2]);
        wbkgrnd(subj_menu_buf, &PURPLE[2]);


        obj_menu_win = newwin(MENU_H, MENU_W, MENU_Y, COLS-MENU_W-1);
        obj_menu_pan = new_panel(obj_menu_win); 
        hide_panel(obj_menu_pan);

        obj_menu_sub = derwin(obj_menu_win, SUB_H, SUB_W, SUB_Y, SUB_X);
        obj_menu_buf = derwin(obj_menu_win, BUF_H, BUF_W, BUF_Y, BUF_X);

        wbkgrnd(obj_menu_win, &PURPLE[2]);
        wbkgrnd(obj_menu_sub, &PURPLE[2]);
        wbkgrnd(obj_menu_buf, &PURPLE[2]);

}


void build_nouns(void)
{
        #define ORIGINAL PUR_GRE
        #define STANDOUT PUR_DYE
        #define OTANDOUT PUR_RED
        #define NROWS MENU_H-2
        #define NCOLS 1 

        char **subjects;
        char **objects;
        ITEM **sitems;
        ITEM **oitems;
        int nsubj;
        int nobj;
        int i;

        if (subj_menu_pan==NULL || obj_menu_pan==NULL) buildview_nouns();
        // --------------------------------------------------------------
        nsubj = nguys();
        nobj  = nguys();

        subjects = calloc(nsubj, sizeof(char *));
        objects  = calloc(nobj, sizeof(char *));

        sitems = calloc(nsubj+1, sizeof(ITEM *));
        oitems = calloc(nobj+1, sizeof(ITEM *));
        // --------------------------------------------------------------
        for (i=0; i<nsubj; i++) {
                subjects[i] = flname(keyring[i]);
                sitems[i] = new_item(subjects[i], subjects[i]);
                set_item_userptr(sitems[i], &keyring[i]);
        }
        sitems[nsubj] = (ITEM *)NULL;

        for (i=0; i<nobj; i++) {
                objects[i]  = flname(keyring[i]);
                oitems[i] = new_item(objects[i], objects[i]);
                set_item_userptr(oitems[i], &keyring[i]);
        }
        oitems[nobj] = (ITEM *)NULL;
        // --------------------------------------------------------------
        subj_menu = new_menu((ITEM **)sitems);

           set_menu_win(subj_menu, subj_menu_win);
           set_menu_sub(subj_menu, subj_menu_sub);
          set_menu_fore(subj_menu, COLOR_PAIR(STANDOUT));
          set_menu_back(subj_menu, COLOR_PAIR(ORIGINAL));
          menu_opts_off(subj_menu, O_SHOWDESC);
        set_menu_format(subj_menu, NROWS, NCOLS);
          set_menu_mark(subj_menu, "");
        // --------------------------------------------------------------
        obj_menu = new_menu((ITEM **)oitems);

           set_menu_win(obj_menu, obj_menu_win);
           set_menu_sub(obj_menu, obj_menu_sub);
          set_menu_fore(obj_menu, COLOR_PAIR(PUR_RED));
          set_menu_back(obj_menu, COLOR_PAIR(ORIGINAL));
          menu_opts_off(obj_menu, O_SHOWDESC);
        set_menu_format(obj_menu, NROWS, NCOLS);
          set_menu_mark(obj_menu, "");
        // --------------------------------------------------------------
        post_menu(subj_menu);
        post_menu(obj_menu);
}


MENU *get_noun_menu(int operand)
{
        return (operand == SUBJECT) ? subj_menu : obj_menu;
}

void open_nouns(int operand)
{
        PANEL *p = (operand==SUBJECT) ? subj_menu_pan : obj_menu_pan;
        if (panel_hidden(p)) show_panel(p);
        else                 hide_panel(p);
        vrt_refresh();
        scr_refresh();
}

void close_nouns(int operand)
{
        PANEL *p = (operand==SUBJECT) ? subj_menu_pan : obj_menu_pan;

        hide_panel(p);
        vrt_refresh();
}

void view_dock(void)
{
        if (dock_pan == NULL) buildview_dock();
        if (subj_menu_pan == NULL || obj_menu_pan == NULL) build_nouns();
        else                  top_panel(dock_pan);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                             pattern matching                               //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void *get_pattern(void)
{
        #define PATTERN_WIN subj_menu_buf
        #define PATTERN_PAN subj_menu_pan
        #define PATTERN_MENU subj_menu
        #define PATTERN (menu_pattern(PATTERN_MENU))
        #define KEY_ESC 27 // int value of ESC keycode

        static bool firstcall;
        int c;

        if (firstcall == true) goto PATTERN_LISTEN;
        else                   goto PATTERN_START;

        PATTERN_START:
                TOGPAN(PATTERN_PAN);
                wbkgrnd(PATTERN_WIN, &PURPLE[1]);
                wprintw(PATTERN_WIN, " /"); 
                firstcall = true;
                goto PATTERN_LISTEN;

        PATTERN_LISTEN:
                while ((c=getch()), c!='\n') {
                        switch (c) {
                        case KEY_ESC:
                                if (*(PATTERN)=='\0') goto PATTERN_END;
                                menu_driver(PATTERN_MENU, REQ_CLEAR_PATTERN);
                                break;
                        case KEY_BACKSPACE:
                                menu_driver(PATTERN_MENU, REQ_BACK_PATTERN);
                                break;
                        default:
                                menu_driver(PATTERN_MENU, c);
                                break;
                        }
                        werase(PATTERN_WIN);
                        wprintw(PATTERN_WIN, " /%s", PATTERN);
                        return (current_item(PATTERN_MENU));
                }
        PATTERN_END:
                menu_driver(PATTERN_MENU, REQ_CLEAR_PATTERN);
                werase(PATTERN_WIN);
                wbkgrnd(PATTERN_WIN, &PURPLE[3]);
                win_refresh(PATTERN_WIN);
                scr_refresh();
                firstcall = false;
                return (NULL);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            vitality widget                                 //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//           dark   medium  light   filled   large vert.    large hoz.        //
//              \     |     /         |         |              /              //
enum box_styles{DBOX,MBOX,LBOX,XBOX,FBOX,EBOX,LGVBAR,SMVBAR,LGHBAR,SMHBAR};   //
//                              /         |            /              \       //
//                        checkerboard   empty  small vert.        small hoz. //
////////////////////////////////////////////////////////////////////////////////
static const short   VIT_PAIR[]={PUR_RED, PUR_BRZ, PUR_BLU, PUR_GRE};
static const char   *VIT_TAGS[]={   "HP",    "SP",    "LP",    "EP"};
static const wchar_t  BOX_WCH[]=L"▩▨▧▦▣□ⲾⲿⲺⲻ";
static cchar_t BOX_CCH;


#define SUBJ_WIN subj_wi_win
#define OBJ_WIN obj_wi_win


inline void put_nblocks(WINDOW *win, int y, int x, int block, short pair, int n)
{
        setcchar(&BOX_CCH, &BOX_WCH[block], 0, pair, NULL);
        mvwhline_set(win, y, x, &BOX_CCH, n);
}


void view_vitals(int operand)
{
        if (operand!=SUBJECT && operand!=OBJECT) operand=SUBJECT;

        #define X(op) (op==SUBJECT) ? 0 : VIT_MAXLEN
        #define W(op) (op==SUBJECT) ? SUBJ_WIN : OBJ_WIN

        int vital;      // value of each vital in loop
        int i, xofs;

        werase(W(operand));
        put_nblocks(W(operand), 0, 0, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        focus(active_key[operand]);
        xofs = X(operand);

        for (i=0; i<4; i++) {
                vital = get_vital(active_key[operand], i);
                if (operand==OBJECT)  xofs -= vital;
                put_nblocks(W(operand), 0, xofs, DBOX, VIT_PAIR[i], vital);
                if (operand==SUBJECT) xofs += vital;
        }
        win_refresh(W(operand));
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                              pulse widget                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// The best method for counting bits in a 32-bit integer v is the following:
inline unsigned int ones(uint32_t v)
{
        v = v - ((v >> 1) & 0x55555555);                    // reuse input
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
        return (((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24);
/*
  The best bit counting method takes only 12 operations, which is the 
  same as the lookup-table method, but avoids the memory and potential 
  cache misses of a table. It is a hybrid between the purely parallel 
  method above and the earlier methods using multiplies (in the section 
  on counting bits with 64-bit instructions), though it doesn't use 
  64-bit instructions. The counts of bits set in the bytes is done in 
  parallel, and the sum total of the bits set in the bytes is computed 
  by multiplying by 0x1010101 and shifting right 24 bits. 
*/
}


// Count leading zeroes in a word
inline unsigned int lzc(uint32_t v)
{
        #define WORDBITS 32

        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        return(WORDBITS - ones(v));
}


void do_pulse(void)
{
        static const uint32_t rbump = 0x00000008;
        static const uint32_t lbump = 0xC0000000;

        #define BLOCKSTYLE SMHBAR, TITLE_SCREEN
        #define PULSESTYLE LGHBAR, TITLE_SCREEN

        uint32_t fundamental; // fundamental pulse state
        bool forward;         // direction of pulse
        int xmin;             // starting x offset
        int max;              // loop boundary
        int i;

        max = nguys();

        for (i=0; i<max; i++) {
                focus(keyring[i]);
                fundamental = focused->fundamental;
                forward     = focused->forward;
                xmin        = vit_blocklen(keyring[i]);

                if (keyring[i] == active_key[SUBJECT]) {
                        put_nblocks(SUBJ_WIN, 0, xmin, BLOCKSTYLE, (VIT_MAXLEN-xmin));
                        put_nblocks(SUBJ_WIN, 0, lzc(fundamental), PULSESTYLE, 1);
                }
                if (keyring[i] == active_key[OBJECT]) {
                        put_nblocks(OBJ_WIN, 0, 0, BLOCKSTYLE, VIT_MAXLEN-xmin);
                        put_nblocks(OBJ_WIN, 0, (VIT_MAXLEN-1)-lzc(fundamental), PULSESTYLE, 1);
                }
                win_refresh(SUBJ_WIN);
                win_refresh(OBJ_WIN);

                if (forward) focused->fundamental>>=1;
                else         focused->fundamental<<=1;

                if ((fundamental|rbump)         == rbump) forward=false;
                if (((fundamental<<xmin)|lbump) == lbump) forward=true;

                focused->forward = forward;
        }
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                           attributes widget                                //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void view_attributes(void)
{
        #define STRIDE 5
        #define ATTR_STANDOUT PUR_GRE
        #define ATTR_ORIGINAL PUR_PUR

        int val[8];
        int i;

        werase(SUBJ_WIN);
        wcolor_set(SUBJ_WIN, ATTR_STANDOUT, NULL);
        waddwstr(SUBJ_WIN, L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(SUBJ_WIN, ATTR_ORIGINAL, NULL);

        unpack_attributes(active_key[SUBJECT], val);
        for (i=0; i<8; i++) {
                mvwprintw(SUBJ_WIN, 0, (i*STRIDE)+1, "%02u", val[i]);
        }
        win_refresh(SUBJ_WIN);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                              names widget                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#define NOUN_STANDOUT PUR_YEL
#define NOUN_BRIGHTER PUR_GRE
#define NOUN_ORIGINAL PUR_PUR
#define NOUNWIN(op) (op==SUBJECT) ? subj_id_win : obj_id_win

void view_noun(int op)
{
        if (op > 1) return;
        werase(NOUNWIN(op));

        if (op==SUBJECT) {
                wcolor_set(NOUNWIN(op), PUR_YEL, NULL);
                wprintw(NOUNWIN(op), "%-*s",ID_W,  flname(active_key[op]));
        }
        if (op==OBJECT) {
                wcolor_set(NOUNWIN(op), PUR_RED, NULL);
                wprintw(NOUNWIN(op), "%*s", ID_W, flname(active_key[op]));
        }
        win_refresh(NOUNWIN(op));
}


void view_noun_grey(int op)
{
        if (op > 1) return;
        werase(NOUNWIN(op));
        if (op==SUBJECT) {
                wcolor_set(NOUNWIN(op), PUR_GRE, NULL);
                wprintw(NOUNWIN(op), "%-*s", ID_W, flname(active_key[op]));
        }
        if (op==OBJECT) {
                wcolor_set(NOUNWIN(op), PUR_GRE, NULL);
                wprintw(NOUNWIN(op), "%*s", ID_W, flname(active_key[op]));
        }
        win_refresh(NOUNWIN(op));
}
