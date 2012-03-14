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
        top_panel(noun_pan);
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
//                              dock window                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#define DOCK_W COLS
#define DOCK_H 1
#define DOCK_X 0
#define DOCK_Y LINES-DOCK_H
#define DOCKNAM_H 1
#define DOCKNAM_W 20
#define DOCKNAM_Y 0
#define DOCKNAM_X 2
#define DOCKBUF_H 1
#define DOCKBUF_W COLS-25
#define DOCKBUF_Y 0
#define DOCKBUF_X 22
WINDOW *dock_win;
WINDOW *dock_nam;
WINDOW *dock_buf;
PANEL  *dock_pan;


inline void buildview_dock(void)
{
        dock_win = newwin(DOCK_H, DOCK_W, DOCK_Y, DOCK_X);
        dock_nam = derwin(dock_win, DOCKNAM_H, DOCKNAM_W, DOCKNAM_Y, DOCKNAM_X);
        dock_buf = derwin(dock_win, DOCKBUF_H, DOCKBUF_W, DOCKBUF_Y, DOCKBUF_X);
        wbkgrnd(dock_win, &PURPLE[2]);
        wbkgrnd(dock_nam, &PURPLE[2]);
        wbkgrnd(dock_buf, &PURPLE[2]);
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
                while ((c=getch()) != '\n') {
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
                menu_driver(noun_menu, REQ_CLEAR_PATTERN);
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


#define WIDGETS dock_buf
uint32_t active_key;


inline void put_nblocks(int y, int x, int block, short pair, int n)
{
        setcchar(&BOX_CCH, &BOX_WCH[block], 0, pair, NULL);
        mvwhline_set(WIDGETS, y, x, &BOX_CCH, n);
}


void view_vitals(void)
{
        uint8_t xpulse; // x-offset of the pulse peak
        int vital;      // value of each vital in loop
        int i, xofs;

        focus(active_key);
        xpulse = focused->xpulse;

        werase(WIDGETS);
        put_nblocks(0, 0, SMHBAR, TITLE_SCREEN, VIT_MAXLEN);

        for (i=0; i<4; i++) {
                vital = get_vital(active_key, i);
                put_nblocks(0, xofs, DBOX, VIT_PAIR[i], vital);
                xofs += vital;
        }
        win_refresh(WIDGETS);
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

                if (keyring[i] == active_key) {
                        put_nblocks(0, xmin, BLOCKSTYLE, (VIT_MAXLEN-xmin));
                        if (action != 0) {
                                if (forward)
                                        put_nblocks(0, pulse+1, BLOCKSTYLE, 1);
                                else
                                        put_nblocks(0, pulse-1, BLOCKSTYLE, 1);
                        }
                        put_nblocks(0, pulse, PULSESTYLE, 1);
                        win_refresh(WIDGETS);
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

        werase(WIDGETS);
        wcolor_set(WIDGETS, ATTR_STANDOUT, NULL);
        waddwstr(WIDGETS, L"Σ    Φ    Δ    A    Ψ    W    Χ    Λ");
        wcolor_set(WIDGETS, ATTR_ORIGINAL, NULL);

        unpack_attributes(active_key, val);
        for (i=0; i<8; i++) {
                mvwprintw(WIDGETS, 0, (i*STRIDE)+1, "%02u", val[i]);
        }
        win_refresh(WIDGETS);
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
#define NOUNWIN dock_nam


void view_noun(void)
{
        werase(NOUNWIN);
        wcolor_set(NOUNWIN, NOUN_STANDOUT, NULL);
        wprintw(NOUNWIN, "%s", flname(active_key));
        win_refresh(NOUNWIN);
}


void view_noun_grey(void)
{
        werase(NOUNWIN);
        wcolor_set(NOUNWIN, NOUN_BRIGHTER, NULL);
        wprintw(NOUNWIN, "%s", flname(active_key));
        win_refresh(NOUNWIN);
}
