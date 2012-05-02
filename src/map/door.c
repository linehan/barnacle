#include "../com/arawak.h"
#include "../test/test.h"
#include "../lib/stoc/mersenne.h"
#include "../lib/hash.h"
#include "../mob/mob.h"
#include "map.h"
#include "door.h"
#include "tile.h"


struct hashtable_t *doortbl;

/*
 * Make sure everything is set up properly
 */
inline void init_doortbl(void)
{
        doortbl = new_hashtable(0); 
}


/*
 * door_trigger -- make the destination map of a door into the active map
 */
void door_trigger(struct mob_t *mob, uint32_t key)
{
        struct door_t *door = get_door(key);
        assert(door && door->dst && "Door does not exist\n");

        /* Mob is at src map */
        if (mob->mapid == door->src->id) {
                map_set_extra(door->dst);
                mob_place(mob, door->dstrec.y, door->dstrec.x);
        /* Mob is at dst map */
        } else {
                map_set_extra(door->src);
                mob_place(mob, door->srcrec.y, door->srcrec.x);
        }

        ACTIVE = EXTRA;
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

        new->srcrec.w = src_w;
        new->srcrec.h = src_h;
        new->srcrec.y = src_y;
        new->srcrec.x = src_x;

        new->dstrec.w = dst_w;
        new->dstrec.h = dst_h;
        new->dstrec.y = dst_y;
        new->dstrec.x = dst_x;

        for (i=src_y; i<src_y+src_h; i++) {
        for (j=src_x; j<src_x+src_w; j++) {
                SET_TILE(src, i, j, DOR);
                mx_set(src->door, i, j, new->id);
        }
        }
        for (i=dst_y; i<dst_y+dst_h; i++) {
        for (j=dst_x; j<dst_x+dst_w; j++) {
                SET_TILE(dst, i, j, DOR);
                mx_set(dst->door, i, j, new->id);
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


void place_door_tile(struct map_t *map, int y, int x)
{
        struct door_t *door;
        uint32_t key;
        int i;
        int j;

        key  = mx_val(map->door, y, x);
        door = get_door(key);

        assert(door && "Door does not exist\n");

        switch (door->tag) {
        case CAVE_DOOR:
                place_cavedoor_tile(map, y, x);
                break;
        }
} 


