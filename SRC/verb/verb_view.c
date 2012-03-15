#define _XOPEN_SOURCE_EXTENDED = 1
#include <stdbool.h>
#include <menu.h>
#include "../gfx/gfx.h"
#include "../gfx/dock.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "verb_model.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "../noun/noun_control.h"


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            view verb packet                                //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void view_verb(WINDOW *win, uint32_t skey)
{
        #include "../lib/magic.h"

        static wchar_t iconwch[]=L"⊕⊖⊗⊘⊙⊛⊜⊝";
        static cchar_t iconcch;

        uint32_t fundamental;
        uint32_t verb;
        uint8_t xmin;
        uint8_t lzf, lzv;

        focus(skey);

        fundamental = focused->fundamental;
        verb = focused->verb;
        xmin = vit_blocklen(skey);

        if (verb != 0) {

                lzf = lzc(fundamental);
                lzv = lzc(verb);

                // Pulse is co-incident with verb at block terminal
                if (lzv==lzf) {
                        focused->verb >>= 1;
                        lzv += 1;
                }
                // Pulse is co-incident with verb at free terminal
                if (lzv==lzf && lzv>30) {
                        focused->verb <<= 1;
                        lzv -= 1;
                }

                setcchar(&iconcch, &iconwch[0], 0, PUR_BLU, NULL);
                mvwadd_wch(win, 0, lzv, &iconcch);

                // Pulse moving left behind verb
                if (lzf==(lzv+1)) {
                        if ((verb << xmin) == 0) {
                                do_verb(skey, 0);
                                focused->verb = verb;
                                operate_on(&skey);
                        }
                        else focused->verb << 1;
                }                        

                // Pulse moving right behind verb
                else if (lzv==(lzf+1)) {
                        if ((verb >> 3) == 0) {
                                do_verb(skey, 0);
                                focused->verb = 0;
                                operate_on(&skey);
                        }
                        else focused->verb >> 1;
                }


        }
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                              pulse widget                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
inline void put_n(WINDOW *win, int y, int x, wchar_t *wch, short pair, int n)
{
        static cchar_t cch;
        setcchar(&cch, wch, 0, pair, NULL);
        mvwhline_set(win, y, x, &cch, n);
}

void do_pulse(void)
{
        #include "../lib/magic.h"
        static const uint32_t rbump = 0x00000008;
        static const uint32_t lbump = 0xC0000000;
        static wchar_t wch[]=L"ⲾⲿⲺⲻ";

        #define BASELINE &wch[3], TITLE_SCREEN
        #define PULSING  &wch[2], TITLE_SCREEN
        #define SUBJ_WIN dock_window(SUBJ_WI_WIN)
        #define OBJ_WIN dock_window(OBJ_WI_WIN)

        uint32_t fundamental; // fundamental pulse state
        uint32_t verb;
        bool forward;         // direction of pulse
        int xmin;             // starting x offset
        int max;              // loop boundary
        int i;

        max = nnouns();

        for (i=0; i<max; i++) {
                focus(keyring[i]);
                fundamental = focused->fundamental;
                forward     = focused->forward;
                xmin        = vit_blocklen(keyring[i]);

                if (keyring[i] == request_key(SUBJECT)) {
                        put_n(SUBJ_WIN, 0, xmin, BASELINE, (VIT_MAXLEN-xmin));
                        put_n(SUBJ_WIN, 0, lzc(fundamental), PULSING, 1);
                        view_verb(SUBJ_WIN, request_key(SUBJECT));
                }
                if (keyring[i] == request_key(OBJECT)) {
                        put_n(OBJ_WIN, 0, 0, BASELINE, VIT_MAXLEN-xmin);
                        put_n(OBJ_WIN, 0, (VIT_MAXLEN-1)-lzc(fundamental), PULSING, 1);
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
