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


void do_verb(uint32_t skey, int id)
{
        if (id > 7) return;
        else {
                focus(skey);
                verbs[id](skey, focused->target);
        }
}



void inject_verb(uint32_t skey, uint32_t okey, int verbid)
{
        #include "../lib/magic.h"
        static const uint32_t rbump = 0x00000008;
        static const uint32_t lbump = 0xC0000000;

        static const uint32_t verbmasks[]={ 0x80000000, 0xC0000000 };

        int xmin = vit_blocklen(skey);

        focus(skey);
        focused->verb = (0x80000000>>xmin);
        focused->target = okey;
}
