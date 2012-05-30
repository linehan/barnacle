#pragma once
#ifndef __ITEMS_H
#define __ITEMS_H

#include "../noun/noun.h"


struct noun_t;


#define ITEM_OBJECT(d,p) struct item_t *d = (struct item_t *)p


enum items {
        ITEM_ROPE,
        ITEM_TORCH,
        ITEM_PICKAXE,
        ITEM_SHOVEL,
        ITEM_APPLE,
        ITEM_GRAPEFRUIT,
        ITEM_LG_POTION,
        ITEM_SM_POTION,
        ITEM_ELIXIR,
        ITEM_PALM,
};



struct item_t {
        /*--------------------- Identity */
        uint32_t id;
        enum items tag;
        struct list_node node;

        /*--------------------- Rendering */
        char *name;
        char *desc;
        wchar_t *icon;
        short pair;
        short fg;

        /*--------------------- State */
        bool transparent;
        bool equipped;
        int y;
        int x;
        void *data;

        /*--------------------- Methods */
        void (*new) (void *self);
        void (*del) (void *self);
        void (*equip)(void *self, bool yn);
        void (*put) (void *self, int y, int x);
        void (*use) (void *self, struct noun_t *noun);
        void (*burn)(void *self, struct noun_t *noun);
        void (*eat) (void *self, struct noun_t *noun);

};


struct item_t *make_item(enum items tag);
struct item_t *key_item(uint32_t key);
struct item_t *yx_item(struct map_t *map, int y, int x);

void noun_item_drop(struct noun_t *noun);

void inventory_add(struct list_head *inv, struct item_t *item);
void inventory_del(struct list_head *inv, struct item_t *item);


#endif
