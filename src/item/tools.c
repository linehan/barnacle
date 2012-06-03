/* 
 * tools.c -- Tool-type items 
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
#include "../map/cell.h"
#include "../map/terrain.h"
#include "../lib/stoc/stoc.h"
#include "../noun/models.h"
#include "item.h"
#include "tools.h"
#include "../map/light.h"

/* Ropes
``````````````````````````````````````````````````````````````````````````````*/
struct rope_t {
        int len;
        struct cell_t *head;
        struct cell_t *tail;
};

/* NEW */
void new_rope(void *self)
{
        #define ROPE_S_ICON L"∮"
        #define ROPE_M_ICON L"∯"
        #define ROPE_L_ICON L"∰"
        #define ROPE_L_LEN 20

        ITEM_OBJECT(item, self);

        struct rope_t *new;

        new = malloc(sizeof(struct rope_t));

        new->len  = ROPE_L_LEN;
        new->head = NULL;
        new->tail = NULL;

        item->data = new;
        item->tag  = ITEM_ROPE;
        item->name = "Rope";
        item->desc = "For hanging things by the neck until dead";
        item->icon = ROPE_L_ICON;
        item->use  = &use_rope;
}


/* USE */
void use_rope(void *self, struct noun_t *noun)
{
        struct item_t *item;
        struct rope_t *rope;
        int tile;
        int i;
        int y;
        int next;
        int x;

        if (pos_hdg(noun->pos) != WEST 
        &&  pos_hdg(noun->pos) != EAST)
                return;

        item = (struct item_t *)self;
        rope  = (struct rope_t *)item->data;
        y     = pos_y(noun->pos);
        x     = pos_x(noun->pos);

        /* Decide which way the rope will dangle */
        if ((TILE(ACTIVE, min(y+1, LINES), max(x-1, 0)) == CAVEFLOOR)
        || (pos_hdg(noun->pos) == WEST)) {

                tile = L_ROPE_ANCHOR;
                dec(x, 0);
        }
        else if ((TILE(ACTIVE, min(y+1, LINES), min(x+1, COLS-1)) == CAVEFLOOR)
        ||      (pos_hdg(noun->pos) == EAST)) {
                tile = R_ROPE_ANCHOR;
                inc(x, COLS);
        }

        /* Don't draw anything if the tile isn't the floor */
        if (TILE(ACTIVE, y, x) != CAVEFLOOR)
                return;

        rope->head = new_cell(y, x);
        rope->tail = rope->head; /* Whoa */

        SET_TILE(ACTIVE, y, x, tile);

        /* Unwind the rope */
        for (i=0; i<rope->len; i++) {

                struct cell_t *new;

                inc(y, LINES);
                /* Look ahead */
                if (TILE(ACTIVE, clamp(y+1, 0, LINES), x) == CAVESOLID)
                       break; 
                
                new         = new_cell(y, x);
                new->parent = rope->tail;
                rope->tail  = new;

                SET_TILE(ACTIVE, y, x, ROPE);
        }
        MAPBOOK->render(ACTIVE);
}


/* Pickaxe 
``````````````````````````````````````````````````````````````````````````````*/

void new_pickaxe(void *self)
{
        #define PICKAXE_WCH L"ང"

        ITEM_OBJECT(item, self);

        item->data = NULL;
        item->tag  = ITEM_PICKAXE;
        item->name = "Pickaxe";
        item->desc = "For prospectin'";
        item->icon = PICKAXE_WCH;
        item->use  = &use_pickaxe;
}

void use_pickaxe(void *self, struct noun_t *noun)
{
        int tile;
        int y;
        int x;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        switch (noun->pos->hdg) {
        case NORTH:
                dec(y, 0);
                noun_set_signal(noun, SM_DigUp, '*');
                break;
        case SOUTH:
                inc(y, LINES);
                noun_set_signal(noun, SM_DigDown, '*');
                break;
        case EAST:
                inc(x, COLS);
                noun_set_signal(noun, SM_DigRight, '*');
                break;
        case WEST:
                dec(x, 0);
                noun_set_signal(noun, SM_DigLeft, '*');
                break;
        }

        if (TILE(ACTIVE, y, x) == CAVESOLID) {
                SET_TILE(ACTIVE, y, x, CAVERUBBLE);
        }

        MAPBOOK->render(ACTIVE);
}        

/* Shovel 
``````````````````````````````````````````````````````````````````````````````*/

void new_shovel(void *self)
{
        #define SHOVEL_WCH L"⏚"

        ITEM_OBJECT(item, self);

        item->data = NULL;
        item->tag  = ITEM_SHOVEL;
        item->name = "Shovel";
        item->desc = "Gilded shitscoop";
        item->icon = SHOVEL_WCH;
        item->use  = &use_shovel;
}

void use_shovel(void *self, struct noun_t *noun)
{
        int tile_d;
        int y;
        int x;

        y = pos_y(noun->pos); 
        x = pos_x(noun->pos);

        tile_d = TILE(ACTIVE, (y+1), x);

        if (tile_d == CAVESOLID) {
                SET_TILE(ACTIVE, (y+1), x, CAVERUBBLE);
                noun_set_signal(noun, SM_DigDown, '*');
        }

        MAPBOOK->render(ACTIVE);
}        

/* Torch 
``````````````````````````````````````````````````````````````````````````````*/

struct torch_t {
        bool lit;
        struct light_t *light;
};


void equip_torch(void *self, bool yn);
void burn_torch(void *self, struct noun_t *noun);


void new_torch(void *self)
{
        #define TORCH_H 5
        #define TORCH_W 7
        #define TORCH_Hr 2
        #define TORCH_Wr 3
        #define TORCH_WCH L"༈"

        ITEM_OBJECT(item, self);
        struct torch_t *new;
        int i;

        new = calloc(1, sizeof(struct torch_t));

        new->light = new_light(TORCH_H, TORCH_W, LINES/2, COLS/2, TORCH_BASE);

        item->data  = new;
        item->tag   = ITEM_TORCH;
        item->icon  = TORCH_WCH;
        item->name  = "Torch";
        item->desc  = "The oily smoke makes your eyes sting";
        item->burn  = &burn_torch;
        item->equip = &equip_torch;
}


void equip_torch(void *self, bool yn)
{
        ITEM_OBJECT(item, self);
        struct torch_t *torch = (struct torch_t *)item->data;

        item->equipped = yn;

        if (item->equipped) {
                show_panel(torch->light->pan);
                torch->light->lit = true;
        } else {
                hide_panel(torch->light->pan);
                torch->light->lit = false;
        }
}


void burn_torch(void *self, struct noun_t *noun)
{
        ITEM_OBJECT(item, self);
        struct torch_t *torch = item->data;

        glow_light(torch->light, noun, true);
}




