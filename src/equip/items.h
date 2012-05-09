#pragma once
#ifndef __ITEMS_H
#define __ITEMS_H

#include "../gfx/ui/stdmenu.h"

enum thingies {
        ITEM_ROPE,
        ITEM_TORCH,
        ITEM_PICKAXE
};



struct equip_t {
        wchar_t *wch;
        short pair;
        char *name;
        char *desc;
        void (*new)(void *self);
        void (*use)(struct mob_t *mob, void *self);
        void (*burn)(struct mob_t *mob, void *self);
        uint32_t id;
        enum thingies tag;
        void *data;
};

struct equip_t *new_equipment(int tag);

#endif
