/* 
 * edible.c -- "Edible" types of items
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
#include "item.h"
#include "../gfx/ui/menu_inventory.h"
#include "../gfx/ui/notify.h"


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

        HP_ADD(&noun->vitals, HP(edible->attr));
        SP_ADD(&noun->vitals, SP(edible->attr));
        AP_ADD(&noun->vitals, AP(edible->attr));

        say_stats(noun->vitals);

        if (HP(edible->attr))
                alertf(__HULK, L"%s gives HP +%d!", item->name, HP(edible->attr));
        if (SP(edible->attr))
                alertf(__HULK, L"%s gives SP +%d!", item->name, SP(edible->attr));
        if (AP(edible->attr))
                alertf(__HULK, L"%s gives AP +%d!", item->name, AP(edible->attr));

        inventory_menu_control(EAT_ITEM);
}
        


/* SUBCLASS CONSTRUCTORS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * APPLE 
 */
void new_apple(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t apple = 0UL;
        HP_SET(&apple, 2);

        item->name = "Apple";
        item->desc = "Repels physicians";
        item->icon = L"ⴰ";
        item->transparent = true;
        item->pair = FLEX_APPLE;
        item->fg   = PINK;
        item->data = new_edible(apple);
        item->use  = &method_use_edible;
}

/*
 * GRAPEFRUIT
 */
void new_grapefruit(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t grapefruit = 0UL;
        AP_SET(&grapefruit, 2);

        item->name = "Grapefruit";
        item->desc = "Makes you lose weight";
        item->icon = L"ⵙ";
        item->transparent = true;
        item->pair = FLEX_GRAPEFRUIT;
        item->fg   = _ORANGE;
        item->data = new_edible(grapefruit);
        item->use  = &method_use_edible;
}


void new_lg_potion(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t lg_potion = 0UL;
        HP_SET(&lg_potion, 10);

        item->name = "Lg. Potion";
        item->desc = "Big gulp";
        item->icon = L"Ⴃ";
        item->transparent = true;
        item->pair = FLEX_LG_POTION;
        item->fg   = BLACK;
        item->data = new_edible(lg_potion);
        item->use  = &method_use_edible;
}

void new_sm_potion(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t sm_potion = 0UL;
        HP_SET(&sm_potion, 5);

        item->name = "Sm. Potion";
        item->desc = "Leetle gulp";
        item->icon = L"ⴃ";
        item->transparent = true;
        item->pair = FLEX_SM_POTION;
        item->fg   = BLACK;
        item->data = new_edible(sm_potion);
        item->use  = &method_use_edible;
}

void new_elixir(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t elixir = 0UL;
        SP_SET(&elixir, 5);

        item->name = "Elixir";
        item->desc = "Elixir schön sauber, she says";
        item->icon = L"⚷";
        item->transparent = true;
        item->pair = FLEX_ELIXIR;
        item->fg   = BLACK;
        item->data = new_edible(elixir);
        item->use  = &method_use_edible;
}


void new_palm(void *self)
{
        ITEM_OBJECT(item, self);

        uint32_t palm = 0UL;
        HP_SET(&palm, 1);
        AP_SET(&palm, 3);
        SP_SET(&palm, 3);

        item->name = "Palm";
        item->desc = "For fanning royalty";
        item->icon = L"⸙";
        item->transparent = true;
        item->pair = FLEX_PALM;
        item->fg   = BLACK;
        item->data = new_edible(palm);
        item->use  = &method_use_edible;
}



