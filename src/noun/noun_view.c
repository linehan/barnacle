////////////////////////////////////////////////////////////////////////////////
//              _                                                             //
//            /(_))                                                           //
//          _/   /                                                            //
//         //   /                                                             //
//        //   /                                                              //
//        /\__/                                                               //
//        \O_/=-.                                                             //
//    _  / || \  ^o                                                           //
//    \\/ ()_) \.                        noun_view.c                          //
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
#include "../gfx/ui/stdpan.h"
#include "../gfx/ui/dock.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../test/test.h"
#include "noun_model.h"
#include "noun_view.h"

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
#define MENU_Y LINES-MENU_H-2
WINDOW *subj_menu_win, *obj_menu_win;
WINDOW *subj_menu_sub, *obj_menu_sub;
WINDOW *subj_menu_buf, *obj_menu_buf;
PANEL  *subj_menu_pan, *obj_menu_pan;
MENU   *subj_menu, *obj_menu;


void buildview_nouns(void)
{
        subj_menu_win = newwin(MENU_H, MENU_W, MENU_Y, MENU_X);
        stdpan(subj_menu_win, &subj_menu_sub, &subj_menu_buf, &subj_menu_pan);
        hide_panel(subj_menu_pan);

        obj_menu_win = newwin(MENU_H, MENU_W, MENU_Y, COLS-MENU_W-1);
        stdpan(obj_menu_win, &obj_menu_sub, &obj_menu_buf, &obj_menu_pan);
        hide_panel(obj_menu_pan);
}

void build_nouns(void)
{
        #define ORIGINAL PUR_GRE
        #define STANDOUT PUR_DYE
        #define OTANDOUT PUR_RED
        #define NROWS MENU_H-2
        #define NCOLS 1 

        uint32_t **skey;
        uint32_t **okey;
        char **subjects;
        char **objects;
        int nsubj;
        int nobj;
        int i;

        if (subj_menu_pan==NULL || obj_menu_pan==NULL) buildview_nouns();
        // --------------------------------------------------------------
        nsubj = nnouns();
        nobj  = nnouns();

        subjects = calloc(nsubj, sizeof(char *));
        objects  = calloc(nobj, sizeof(char *));

        skey = calloc(nsubj, sizeof(uint32_t *));
        okey = calloc(nobj, sizeof(uint32_t *));
        // --------------------------------------------------------------
        for (i=0; i<nsubj; i++) {
                subjects[i] = fullname(keyring[i]);
                skey[i] = &keyring[i];
        }
        for (i=0; i<nobj; i++) {
                objects[i]  = fullname(keyring[i]);
                okey[i] = &keyring[i];
        }
        // --------------------------------------------------------------
        subj_menu = make_new_menu(subjects, subjects, (void **)skey, nsubj);
        obj_menu = make_new_menu(objects, objects, (void **)okey, nobj);

        menu_wins(subj_menu, subj_menu_win, subj_menu_sub);
        menu_pair(subj_menu, STANDOUT, ORIGINAL);
        menu_look(subj_menu, DESC, false, NULL);
        menu_look(subj_menu, MARK, false, NULL);
        set_menu_format(subj_menu, NROWS, NCOLS);

        menu_wins(obj_menu, obj_menu_win, obj_menu_sub);
        menu_pair(obj_menu, PUR_DYE, ORIGINAL);
        menu_look(obj_menu, DESC, false, NULL);
        menu_look(obj_menu, MARK, false, NULL);
        set_menu_format(obj_menu, NROWS, NCOLS);
        // --------------------------------------------------------------
        menu_look(subj_menu, POST, true, NULL);
        menu_look(obj_menu, POST, true, NULL);
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


#define SUBJ_WIN dock_window(SUBJ_WI_WIN) 
#define OBJ_WIN dock_window(OBJ_WI_WIN) 


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

        focus(request_key(operand));
        xofs = X(operand);

        for (i=0; i<4; i++) {
                vital = get_vital(request_key(operand), i);
                if (operand==OBJECT)  xofs -= vital;
                put_nblocks(W(operand), 0, xofs, DBOX, VIT_PAIR[i], vital);
                if (operand==SUBJECT) xofs += vital;
        }
        win_refresh(W(operand));
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

        unpack_attributes(request_key(SUBJECT), val);
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
#define NOUNWIN(op) (op==SUBJECT) ? dock_window(SUBJ_ID_WIN) : dock_window(OBJ_ID_WIN) 

void view_noun(int op)
{
        if (op > 1) return;
        werase(NOUNWIN(op));

        if (op==SUBJECT) {
                wcolor_set(NOUNWIN(op), PUR_YEL, NULL);
                wprintw(NOUNWIN(op), "%-*s", ID_W,  fullname(request_key(op)));
        }
        if (op==OBJECT) {
                wcolor_set(NOUNWIN(op), PUR_YEL, NULL);
                wprintw(NOUNWIN(op), "%*s", ID_W, fullname(request_key(op)));
        }
        win_refresh(NOUNWIN(op));
}


void view_noun_grey(int op)
{
        if (op > 1) return;
        werase(NOUNWIN(op));
        if (op==SUBJECT) {
                wcolor_set(NOUNWIN(op), PUR_GRE, NULL);
                wprintw(NOUNWIN(op), "%-*s", ID_W, fullname(request_key(op)));
        }
        if (op==OBJECT) {
                wcolor_set(NOUNWIN(op), PUR_GRE, NULL);
                wprintw(NOUNWIN(op), "%*s", ID_W, fullname(request_key(op)));
        }
        win_refresh(NOUNWIN(op));
}
