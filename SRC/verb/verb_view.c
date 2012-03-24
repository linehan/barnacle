/*//////////////////////////////////////////////////////////////////////////////
verb_view.c
///////////////////////////////////////////////////////////////////////////// */
#define _XOPEN_SOURCE_EXTENDED = 1
#include <assert.h>
#include <stdbool.h>
#include <menu.h>
#include "../gfx/gfx.h"
#include "../gfx/dock.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gfx/stdpan.h"
#include "../pan/test.h"
#include "verb_model.h"
#include "verb_control.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "../noun/noun_control.h"
#include "../lib/magic.h"

#define MAX (VIT_MAXLEN)
enum verb_elements { BASE, FUND, SEND, REC, CANC, TAKE };
////////////////////////////////////////////////////////////////////////////////
inline void put_n(WINDOW *win, int y, int x, wchar_t *wch, short pair, int n)
{
        assert(wch != NULL);

        static cchar_t cch;
        setcchar(&cch, wch, 0, pair, NULL);
        mvwhline_set(win, y, x, &cch, n);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                                verb menu                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#define VERB_W 18
#define VERB_H 8 
#define VERB_X 0 
#define VERB_Y LINES-VERB_H-3

WINDOW *verb_menu_win;
WINDOW *verb_menu_sub;
WINDOW *verb_menu_buf;
PANEL  *verb_menu_pan;
MENU   *verb_menu;


inline void init_verb_view(void)
{
        verb_menu_win = newwin(VERB_H, VERB_W, VERB_Y, VERB_X);
        stdpan(verb_menu_win, 
               &verb_menu_sub, 
               &verb_menu_buf, 
               &verb_menu_pan);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                     build a menu for a subset of verbs                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void init_verb_menu(uint32_t bitmap)
{
        if (verb_menu_win == NULL) init_verb_view(); 

        static char *name[100];
        static char *desc[100];
        static wchar_t *icon[100];
        int i, n;
        int *slice;

        n = ones(bitmap);
        slice = bitind(bitmap, n);

        werase(DIAGNOSTIC_WIN);
        for (i=0; i<n; i++) {
                wprintw(DIAGNOSTIC_WIN, "%i\n", slice[i]);
        }
                wprintw(DIAGNOSTIC_WIN, "n: %i\n", n);
        for (i=0; i<n; i++) {
                name[i] = verbs[slice[i]].name;
                desc[i] = verbs[slice[i]].desc;
        }

        verb_menu = make_new_menu(name, desc, NULL, n);

        ITEM **item = menu_items(verb_menu);
        for (i=0; i<n; i++) {
                set_item_userptr(item[i], &verbs[slice[i]]);
        }

        menu_wins(verb_menu, verb_menu_win, verb_menu_sub);
        menu_pair(verb_menu, PUR_DDP, PUR_GRE);
        menu_look(verb_menu, DESC, true, NULL);
        menu_look(verb_menu, MARK, false, NULL);
        set_menu_format(verb_menu, VERB_H-2, 1);
        menu_look(verb_menu, POST, true, NULL);

}

MENU *get_verb_menu(void)
{
        return (verb_menu);
}

PANEL *get_verb_panel(void)
{
        return (verb_menu_pan);
}

void post_verb_icon(ITEM *item)
{
        #define XPOS VERB_W-6
        struct verb_info *verb;
        static int y;
        int index;
        int top;

        top  = top_row(verb_menu);
        item = current_item(verb_menu);
        index = item_index(item);

        verb = (struct verb_info *)item_userptr(item);

        mvwdelch(verb_menu_sub, y, XPOS);

        y = (index-top);
        put_n(verb_menu_sub, y, XPOS, verb->icon, verb->pair, 1);
        win_refresh(verb_menu_sub);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                          display diagnostic gfx                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void do_pulse_test(struct verb_t *verb, int option)
{
        static int refresh;

        if (option == refresh) {
                werase(DIAGNOSTIC_WIN);
                refresh = option;
        }

        if (option == SUBJECT)
                wprintw(DIAGNOSTIC_WIN, "subject\n");
        else    wprintw(DIAGNOSTIC_WIN, "object\n");

        wprintw(DIAGNOSTIC_WIN, "mask %s\n", dispel(verb->mask));
        wprintw(DIAGNOSTIC_WIN, "bump %s\n", dispel(verb->bump));
        wprintw(DIAGNOSTIC_WIN, "fund %s\n", dispel(verb->fund));
        wprintw(DIAGNOSTIC_WIN, "send %s\n", dispel(verb->send));
        wprintw(DIAGNOSTIC_WIN, " rec %s\n", dispel(verb->rec));
        vrt_refresh();
        scr_refresh();
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            verb element views                              //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
inline void verb_view_elem(int elem, struct verb_t *verb, WINDOW *win, int opt)
{
        static wchar_t basics[]=L"ⲻⲺⲿⲾ";
        static wchar_t boom[]=L"✶✺";
        uint32_t xofs, xlen, vbid;
        wchar_t *icon;
        short pair;

        assert(verb != NULL);
        assert(win != NULL);

        if (opt == SUBJECT) goto subjects;
        else                goto objects;
        
        subjects:

        switch (elem) {
        case BASE:
                xofs = lzc(verb->bump);
                xlen = (MAX-xofs);
                goto gfx_base;
        case FUND:
                xofs = lzc(verb->fund);
                xlen = 1;
                goto gfx_fund;
        case SEND:
                xofs = lzc(verb->send);
                xlen = 1;
                goto gfx_send;
        case REC:
                xofs = lzc(verb->rec);
                xlen = 1;
                goto gfx_rec;
        case CANC: 
                xofs = lzc(verb->canc);
                xlen = 1;
                goto gfx_canc;
        case TAKE:
                xofs = lzc(verb->take);
                xlen = 1;
                goto gfx_take;
        }

        objects:

        switch (elem) {
        case BASE:
                xofs = 0;
                xlen = (MAX-lzc(verb->bump));
                goto gfx_base;
        case FUND:
                xofs = ((MAX-1)-lzc(verb->fund));
                xlen = 1;
                goto gfx_fund;
        case SEND:
                xofs = ((MAX-1)-lzc(verb->send));
                xlen = 1;
                goto gfx_send;
        case REC:
                xofs = ((MAX-1)-lzc(verb->rec));
                xlen = 1;
                goto gfx_rec;
        case CANC: 
                xofs = ((MAX-1)-lzc(verb->canc));
                xlen = 1;
                goto gfx_canc;
        case TAKE:
                xofs = ((MAX-1)-lzc(verb->take));
                xlen = 1;
                goto gfx_take;
        }


        gfx_base:
                icon = &basics[0];
                pair = TITLE_SCREEN;
                goto drawing;
        gfx_fund:
                icon = &basics[1];
                pair = TITLE_SCREEN;
                goto drawing;
        gfx_send:
                vbid = fifo_rdn(&verb->give, 0);
                icon = verbs[vbid].icon;
                pair = verbs[vbid].pair;
                goto drawing;
        gfx_rec:
                vbid = fifo_rdn(&verb->get, 0);
                icon = verbs[vbid].icon;
                pair = verbs[vbid].pair;
                goto drawing;
        gfx_canc:
                if (verb->canc == 0) return; // Cancels are only drawn
                else verb->canc = 0;         // for one frame.
                icon = &boom[0];
                pair = PUR_YEL;
                goto drawing;
        gfx_take:
                if (verb->take == 0) return; // Cancels are only drawn
                else verb->take = 0;         // for one frame.
                icon = &boom[0];
                pair = PUR_YEL;
                goto drawing;

        return; // prevent non-matches from falling into drawing

        drawing:
                put_n(win, 0, xofs, icon, pair, xlen); // draw it
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//      (\.   \      ,/)                                                      //
//       \(   |\     )/                                                       //
//       //\  | \   /\\                                                       //
//      (/ /\_#oo#_/\ \)      master verb viewer                              //
//       \/\  ####  /\/                                                       //
//            `##'                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void verb_view(uint32_t key, int opt)
{
        #define TESTING 0
        WINDOW *win; 
        opt %= 2;

        if (opt==SUBJECT) win = dock_window(SUBJ_WI_WIN);
        else              win = dock_window(OBJ_WI_WIN);

        focus(key);

        verb_view_elem(BASE, &focused->verb, win, opt);
        verb_view_elem(FUND, &focused->verb, win, opt);
        verb_view_elem(CANC, &focused->verb, win, opt);
        verb_view_elem(TAKE, &focused->verb, win, opt);
        if (&focused->verb.send != 0)
                verb_view_elem(SEND, &focused->verb, win, opt);
        if (&focused->verb.rec != 0)
                verb_view_elem(REC, &focused->verb, win, opt);

        win_refresh(win);

        if (TESTING) {
                do_pulse_test(&focused->verb, opt);
        }
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                         tick and decide what to view                       //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void do_pulse(void)
{
        int max;             
        int i;

        max = nnouns();
        for (i=0; i<max; i++) {

                if (keyring[i] == request_key(SUBJECT)) {
                        view_vitals(SUBJECT);
                        verb_view(keyring[i], SUBJECT);
                }

                if (keyring[i] == request_key(OBJECT)) {
                        view_vitals(OBJECT);
                        verb_view(keyring[i], OBJECT);
                }

                verb_tick(keyring[i]);
        }
}
