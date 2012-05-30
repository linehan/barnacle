/* 
 * item.c -- Item logic and structures 
 * Some of the functions here have to use a slightly different naming 
 * convention to avoid crossing paths with the curses menu library, 
 * which provides many functions named item_* or *_item, and also defines 
 * an ITEM typedef. Initially, all of these things were called "equipment",
 * naturally shortened to "equip" (this file was "equip.c"). Unfortunately,
 * "equip" is also a verb, one that would naturally be needed in some of 
 * these routines. Confusion reigned... now it's "item.c".
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "../com/barnacle.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../lib/hash/hash.h"
#include "../map/tile.h"
#include "../gfx/ui/notify.h"
#include "../gfx/ui/menu_inventory.h"
#include "item.h"
#include "tools.h"
#include "edible.h"

void (*item_novum[])(void *self) = {
        &new_rope, 
        &new_torch, 
        &new_pickaxe,
        &new_shovel,
        &new_apple,
        &new_grapefruit,
        &new_lg_potion,
        &new_sm_potion,
        &new_elixir,
        &new_palm,
};



/* STORAGE AND ACCESS
``````````````````````````````````````````````````````````````````````````````*/
/* ITEM TABLE */
struct htab_t *itemtable;


/**
 * BY KEY 
 * key_item -- access function, for internal use only.
 * @key: hash key
 */
struct item_t *key_item(uint32_t key)
{
        struct item_t *item;
        item = (struct item_t *)htab_get(itemtable, key);

        return (item);
}

/**
 * BY COORDINATES (Y,X)
 * yx_item -- given coordinates y, x, return any item at that position
 * @y: y-coordinate
 * @x: x-coordinate
 */
struct item_t *yx_item(struct map_t *map, int y, int x)
{
        return (key_item(mx_val(map->item, y, x)));  
}


/* INSTANTIATION
``````````````````````````````````````````````````````````````````````````````*/
/* Forward declared methods */
void method_item_put(void *self, int y, int x);
void method_item_equip(void *self, bool yn);
void DUMMY_METHOD(void *self, struct noun_t *noun);


/**
 * new_item -- make a new item object given an identifying tag
 * @tag: type of item
 */
struct item_t *make_item(enum items tag)
{
        struct item_t *new = calloc(1, sizeof(struct item_t));

        if (!itemtable)
                itemtable = new_htab(0);

        new->id    = mt_random();
        new->tag   = tag;
        new->new   = item_novum[tag];
        new->put   = &method_item_put;
        new->use   = &DUMMY_METHOD;
        new->equip = &method_item_equip;

        new->new(new);

        htab_add(itemtable, new->id, new);

        return (new);
}



/* ITEM DRAWING (ON THE MAP) 
``````````````````````````````````````````````````````````````````````````````*/
/* Add tile label on the map */ 
inline void place_item_label(struct item_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, eq->id); 
}

/* Remove tile label on the map */
inline void clean_item_label(struct item_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, 0); 
        erase_tile(ACTIVE, eq->y, eq->x);
}

/* Add the graphical tile to the map */ 
inline void place_item_tile(struct item_t *eq)
{
        if (eq->transparent)
                init_pair(eq->pair, eq->fg, bgcolor_yx(PLATE(ACTIVE, BGR), eq->y, eq->x));

        mvwcch(PLATE(ACTIVE, BGR), eq->y, eq->x, eq->icon, 0, eq->pair);
}



/* ITEM-NOUN INTERACTION
``````````````````````````````````````````````````````````````````````````````*/
/* Add an item to a noun's inventory */
void inventory_add(struct list_head *inv, struct item_t *item)
{
        assert(inv  || !"Inventory is NULL");
        assert(item || !"Item is NULL");

        clean_item_label(item);
        list_add(inv, &item->node);
}

/* Add an item to a noun's inventory */
void inventory_del(struct list_head *inv, struct item_t *item)
{
        assert(inv  || !"Inventory is NULL");
        assert(item || !"Item is NULL");

        list_del_from(inv, &item->node);
}


/* METHODS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * method_item_dummy -- for inheritance
 */
void DUMMY_METHOD(void *self, struct noun_t *noun) 
{
        return;
}

/**
 * method_item_equip -- make an item "equipped" or active
 */
void method_item_equip(void *self, bool yn)
{
        ITEM_OBJECT(item, self);
        item->equipped = yn;
}

/** 
 * method_item_put -- places a piece of item at a location on the map
 */
void method_item_put(void *self, int y, int x)
{
        ITEM_OBJECT(item, self);
        item->y = y;
        item->x = x;
        place_item_label(item);
        place_item_tile(item);
}








        

/** 
 * METHOD DROP
 * method_item_drop -- move an item from a noun's inventory to the map
 */
void noun_item_drop(struct noun_t *noun)
{
        int n;
        int x;
        int y;
        int i;

        x = pos_x(noun->pos);
        y = pos_y(noun->pos);
        n = roll1d(4);

        for (i=0; i<n; i++) {
                struct item_t *item = make_item(4+roll1d(4));

                switch (i) { 
                case 0:
                        item->put(item, y+1, x-1);
                        break;
                case 1:
                        item->put(item, y-1, x+1);
                        break;
                case 2:
                        item->put(item, y, x+1);
                        break;
                }
        }
}

        
