#define _XOPEN_SOURCE_EXTENDED = 1
#include <stdbool.h>
#include <menu.h>
#include "../gfx/gfx.h"
#include "../gfx/dock.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../pan/test.h"
#include "verb_model.h"
#include "verb_control.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "../noun/noun_control.h"
#include "../lib/magic.h"

#define MAX (VIT_MAXLEN)
enum verb_elements { BASE, FUND, SEND, REC, CANC };
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                          display diagnostic gfx                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void do_pulse_test(struct verb_t *verb, int option)
{
        if (verb == NULL) {
                werase(DIAGNOSTIC_WIN);
                return;
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
inline void put_n(WINDOW *win, int y, int x, wchar_t *wch, short pair, int n)
{
        static cchar_t cch;
        setcchar(&cch, wch, 0, pair, NULL);
        mvwhline_set(win, y, x, &cch, n);
}
////////////////////////////////////////////////////////////////////////////////
inline void verb_view_elem(int elem, struct verb_t *verb, WINDOW *win, int opt)
{
        static wchar_t basics[]=L"ⲻⲺ";
        static wchar_t boom[]=L"✶";
        uint32_t xofs, xlen, vbid;
        wchar_t *wch;
        short pair;

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
        }


        gfx_base:
                wch  = &basics[0];
                pair = TITLE_SCREEN;
                goto drawing;
        gfx_fund:
                wch  = &basics[1];
                pair = TITLE_SCREEN;
                goto drawing;
        gfx_send:
                wch  = verbs[fifo_peek(&verb->give, 0)].wch;
                pair = verbs[fifo_peek(&verb->give, 0)].pair;
                goto drawing;
        gfx_rec:
                wch  = verbs[fifo_peek(&verb->get, 0)].wch;
                pair = verbs[fifo_peek(&verb->get, 0)].pair;
                goto drawing;
        gfx_canc:
                if (verb->canc == 0) return; // Cancels are only drawn
                else verb->canc = 0;         // for one frame.
                wch  = &boom[0];
                pair = PUR_YEL;
                goto drawing;


        drawing:
                put_n(win, 0, xofs, wch, pair, xlen); // draw it
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            master verb viewer                              //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void verb_view(uint32_t key, int opt)
{
        #define TESTING 1
        WINDOW *win; 
        opt %= 2;

        if (opt==SUBJECT) win = dock_window(SUBJ_WI_WIN);
        else              win = dock_window(OBJ_WI_WIN);

        focus(key);

        verb_view_elem(BASE, &focused->verb, win, opt);
        verb_view_elem(FUND, &focused->verb, win, opt);
        verb_view_elem(SEND, &focused->verb, win, opt);
        verb_view_elem(CANC, &focused->verb, win, opt);
        verb_view_elem(REC, &focused->verb, win, opt);

        win_refresh(win);

        if (TESTING) {
                do_pulse_test(NULL, SUBJECT);
                do_pulse_test(&focused->verb, SUBJECT);
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

                if (keyring[i] == request_key(SUBJECT)) 
                        verb_view(keyring[i], SUBJECT);

                if (keyring[i] == request_key(OBJECT)) 
                        verb_view(keyring[i], OBJECT);

                verb_tick(keyring[i]);
        }
}
