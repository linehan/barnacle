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
#include "../gfx/gfx.h"
#include "../gfx/ui/dock.h"
#include "../lib/fifo.h"
#include "../lib/magic.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "verb_model.h"
#include "verbs.h"
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
uint32_t update_mask(uint32_t key)
{
        key_noun(key);
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
        key_noun(sender);      

        if (focused->verb.send != 0U) return 0; // only one verb at a time

        fifo_enq(&focused->verb.to, recipient);
        fifo_enq(&focused->verb.give, verb);

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

        key_noun(sender);

        recipient = fifo_deq(&focused->verb.to);
        verb      = fifo_deq(&focused->verb.give);
        focused->verb.send ^= 0x00000001;

        key_noun(recipient);

        fifo_enq(&focused->verb.from, sender);
        fifo_enq(&focused->verb.get, verb);
        focused->verb.rec |= 0x00000001;

        return (0);
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

        key_noun(to);

        from = fifo_deq(&focused->verb.from);
        verb = fifo_deq(&focused->verb.get);

        verbs[verb].say(from, to);

        /*return ((focused->verb.rec<<1) ^ focused->verb.bump);*/
        /*return ((focused->verb.rec<<1) & (focused->verb.mask));*/
        /*BITCLEAR(focused->verb.rec, lzc(focused->verb.rec));*/
        return (0U);
}