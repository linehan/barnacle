#pragma once
#ifndef __INVENTORY_H
#define __INVENTORY_H

#include "../equip/equipment.h"

#define inv_menu(inv) ((inv)->menu)
#define inv_tmp(inv)  ((inv)->tmp)
#define inv_eqwin(inv) ((inv)->equipped_win)

#define inv_add(inv,eq)   (inv)->add((inv), (eq))
#define inv_use(inv,key)  (inv)->use((inv), (key))
#define inv_get(inv,key)  (inv)->get((inv), (key))
#define inv_burn(inv,key) (inv)->burn((inv), (key))
#define inv_mkmenu(inv)   (inv)->mkmenu((inv))

#define SET_CUR_KEY(inv) ((inv)->cur_key = *(uint32_t *)(inv)->menu->cur_ptr)
#define CUR_KEY(inv)  ((inv)->cur_key)
#define TORCHKEY(inv) ((inv)->torch_key)
#define ROPEKEY(inv)  ((inv)->rope_key)


struct inventory_t {
        struct htab_t *tbl;
        struct noun_t *noun;
        int n;

        uint32_t key[100];
        uint32_t cur_key;
        uint32_t torch_key;
        uint32_t rope_key;

        struct equip_t *tmp;
        struct stdmenu_t *menu;

        WINDOW *equipped_win;
        PANEL  *equipped_pan;

        void (*add)(void *self, struct equip_t *equip);
        void (*use)(void *self, uint32_t key);
        void (*get)(void *self, uint32_t key);
        void (*burn)(void *self, uint32_t key);
        void (*mkmenu)(void *self);
        void (*del)(void *self);
};


struct inventory_t *new_inventory(struct noun_t *noun);
void inventory_control(struct inventory_t *inv, int input);

#endif
