#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../lib/hash/hash.h"
#include "equipment.h"
#include "equipment_items.h"
#include "edible.h"



void (*item_novum[])(void *self) = {
        &new_rope, 
        &new_torch, 
        &new_pickaxe,
        &new_shovel,
        &new_apple
};


/* STORAGE AND ACCESS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * EQUIPMENT TABLE
 * Whether it goes on the map or a hole in the ground, or in some 
 * creature's inventory, every item is stored in this table.
 */
struct htab_t *equiptable;


/*
 * GET EQUIPMENT
 * The access function, for internal use only.
 */
inline struct equip_t *get_equip(uint32_t key)
{
        return (struct equip_t *)htab_get(equiptable, key);
}




/* INSTANTIATION
``````````````````````````````````````````````````````````````````````````````*/

/* FORWARD DECLARED METHODS */
void method_equipment_put(void *self, int y, int x);
void method_equipment_take(void *self, struct noun_t *noun);
/*void method_equipment_drop(void *self, struct noun_t *noun);*/


/*
 * CONSTRUCTOR
 * new_equipment -- make a new equipment object given an identifying tag
 */
struct equip_t *new_equipment(enum equipments tag)
{
        struct equip_t *new = malloc(sizeof(struct equip_t));

        if (!equiptable)
                equiptable = new_htab(0);

        new->id   = mt_random();
        new->tag  = tag;
        new->new  = item_novum[tag];
        new->put  = &method_equipment_put;
        new->take = &method_equipment_take;

        new->new(new);

        htab_add(equiptable, new->id, new);

        return (new);
}




/* METHODS
``````````````````````````````````````````````````````````````````````````````*/
/* PUT ON MAP */
inline void place_equipment_label(struct equip_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, eq->id); 
}
/* TAKE OFF MAP */
inline void clean_equipment_label(struct equip_t *eq)
{
        mx_set(ACTIVE->item, eq->y, eq->x, 0U); 
}
/* DRAW ITEM */
inline void place_equipment_tile(struct equip_t *eq)
{
        mvwp(PLATE(ACTIVE, BGR), eq->y, eq->x, eq->icon, eq->pair, 0);
}


/* METHOD PUT
 * method_equipment_put -- places a piece of equipment at a location on the map
 */
void method_equipment_put(void *self, int y, int x)
{
        EQUIPMENT_OBJECT(equip, self);
        equip->y = y;
        equip->x = x;
        place_equipment_label(equip);
        place_equipment_tile(equip);
}


/* METHOD TAKE
 * method_equipment_take -- move an item from the map to a noun's inventory
 */
void method_equipment_take(void *self, struct noun_t *noun)
{
        EQUIPMENT_OBJECT(equip, self);
        clean_equipment_label(equip);
        list_add(&noun->inv, &equip->node);
}

/* METHOD DROP
 * method_equipment_drop -- move an item from a noun's inventory to the map
 */

