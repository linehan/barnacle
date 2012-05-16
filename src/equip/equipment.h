#pragma once
#ifndef __ITEMS_H
#define __ITEMS_H

#include "../noun/noun.h"

struct noun_t;

enum equipments {
        ITEM_ROPE,
        ITEM_TORCH,
        ITEM_PICKAXE,
        ITEM_SHOVEL
};

struct equip_t {
        uint32_t id;
        enum equipments tag;
        char *name;
        char *desc;
        wchar_t *wch;
        short pair;
        void *data;
        void (*new)(void *self);
        void (*use)(void *self, struct noun_t *noun);
        void (*burn)(void *self, struct noun_t *noun);
};

struct equip_t *new_equipment(int tag);

#endif
