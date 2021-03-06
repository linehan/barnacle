/* 
 * combat.c -- Noun combat.
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
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../map/map.h"
#include "noun.h"

inline int neighbors_exist(struct seed_t *seed)
{
        uint32_t *neighbor;
        int i;
        int n;

        for (i=0; i<8; i++) {
                neighbor = seed->nbr[i];
                if (neighbor)
                        n++;
        }
        return (n);
}

inline uint32_t random_neighbor(struct seed_t *seed)
{
        uint32_t *neighbor;
        int i;

        for (i=0; i<8; i++) {
                neighbor = seed->nbr[i];
                if (neighbor && *neighbor)
                        break;
        }

        return *neighbor;
}


uint32_t noun_nbr_any(struct noun_t *noun)
{
        struct seed_t seed; 
        mx_seed(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), &seed);

        if (neighbors_exist(&seed))
                return (random_neighbor(&seed));
        else
                return NONOUN;
}



uint32_t noun_nbr(struct noun_t *noun, int dir)
{
        struct seed_t seed; 
        uint32_t *nbr;

        mx_seed(ACTIVE->mobs, pos_y(noun->pos), pos_x(noun->pos), &seed);

        switch (dir) {
        case NORTH:
                nbr = seed.n;
                break;
        case SOUTH:
                nbr = seed.s;
                break;
        case EAST:
                nbr = seed.e;
                break;
        case WEST:
                nbr = seed.w;
                break;
        }
        return (*nbr);
}


uint32_t noun_nbr_model(struct noun_t *noun, int dir, int model)
{
        uint32_t neighbor;

        neighbor = noun_nbr(noun, dir);

        if (neighbor != NONOUN && noun_model(neighbor) == model)
                return neighbor;
        else
                return NONOUN;
}





