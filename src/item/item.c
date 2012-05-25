#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../lib/hash/hash.h"
#include "../map/tile.h"
#include "../gfx/ui/notify.h"
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
/* FORWARD DECLARED METHODS */
void method_item_put(void *self, int y, int x);
void method_item_equip(void *self, bool yn);
void DUMMY_METHOD(void *self, struct noun_t *noun);


/**
 * CONSTRUCTOR
 * new_item -- make a new item object given an identifying tag
 */
struct item_t *make_item(enum items tag)
{
        struct item_t *new = malloc(sizeof(struct item_t));

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



/* METHOD HELPERS
``````````````````````````````````````````````````````````````````````````````*/
/* ADD LABEL */ 
inline void place_item_label(struct item_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, eq->id); 
}

/* REMOVE LABEL */
inline void clean_item_label(struct item_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, 0); 
        erase_tile(ACTIVE, eq->y, eq->x);
}

/* ADD TILE */ 
inline void place_item_tile(struct item_t *eq)
{
        if (eq->transparent)
                init_pair(eq->pair, eq->fg, bgcolor_yx(PLATE(ACTIVE, BGR), eq->y, eq->x));

        mvwp(PLATE(ACTIVE, BGR), eq->y, eq->x, eq->icon, eq->pair, 0);
}

/* ADD TO NOUN */
inline void add_to_noun(struct noun_t *noun, struct item_t *item)
{
        assert(noun || !"Noun is NULL");
        assert(item || !"Item is NULL");

        clean_item_label(item);
        list_add(&noun->inv, &item->node);
        /*inventory_mkmenu(&noun->inv);*/
}



/* METHODS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * DUMMY METHOD
 * method_item_dummy -- for inheritance
 */
void DUMMY_METHOD(void *self, struct noun_t *noun) 
{
        return;
}

/**
 * METHOD EQUIP 
 * method_item_equip -- make an item "equipped" or active
 */
void method_item_equip(void *self, bool yn)
{
        ITEM_OBJECT(item, self);
        item->equipped = yn;
}

/** 
 * METHOD PUT
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
 * METHOD TAKE
 * method_item_take -- move an item from the map to a noun's inventory
 */
void method_noun_take(void *self, int y, int x)
{
        struct noun_t *noun = (struct noun_t *)self;
        struct item_t *item;

        item = yx_item(ACTIVE, y, x);

        if (!item)
                return;

        add_to_noun(noun, item);
        inventory_to_menu(&noun->inv);
        alert(I_FIND, item->name);
}

/** 
 * METHOD DROP
 * method_item_drop -- move an item from a noun's inventory to the map
 */
void noun_item_drop(struct noun_t *noun)
{
        struct seed_t seed;
        int n;
        int x;
        int y;
        int i;

        x = pos_x(noun->pos);
        y = pos_y(noun->pos);
        n = roll1d(4);

        /*mx_fill(ACTIVE->item, pos_y(noun->pos), pos_x(noun->pos), &seed);*/

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

        
