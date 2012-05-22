#include <stdlib.h>
#include "../com/arawak.h"
#include "item.h"


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
        ITEM_OBJECT(item, self);
        struct edible_t *edible;

        edible = (struct edible_t *)item->data;

        noun->vitals |= edible->attr;
}
        


/* SUBCLASS CONSTRUCTORS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * APPLE 
 */
void new_apple(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Apple";
        item->desc = "Repels physicians";
        item->icon = L"ⴰ";
        item->transparent = true;
        item->pair = FLEX_APPLE;
        item->fg   = PINK;
        item->data = new_edible(0x00000001);
        item->use  = &method_use_edible;
}

/*
 * GRAPEFRUIT
 */
void new_grapefruit(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Grapefruit";
        item->desc = "Makes you lose weight";
        item->icon = L"ⵙ";
        item->transparent = true;
        item->pair = FLEX_GRAPEFRUIT;
        item->fg   = _ORANGE;
        item->data = new_edible(0x00000002);
        item->use  = &method_use_edible;
}


void new_lg_potion(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Lg. Potion";
        item->desc = "Big gulp";
        item->icon = L"Ⴃ";
        item->transparent = true;
        item->pair = FLEX_LG_POTION;
        item->fg   = BLACK;
        item->data = new_edible(0x00000002);
        item->use  = &method_use_edible;
}

void new_sm_potion(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Sm. Potion";
        item->desc = "Leetle gulp";
        item->icon = L"ⴃ";
        item->transparent = true;
        item->pair = FLEX_SM_POTION;
        item->fg   = BLACK;
        item->data = new_edible(0x00000002);
        item->use  = &method_use_edible;
}

void new_elixir(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Elixir";
        item->desc = "Lick them";
        item->icon = L"⚷";
        item->transparent = true;
        item->pair = FLEX_ELIXIR;
        item->fg   = BLACK;
        item->data = new_edible(0x00000002);
        item->use  = &method_use_edible;
}


void new_palm(void *self)
{
        ITEM_OBJECT(item, self);

        item->name = "Palm";
        item->desc = "Fishy bonez";
        item->icon = L"⸙";
        item->transparent = true;
        item->pair = FLEX_PALM;
        item->fg   = BLACK;
        item->data = new_edible(0x00000002);
        item->use  = &method_use_edible;
}


