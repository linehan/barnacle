////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                 (>,                        //
//                                             ,<)oo |\                       //
//       oo              (>,  oo              / 88 "}| \           o (>,      //
//      8 "} > @ <        /  8 "} > @ <       | `., ., ,'         8 oo |\     //
//     .8 .-._/|          |`.8 .-._/|          `-.'`')'            8 "}| \    //
//   .'_'`')`_.'          `-.'`')`_.'             ) /              , ., ,'    //
//   \| ) /                  ) /                 / (__,_          ('`')'      //
//  (>'/  |_,_              /  |__,             |   (-,/          )\____,<)   //
//     |    (,|             |    ( /          .'    ) /          / (__,_      //
//   .'    ,'\|           .'    , /            `._,\ '`-         |   (-,/     //
//    `._/    )            `._/  '`-               `\          .'    ) /      //
//     \|     '`-           \|                     -`'          `._,\ '`-     //
//--   ''--        --- --  -`' -        ---   --  - -        ---   -`' - ---  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
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
//                               verb functions                               //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void hp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, HP);
}
void hp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, HP);
}
//----------------------------------------------------------------------------//
void sp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, SP);
}
void sp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, SP);
}
//----------------------------------------------------------------------------//
void lp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, LP);
}
void lp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, LP);
}
//----------------------------------------------------------------------------//
void ep_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, EP);
}
void ep_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, EP);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                               verb varieties                               //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
struct verb_info verbs[]={{L"⊕", PUR_EEN, hp_inc},
                          {L"⊖", PUR_RED, hp_dec},
                          {L"⊗", PUR_BLU, sp_inc},
                          {L"⊘", PUR_YEL, sp_dec},
                          {L"⊙", PUR_PUR, lp_inc},
                          {L"⊛", PUR_GRE, lp_dec},
                          {L"⊜", PUR_PUR, ep_inc},
                          {L"⊝", PUR_GRE, ep_dec}};
////////////////////////////////////////////////////////////////////////////////
inline uint32_t update_mask(uint32_t key)
{
        focus(key);
        focused->verb.mask = (~0U >> vit_blocklen(key));
        focused->verb.bump = 0x80000000 >> lzc(focused->verb.mask);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                            signal a new verb                               //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
uint32_t verb_new(uint32_t sender, uint32_t recipient, uint32_t verb)
{
        focus(sender);      

        if (focused->verb.send != 0) return; // only one verb at a time

        update_mask(sender);

        fifo_push(&focused->verb.to, recipient);
        fifo_push(&focused->verb.give, verb);

        return (focused->verb.send |= (focused->verb.bump));
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                      transmit a verb to its recipient                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
uint32_t verb_send(uint32_t sender)
{
        uint32_t recipient;
        uint32_t verb;
        uint32_t sendvalue;

        focus(sender);

        recipient = fifo_pop(&focused->verb.to);
        verb      = fifo_pop(&focused->verb.give);
        sendvalue = (focused->verb.send ^ 0x00000001);

        focus(recipient);

        fifo_push(&focused->verb.from, sender);
        fifo_push(&focused->verb.get, verb);
        focused->verb.rec |= 0x00000001;

        return (sendvalue);
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                              execute a verb                                //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
uint32_t verb_do(uint32_t to)
{
        uint32_t from;        
        uint32_t verb;

        focus(to);

        from = fifo_pop(&focused->verb.from);
        verb = fifo_pop(&focused->verb.get);

        verbs[verb].say(from, to);

        return ((focused->verb.rec<<1) ^ focused->verb.bump);
}
