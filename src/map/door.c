#include "../com/arawak.h"
#include "map.h"
#include "door.h"
#include "../lib/hash.h"
#include "../lib/stoc/mersenne.h"
#include "tile.h"
#include "../test/test.h"

struct hashtable_t *doortbl;

inline void init_doortbl(void)
{
        doortbl = new_hashtable(0); 
}

void put_door(int tag, struct map_t *src, struct map_t *dest, int h, int w, int y, int x)
{
        struct door_t *new;
        int i;
        int j;

        new = calloc(1, sizeof(struct door_t));

        new->tag  = tag;
        new->src  = src;
        new->dest = dest;
        new->id   = mt_random();

        new->rec.w = w;
        new->rec.h = h;
        new->rec.y = y;
        new->rec.x = x;

        wprintw(CONSOLE_WIN, "door->id: %u\n", new->id);
        wprintw(CONSOLE_WIN, "door->rec.y: %u\n", new->rec.y);
        wprintw(CONSOLE_WIN, "door->rec.x: %u\n", new->rec.x);
        wprintw(CONSOLE_WIN, "door->rec.h: %u\n", new->rec.h);
        wprintw(CONSOLE_WIN, "door->rec.w: %u\n", new->rec.w);

        for (i=y; i<y+h; i++) {
        for (j=x; j<x+w; j++) {
                mx_set(src->mx, i, j, DOR);
                mx_set(src->door, i, j, new->id);
        }
        }

        if (!doortbl)
                init_doortbl();

        hashtable_add(doortbl, new->id, new);
}

struct door_t *get_door(uint32_t key)
{
        return (struct door_t *)hashtable_get(doortbl, key);
}

void door_trigger(void (*trigger)(void *self), struct door_t *door)
{
        door->trigger = trigger;
}


void place_door_tile(struct map_t *map, int y, int x)
{
        struct door_t *door;
        uint32_t key;
        int i;
        int j;

        key  = mx_val(map->door, y, x);
        door = get_door(key);

        assert(door || !"Door does not exist\n");

        switch (door->tag) {
        case CAVE_DOOR:
                place_cavedoor_tile(map, y, x);
                break;
        }
} 


