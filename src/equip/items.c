#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdmenu.h"
#include "../lib/hash.h"
#include "../lib/stoc/stoc.h"
#include "items.h"
#include "rope.h"


void (*item_novum[])(void *self) = {&new_rope, &new_torch, &new_pickaxe};



struct equip_t *new_equipment(int tag)
{
        struct equip_t *new = malloc(sizeof(struct equip_t));

        new->id = mt_random();
        new->tag = tag;
        new->new = item_novum[tag];

        new->new(new);

        return (new);
}

