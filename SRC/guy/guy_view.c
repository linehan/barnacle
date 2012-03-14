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
        if (option!=KEY_OBJECT && option!=KEY_SUBJECT) return;
        else
                active_key[option] = key;
}


inline uint32_t request_key(int option)
{
        if (option!=KEY_OBJECT && option!=KEY_SUBJECT) return 0;
        else
                return (active_key[option]);
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
#define MENU_Y LINES-MENU_H
#define SUB_H MENU_H-3
#define SUB_W MENU_W-2
#define SUB_Y 1
#define SUB_X 1
#define BUF_H 1
#define BUF_W MENU_W
#define BUF_Y MENU_H-1
#define BUF_X 0
WINDOW *noun_win;
WINDOW *noun_sub;
WINDOW *noun_buf;
PANEL  *noun_pan;
MENU   *noun_menu;


void buildview_nouns(void)
{
        noun_win = newwin(MENU_H, MENU_W, MENU_Y, MENU_X);
        noun_pan = new_panel(noun_win); 
        hide_panel(noun_pan);

        noun_sub = derwin(noun_win, SUB_H, SUB_W, SUB_Y, SUB_X);
        noun_buf = derwin(noun_win, BUF_H, BUF_W, BUF_Y, BUF_X);

        wbkgrnd(noun_win, &PURPLE[2]);
        wbkgrnd(noun_sub, &PURPLE[2]);
        wbkgrnd(noun_buf, &PURPLE[2]);
}


void view_nouns(void)
{
        #define ORIGINAL PUR_PUR
        #define STANDOUT PUR_YEL
        #define NROWS MENU_H-2
        #define NCOLS 1 

        char **nouns;
        ITEM **items;
        int i, n;

        if (noun_pan == NULL) buildview_nouns();

        n     = nguys();
        nouns = calloc(n, sizeof(char *));
        items = calloc(n+1, sizeof(ITEM *));

        for (i=0; i<n; i++) {
                nouns[i] = flname(keyring[i]);
                items[i] = new_item(nouns[i], nouns[i]);
                set_item_userptr(items[i], &keyring[i]);
        }
        items[n] = (ITEM *)NULL;

        noun_menu = new_menu((ITEM **)items);

         set_menu_win(noun_menu, noun_win);
         set_menu_sub(noun_menu, noun_sub);
        set_menu_fore(noun_menu, COLOR_PAIR(STANDOUT));
        set_menu_back(noun_menu, COLOR_PAIR(ORIGINAL));
        menu_opts_off(noun_menu, O_SHOWDESC);

        set_menu_format(noun_menu, NROWS, NCOLS);
        set_menu_mark(noun_menu, "");

        post_menu(noun_menu);
}


MENU *get_noun_menu(void)
{
        return (noun_menu);
}


void close_nouns(void)
{
        hide_panel(noun_pan);
        vrt_refresh();
        scr_refresh();
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                dock window                                 //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*
+------------------------------------------------------------------------------+
|                               dock parent window                             |
+------------------------------------------------------------------------------+
+-----------------------------------+      +-----------------------------------+
|          subject window           |      |           object window           |
+-----------------------------------+      +-----------------------------------+
+-----------+ +---------------------+      +---------------------+ +-----------+
| id window | |    widget window    |      |    widget window    | | id window |
+-----------+ +---------------------+      +---------------------+ +-----------+
*/
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
        obj_id_win  =  derwin(object_win , DOCK_H , ID_W      , 0 ,  WIDGET_W);
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


void view_dock(void)
{
        if (dock_pan == NULL) buildview_dock();
        if (noun_pan == NULL) view_nouns();
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
        #define PATTERN_WIN noun_buf
        #define PATTERN_PAN noun_pan
        #define PATTERN_MENU noun_menu
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
        if (operand!=KEY_SUBJECT && operand!=KEY_OBJECT) operand=KEY_SUBJECT;

        #define X(op) (op==KEY_SUBJECT) ? 0 : VIT_MAXLEN
        #define W(op) (op==KEY_SUBJECT) ? SUBJ_WIN : OBJ_WIN

        uint8_t xpulse; // x-offset of the pulse peak
        int vital;      // value of each vital in loop
        int i, xofs;

        werase(W(operand));
        put_nblocks(W(operand), 0, 0, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        focus(active_key[operand]);
        xpulse = focused->xpulse;
        xofs = X(operand);

        for (i=0; i<4; i++) {
                vital = get_vital(active_key[operand], i);
                if (operand==KEY_OBJECT)  xofs -= vital;
                put_nblocks(W(operand), 0, xofs, DBOX, VIT_PAIR[i], vital);
                if (operand==KEY_SUBJECT) xofs += vital;

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
void do_pulse(void)
{
        #define BLOCKSTYLE SMHBAR, TITLE_SCREEN
        #define PULSESTYLE LGHBAR, TITLE_SCREEN

        uint32_t action;  // action state
        int xmin;         // starting x offset
        int pulse;        // position of pulse
        bool forward;     // direction of pulse
        int max=nguys();  // loop boundary
        int i;

        for (i=0; i<max; i++) {
                focus(keyring[i]);
                pulse   = focused->xpulse;
                xmin    = vit_blocklen(keyring[i]);
                forward = focused->forward;
                action  = focused->action;

                if (keyring[i] == active_key[KEY_SUBJECT]) {
                        put_nblocks(SUBJ_WIN, 0, xmin, BLOCKSTYLE, (VIT_MAXLEN-xmin));
                        if (action != 0) {
                                if (forward)
                                        put_nblocks(SUBJ_WIN, 0, pulse+1, BLOCKSTYLE, 1);
                                else
                                        put_nblocks(SUBJ_WIN, 0, pulse-1, BLOCKSTYLE, 1);
                        }
                        put_nblocks(SUBJ_WIN, 0, pulse, PULSESTYLE, 1);
                        win_refresh(SUBJ_WIN);
                }

                if (forward)  pulse++; 
                else          pulse--;

                if (pulse>VIT_MAXLEN-2) forward=false;
                if (pulse<=xmin)        forward=true;

                if (pulse == VIT_MAXLEN-1 || pulse == xmin) action = 0;

                focused->xpulse  = pulse;
                focused->forward = forward;
                focused->action  = action;
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

        unpack_attributes(active_key[KEY_SUBJECT], val);
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
#define NOUNWIN subj_id_win


void view_noun(void)
{
        werase(NOUNWIN);
        wcolor_set(NOUNWIN, NOUN_STANDOUT, NULL);
        wprintw(NOUNWIN, "%s", flname(active_key[KEY_SUBJECT]));
        win_refresh(NOUNWIN);
}


void view_noun_grey(void)
{
        werase(NOUNWIN);
        wcolor_set(NOUNWIN, NOUN_BRIGHTER, NULL);
        wprintw(NOUNWIN, "%s", flname(active_key[KEY_SUBJECT]));
        win_refresh(NOUNWIN);
}
