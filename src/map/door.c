/* 
 * door.c -- Transition between map screens.
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
#include "../com/barnacle.h"
#include "../test/test.h"
#include "../lib/stoc/mersenne.h"
#include "../lib/hash/hash.h"
#include "map.h"
#include "door.h"
#include "tile.h"
#include "light.h"

struct htab_t *doortbl;

/*
 * Make sure everything is set up properly
 */
inline void init_doortbl(void)
{
        doortbl = new_htab(0); 
}


/*
 * door_trigger -- make the destination map of a door into the active map
 */
void door_trigger(struct noun_t *noun, uint32_t key)
{
        struct door_t *door = get_door(key);
        assert(door && door->dst && "Door does not exist\n");

        /* Mob is at src map */
        if (noun->map_id == door->src->id) {
                map_set_extra(door->dst);
                noun->setyx(noun, door->dstrec->y+1, door->dstrec->x);
                noun->map_id = door->dst->id;
                top_panel(doorlight->pan);
                doorlight->lit = true;
                glow_light(doorlight, noun, false);

        /* Mob is at dst map */
        } else {
                map_set_extra(door->src);
                noun->setyx(noun, door->srcrec->y+1, door->srcrec->x);
                noun->map_id = door->src->id;
                doorlight->lit = false;
                hide_panel(doorlight->pan);
        }
        top_panel(noun->pan);
}


void put_door(int tag, struct map_t *src, struct map_t *dst, 
              int src_h, int src_w, int src_y, int src_x,
              int dst_h, int dst_w, int dst_y, int dst_x)
{
        struct door_t *new;
        int i;
        int j;

        new = calloc(1, sizeof(struct door_t));

        new->tag = tag;
        new->src = src;
        new->dst = dst;
        new->id  = mt_random();
        doorlight = new_light(5, 7, dst_y, dst_x, SUN_BASE);

        new->srcrec = new_rec(src_h, src_w, src_y, src_x);
        new->dstrec = new_rec(dst_h, dst_w, dst_y, dst_x);

        for (i=src_y; i<src_y+src_h; i++) {
        for (j=src_x; j<src_x+src_w; j++) {
                SET_TILE(src, i, j, CAVEDOOR_OUTSIDE);
                SET_DOOR(src, i, j, new->id);
        }
        }
        for (i=dst_y; i<dst_y+dst_h; i++) {
        for (j=dst_x; j<dst_x+dst_w; j++) {
                SET_TILE(dst, i, j, CAVEDOOR_INSIDE);
                SET_DOOR(dst, i, j, new->id);
        }
        }

        if (!doortbl)
                init_doortbl();

        htab_add(doortbl, new->id, new);
}


struct door_t *get_door(uint32_t key)
{
        return (struct door_t *)htab_get(doortbl, key);
}

/*int radius(int y1, int x1, int y2, int x2)*/
/*{*/
        /*return (abs(x1 - x2) + abs(y1 - y2));*/
/*}*/



/*struct light_t *new_light(int y, int x)*/
/*{*/
        /*#define TORCH_H 5*/
        /*#define TORCH_W 7*/
        /*#define TORCH_Hr 2*/
        /*#define TORCH_Wr 3*/
        /*static const short SUN_pair[3] = {SUNP1, SUNP2, SUNP3};*/
        /*static const short SUN_bg[3]   = {SUNB1, SUNB2, SUNB3};*/
        /*static const short SUN_fg[3]   = {SUN1, SUN2, SUN3};*/
        /*struct light_t *new;*/

        /*int i;*/

        /*new = calloc(1, sizeof(struct light_t));*/

        /*new->h  = TORCH_H;*/
        /*new->w  = TORCH_W;*/
        /*new->hr = TORCH_Hr;*/
        /*new->wr = TORCH_Wr;*/

        /*for (i=0; i<3; i++) {*/
                /*new->pair[i] = SUN_pair[i];*/
                /*new->fg[i]   = SUN_fg[i];*/
                /*new->bg[i]   = SUN_bg[i];*/
        /*}*/

        /*new->win = newwin(new->h, new->w, 0, 0);*/
        /*new->pan = new_panel(new->win);*/
        /*hide_panel(new->pan);*/

        /*return (new);*/
/*}*/


/*void door_light(struct noun_t *noun)*/
/*{*/
        /*int i;*/
        /*int j;*/
        /*int x0;*/
        /*int y0;*/
        /*int light_h;*/
        /*int light_w;*/
        /*int y;*/
        /*int x;*/

        /*if (!lightok)*/
                /*return;*/

        /*y = lighty;*/
        /*x = lightx;*/

        /*[>[> -------------------------------------- y-adjustments <]<]*/
        /*if (y < light->hr+1) {*/
                /*light_h = (light->hr + y);*/
                /*y0 = light->hr - y;*/
        /*}*/
        /*else if (y > LINES-(light->hr+1)) {*/
                /*light_h = light->hr + (LINES-y);*/
                /*y0 = y - light->hr;*/
        /*}*/
        /*else {*/
                /*light_h = light->h;*/
                /*y0 = y - light->hr;*/
        /*}*/
        /*[>[> -------------------------------------- x-adjustments <]<]*/
        /*if (x < light->wr) */
        /*{*/
                /*light_w = (light->wr + x);*/
                /*x0 = light->wr - x;*/
        /*}*/
        /*else if (x > COLS-light->wr) {*/
                /*light_w = light->wr+ (COLS-x);*/
                /*x0 = x - light->wr;*/
        /*}*/
        /*else {*/
                /*light_w = light->w;*/
                /*x0 = x - light->wr;*/
        /*}*/

        /*if (wresize(light->win, light_h, light_w));*/
                /*replace_panel(light->pan, light->win);*/

        /*move_panel(light->pan, y0, x0);*/

        /*[>[> -------------------------------------- copy bg to torch buffer <]<]*/
        /*copywin(PLATE(ACTIVE, BGR), light->win, y0, x0, 0, 0, light_h-1, light_w-1, 0);*/
        /*overwrite(noun->win, light->win);*/

        /*[>[> -------------------------------------- re-color the copied wchars <]<]*/
        /*for (i=0; i<light_h; i++) {*/
        /*for (j=0; j<light_w; j++) {*/
                /*mvwprintw(light->win, i, j, "%d", radius(i, j, light_h/2, light_w/2));*/
                /*[>if ((i==0 && j==0)<]*/
                /*[>||  (i==0 && j==1)<]*/
                /*[>||  (i==0 && j==light_w-1)<]*/
                /*[>||  (i==0 && j==light_w-2)<]*/
                /*[>||  (i==light_h-1 && j==0)<]*/
                /*[>||  (i==light_h-1 && j==1)<]*/
                /*[>||  (i==light_h-1 && j==light_w-1)<]*/
                /*[>||  (i==light_h-1 && j==light_w-2))<]*/
                /*[>{<]*/
                        /*[>mvwchgat(light->win, i, j, 1, 0, light->pair[2], NULL);<]*/
                /*[>}<]*/
                /*[>else if (i==0 || i==light_h-1 || j==0 || j==1 || j==light_w-1 || j==light_w-2)<]*/
                        /*[>mvwchgat(light->win, i, j, 1, 0, light->pair[1], NULL);<]*/
                /*[>else <]*/
                        /*[>mvwchgat(light->win, i, j, 1, 0, light->pair[0], NULL);<]*/
        /*}*/
        /*}*/
        /*show_panel(light->pan);*/
/*}*/


