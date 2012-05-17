#include <stdlib.h>
#include "../com/arawak.h"
#include "equipment.h"


/* CLASS DATA 
``````````````````````````````````````````````````````````````````````````````*/
/* 
 * TYPE
 * Just an attribute word that can be combined with a noun's vitals
 * to produce different statistics. */
struct edible_t {
        uint32_t attr;
};

/* TYPE CONSTRUCTOR 
 * new_edible -- creates a new edible data type */
inline struct edible_t *new_edible(uint32_t attr)
{
        struct edible_t *new = calloc(1, sizeof(struct edible_t));
        new->attr = attr;

        return (new);
}


/* CLASS METHODS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * USE
 */
void method_use_edible(void *self, struct noun_t *noun)
{
        EQUIPMENT_OBJECT(equip, self);
        struct edible_t *edible;

        edible = (struct edible_t *)equip->data;

        noun->vitals |= edible->attr;
}
        


/* SUBCLASS CONSTRUCTORS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * APPLE 
 */
void new_apple(void *self)
{
        EQUIPMENT_OBJECT(equip, self);

        equip->name = "Apple";
        equip->desc = "Repels physicians";
        equip->icon = L"ⴰ";
        equip->data = new_edible(0x00000001);
        equip->use  = &method_use_edible;
}



