#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/stoc/stoc.h"
#include "equipment.h"
#include "equipment_items.h"


void (*item_novum[])(void *self) = {
        &new_rope, 
        &new_torch, 
        &new_pickaxe,
        &new_shovel
};



struct equip_t *new_equipment(int tag)
{
        struct equip_t *new = malloc(sizeof(struct equip_t));

        new->id = mt_random();
        new->tag = tag;
        new->new = item_novum[tag];

        new->new(new);

        return (new);
}

