/* 
 * models.c -- Different routines for spawning (creating+placing) noun mobs.
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
#include "../lib/textutils.h"
#include "noun.h"
#include "models.h"
#include "models/human.h"
#include "models/hopper.h"
#include "models/omega.h"

/**
 * spawn -- spawns the desired model of noun at given radius from the player.
 * The exact coordinates are chosen randomly. 
 * @model: the model of noun to be spawned
 */
void spawn(int model)
{
        #define spawn_radius 10 /* Radius centered at PLAYER (y,x) */

        int spawn_x;
        int spawn_y;
        int dx;
        int dy;

        dx = roll_fair(spawn_radius) * flip_sign(0.5);
        dy = roll_fair(spawn_radius) * flip_sign(0.5);

        spawn_x = pos_x(PLAYER->pos) + dx;
        spawn_y = pos_y(PLAYER->pos) + dy;
        
        spawn_noun(model, spawn_y, spawn_x, true);
}

/**
 * nspawn -- spawn n nouns of the desired model 
 * @n    : number of nouns to spawn
 * @model: model of spawned nouns
 */
void nspawn(int n, int model)
{
        while (n-->0)
                spawn(model);
}


