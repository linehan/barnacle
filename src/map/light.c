/* 
 * light.c -- Generalized "light source" objects.
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
#include "../gfx/gfx.h"
#include "../eng/error.h"
#include "map.h"
#include "light.h"


/*
 * Compute the manhattan distance from the center of a rectangle
 */
int radius(int y1, int x1, int y2, int x2)
{
        return (MAX(x1,x2) - MIN(x1,x2)) + (MAX(y1,y2) - MIN(y1,y2));
}



/*
 * Return a new light object
 */
struct light_t *new_light(int h, int w, int y, int x, short base)
{
        struct light_t *new;

        new = calloc(1, sizeof(struct light_t));
        
        /* Dimensions */
        new->h   = h;
        new->w   = w;
        new->yul = y - h/2;
        new->xul = x - w/2;
        new->y   = y; /* Centered instead */
        new->x   = x; /* of top-l corner! */

        /* Colors */
        new->num_colors = radius(0,0, (h/2),(w/2)) + 1;
        new->base = base;

        new->pair = calloc(new->num_colors, sizeof(short));
        new->bg   = calloc(new->num_colors, sizeof(short));
        new->fg   = calloc(new->num_colors, sizeof(short));

        TRY {
                if (new->num_colors != 6)
                        THROW(8);
        } CATCH("There are %d colors", new->num_colors);

        assign_glow_colors(base, new->fg, new->bg, new->pair);

        /* Windows & Panels */
        new->win = newwin(h, w, y, x);
        new->pan = new_panel(new->win);
        hide_panel(new->pan);

        new->lit = false;

        return (new);
}

/*
 * Render the light 
 */
void glow_light(struct light_t *light, struct noun_t *noun, bool move)
{
        #define TOP_EDGE(y,h)    ((y) < ((h)+1))           ? true : false
        #define BOTTOM_EDGE(y,h) ((y) > (LINES - ((h)+1))) ? true : false
        #define LEFT_EDGE(x,w)   ((x) < (w))               ? true : false
        #define RIGHT_EDGE(x,w)  ((x) > (COLS - (w)))      ? true : false
        #define MAX_RADIUS 5
        int light_h, light_w;
        int y, x;
        int i, j;
        int r;

        if (!light->lit)
                return;

        if (move) {
                y = light->y = pos_y(noun->pos);
                x = light->x = pos_x(noun->pos);
                light->yul = light->y - light->h/2;
                light->xul = light->x - light->w/2;
        } else {
                y = light->y;
                x = light->x;
        }

        /* -------------------------------------- y-adjustments */
        light_h = light->h;

        if (TOP_EDGE(y, light->h))
                light_h = (light->h + y);
        
        if (BOTTOM_EDGE(y, light->h))
                light_h = light->h + (LINES-y);

        /* -------------------------------------- x-adjustments */
        light_w = light->w;

        if (LEFT_EDGE(x, light->w)) 
                light_w = (light->w + x);

        if (RIGHT_EDGE(x, light->w)) 
                light_w = light->w+ (COLS-x);

        /* -------------------------------------- adjust window if necessary */
        if (wresize(light->win, light_h, light_w));
                replace_panel(light->pan, light->win);

        move_panel(light->pan, light->yul, light->xul);

        /* -------------------------------------- copy bg to torch buffer */
        copywin(PLATE(ACTIVE, BGR), light->win, 
                light->yul, light->xul, 0, 0, light_h-1, light_w-1, 0);

        overwrite(noun->win, light->win); /* Copy noun mob */

        /* -------------------------------------- re-color the copied wchars */
        for (i=0; i<light_h; i++) {
        for (j=0; j<light_w; j++) {
                r = radius(i, j, light->h/2, light->w/2);
                /*mvwprintw(light->win, i, j, "%d", r);*/
                if (r < MAX_RADIUS)
                        mvwchgat(light->win, i, j, 1, 0, light->pair[r], NULL);
                else if (is_blank(light->win, i, j))
                        mvwchgat(light->win, i, j, 1, 0, PCAVEBLANK, NULL);
        }
        }
        show_panel(light->pan);
}

