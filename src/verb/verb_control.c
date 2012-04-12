/*//////////////////////////////////////////////////////////////////////////////
 
                      _..--.._       _..--.      _..--..
                    ,'      ,'`.   ,','.--.\   ,' \   `.`.
                   /  /    /  /|  : : /  _ \:  |\  \    \ \
                  /  :    :  /`.  | |:<o  \o;  ,`\  \    : \
                 /   '__ '| :,.-' ;'\:  _`'/`.`--,:  : __'  \
                :   ,'_ ``' |._) /  :\`..__)  \  >| ,'' _``. \
                | ,;,   .  `:\ _:   | `,/_\.   :`/;'  ,   .:\ )
                `'/'   _ \   \:\(  _|__`>_/`' /(:/   /     .\` 
                 /:  .'  ,`.._|_\\'  ( _=`;._//_|_..'`      \\
                 :: /   '|    (__=`, :`||| `,.__)     \      :
                 | \           \`.\\__\;|| //`|/       :     |
                 |  `.._____.-,`'| \\___||// /`-._           |
                 :         | : ,<''_\\,.|//_`>.  :`._       ;:
                 \         ; ; )`-..______..-'(  :\  `-.__.' /
                 ;        / /|:                : | `.._____.':
                :      _.' / ||                | |   `.       :
                :  _.-'   /  ::                ; :     `-.____;
                 \;     ,'   ( \              /  )\          /
                ,'    ,'____,' ,`-,.______..-') (__\        _`.
               (___..'>_>____`.`.'._)_\_>._)-' ,'___`._________) SSt


                                 verb_control.c


///////////////////////////////////////////////////////////////////////////// */
#include <stdio.h>
#include <stdlib.h>
#include "../lib/fifo.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/dock.h"
#include "../noun/noun_model.h"
#include "../noun/noun_view.h"
#include "../com/arawak.h"
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

        key_noun(key);
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

        if (send==0U) verb_send(key);
        else focused->verb.send = send;
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

        key_noun(key);

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
        key_noun(key);
        update_mask(key);

        if ((focused->verb.forw == true)) 
                focused->verb.fund >>= 1;
        else    focused->verb.fund <<= 1;

        if ((focused->verb.fund == 0x00000001))
                        focused->verb.forw=false;
        if (lzc(focused->verb.fund) <= lzc(focused->verb.bump)) 
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
        key_noun(key);
        focused->verb.canc  = focused->verb.send;
        focused->verb.send ^= focused->verb.send;
}
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//                         cancel the verb being sent                         //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
int choose_verb(int input)
{
        static struct verb_info *verb;
        MENU *menu;
        PANEL *pan;
        ITEM *item;
        int h;

        if (input == MODE_STARTED) init_verb_menu(0x000000FF);

        menu = get_verb_menu();
        pan  = get_verb_panel();

        switch (input) {
        case MODE_STARTED:
                top_panel(pan);
                break;
        case 'k':
                menu_driver(menu, REQ_PREV_ITEM);
                break;
        case 'j':
                menu_driver(menu, REQ_NEXT_ITEM);
                break;
        case 'n':
                menu_driver(menu, REQ_PREV_MATCH);
                break;
        case 'p':
                menu_driver(menu, REQ_NEXT_MATCH);
                break;
        case ' ':
                verb_new(request_id(SUBJECT), 
                         request_id(OBJECT), 
                         (uint32_t)verb->id);
                break;
        case 'x':
                verb_cancel(request_id(SUBJECT));
                break;
        case 'v':
                hide_panel(pan);
                return MODE_RESTORE;
        }
        item = current_item(menu);
        verb = (struct verb_info *)item_userptr(item);
        post_verb_icon(item);

        return MODE_PERSIST;
}
