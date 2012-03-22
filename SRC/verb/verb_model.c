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
#include "../gfx/dock.h"
#include "../lib/fifo.h"
#include "../lib/magic.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "verb_model.h"
#include "verbs.h"
////////////////////////////////////////////////////////////////////////////////

/*struct verbpkg {*/
        /*uint32_t to;*/
        /*uint32_t from;*/
        /*struct verb_info *verb;*/
/*};*/

/*static const uint32_t RECEIVE = 0x80000000; */
/*static const uint32_t TRANSMIT = 0x80000000;*/
/*static const uint32_t SHUTTLE = 0x80000000;*/

/*#define COMBINE(a, b) (a) ^= (b); \*/
                      /*(b) ^= (a)*/



////////////////////////////////////////////////////////////////////////////////
uint32_t update_mask(uint32_t key)
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

        if (focused->verb.send != 0U) return; // only one verb at a time

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

        focus(sender);

        recipient = fifo_pop(&focused->verb.to);
        verb      = fifo_pop(&focused->verb.give);
        focused->verb.send ^= 0x00000001;

        focus(recipient);

        fifo_push(&focused->verb.from, sender);
        fifo_push(&focused->verb.get, verb);
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

        focus(to);

        from = fifo_pop(&focused->verb.from);
        verb = fifo_pop(&focused->verb.get);

        verbs[verb].say(from, to);

        /*return ((focused->verb.rec<<1) ^ focused->verb.bump);*/
        /*return ((focused->verb.rec<<1) & (focused->verb.mask));*/
        /*BITCLEAR(focused->verb.rec, lzc(focused->verb.rec));*/
        return (0U);
}
