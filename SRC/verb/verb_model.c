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
#include "../noun/noun_model.h"
#include "verb_model.h"




void hp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, HP);
}
void hp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, HP);
}


void sp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, SP);
}
void sp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, SP);
}


void lp_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, LP);
}
void lp_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, LP);
}


void ep_inc(uint32_t skey, uint32_t okey)
{
        inc_vital(okey, EP);
}
void ep_dec(uint32_t skey, uint32_t okey)
{
        dec_vital(okey, EP);
}




// A verb is simply a pointer to a function accepting two nouns as arguments.
void (*verbs[8]) (uint32_t skey, uint32_t okey) = {hp_inc, hp_dec, 
                                                   sp_inc, sp_dec,
                                                   lp_inc, lp_dec,
                                                   ep_inc, ep_dec };


void do_verb(uint32_t skey, uint32_t okey, int id)
{
        if (id > 7) return;
        else
                verbs[id](skey, okey);
}

