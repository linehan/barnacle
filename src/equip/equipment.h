#pragma once
#ifndef __ITEMS_H
#define __ITEMS_H

#include "../noun/noun.h"


struct noun_t;




enum equipments {
        ITEM_ROPE,
        ITEM_TORCH,
        ITEM_PICKAXE,
        ITEM_SHOVEL,
        ITEM_APPLE
};


struct equip_t {
        struct list_node node;
        enum equipments tag;
        uint32_t id;
        char    *name;
        char    *desc;
        wchar_t *icon;
        short pair;
        int y;
        int x;
        void *data;

        void (*new) (void *self);
        void (*del) (void *self);
        void (*take)(void *self, struct noun_t *noun);
        void (*drop)(void *self, struct noun_t *noun);
        void (*burn)(void *self, struct noun_t *noun);
        void (*use) (void *self, struct noun_t *noun);
        void (*eat) (void *self, struct noun_t *noun);
        void (*put) (void *self, int y, int x);
};

struct equip_t *new_equipment(enum equipments tag);

#define EQUIPMENT_OBJECT(decl, ptr) \
        struct equip_t *decl = (struct equip_t *)ptr

#endif
