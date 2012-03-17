#define _XOPEN_SOURCE_EXTENDED = 1 
#include <stdio.h>
#include <stdlib.h>
#include "../lib/fifo.h"
#include "../gfx/palette.h"
#include "../gfx/gfx.h"
#include "../gfx/dock.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "verb_model.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            shift the send buffer                           //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
inline void verb_tick_send(uint32_t key)
{
        uint32_t fund;
        uint32_t send; 
        uint32_t bump;

        focus(key);
        if (focused->verb.send == 0U) return; // nothing to do
        update_mask(key);

        fund = focused->verb.fund;
        send = focused->verb.send;
        bump = focused->verb.bump;

        // fund 1 right of send
        if (((fund << 1) == send) && send!=bump) 
                send <<= 1;

        // fund 1 left of send
        if ((fund >> 1) == send)                 
                send >>= 1;

        // fund co-incident with send
        if ((fund == send)) {
                if (focused->verb.forw == true)
                        send >>= 1;
                else if (send == 0x00000001)   
                        send >>= 1;
                else    send <<= 1;
        }

        focused->verb.send = (send==0U) ? verb_send(key) : send;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                          shift the receive buffer                          //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
inline void verb_tick_rec(uint32_t key)
{
        uint32_t rec;
        uint32_t bump;

        focus(key);

        rec  = focused->verb.rec;
        bump = focused->verb.bump;

        if (rec == 0U) return; // nothing to do

        rec <<= 1;
        focused->verb.rec = ((rec&bump)==bump) ? verb_do(key) : rec;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                        shift the fundamental buffer                        //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void verb_tick(uint32_t key)
{
        // 1. The fundamental's direction oscillates
        // 2. The send buffer always shifts away from the origin (to the right)
        // 3. The receive buffer always shifts toward the origin (to the left)
        focus(key);
        update_mask(key);

        if ((focused->verb.forw == true)) 
                focused->verb.fund >>= 1;
        else    focused->verb.fund <<= 1;

        if ((focused->verb.fund == 0x00000001))
                        focused->verb.forw=false;
        if (focused->verb.fund == focused->verb.bump) 
                        focused->verb.forw=true;

        verb_tick_send(key);
        verb_tick_rec(key);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                         cancel the verb being sent                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void verb_cancel(uint32_t key)
{
        focus(key);
        focused->verb.canc  = focused->verb.send;
        focused->verb.send ^= focused->verb.send;
}
