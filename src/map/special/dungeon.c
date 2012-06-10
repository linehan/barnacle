#include "../../com/barnacle.h"
#include "../map.h"
#include "../tile.h"
#include "dungeon.h"

#define MIN_ROOMS (4)
#define MAX_ROOMS (10)
#define MIN_ROOM_SEP (3)

#define MAX_ROOM_WIDTH ((COLS/(MIN_ROOMS)) - (2 * MIN_ROOM_SEP))
#define MIN_ROOM_WIDTH (3)

#define MAX_ROOM_HEIGHT ((LINES/(MIN_ROOMS)) - (2 * MIN_ROOM_SEP))
#define MIN_ROOM_HEIGHT (3)


/******************************************************************************
 * Rectangles -- random size and placement
 ******************************************************************************/
/*
 * rec_size_random -- set height and width of a rect to random bounded values
 * @rec  : pointer to a rectangle
 * @h_min: minimum height
 * @h_max: maximum height
 * @w_min: minimum width
 * @w_max: maximum width
 */
void rec_size_random(struct rec_t *rec, int hmin, int hmax, int wmin, int wmax)
{
        int hdif;
        int wdif;

        hdif = hmax - hmin;
        wdif = wmax - wmin;

        rec->h = hmin + roll_fair(hdif);
        rec->w = wmin + roll_fair(wdif);
}


/*
 * rec_place_random -- set (y,x) coords of a rect to random bounded values
 * @rec  : pointer to a rectangle
 * @y_min: minimum y-coordinate
 * @y_max: maximum y-coordinate
 * @x_min: minimum x-coordinate
 * @x_max: maximum x-coordinate
 */
void rec_place_random(struct rec_t *rec, int ymin, int ymax, int xmin, int xmax)
{
        int ydif;
        int xdif;

        ydif = ymax - ymin;
        xdif = xmax - xmin;

        rec->y = ymin + roll_fair((ydif - rec->h));
        rec->x = xmin + roll_fair((xdif - rec->w));
}



/******************************************************************************
 * Generation -- create the rooms and place the tiles in the dungeon 
 ******************************************************************************/
void gen_dungeon(struct map_t *map)
{
        struct rec_t *room;
        int num_rooms;
        int i;
        int j;
        int k;

        /* Generate a random number of rectangles to represent rooms */
        num_rooms = MIN_ROOMS + roll_fair(MAX_ROOMS-MIN_ROOMS);
        room      = malloc(num_rooms * sizeof(struct rec_t));

        /* 
         * Size the rooms; all of the rooms must be sized before any 
         * can be placed, otherwise the call to rec_intersect() will
         * not be able to check against every possible room. 
         */
        for (i=0; i<num_rooms; i++) {
                rec_size_random(&room[i], MIN_ROOM_HEIGHT, MAX_ROOM_HEIGHT,
                                          MIN_ROOM_WIDTH,  MAX_ROOM_WIDTH);
        }

        /* 
         * Place the rooms on the map; for each room, a placement is
         * chosen and then checked against the placement of every
         * other rectangle. If the check fails, a new placement is
         * chosen, and the test starts from the beginning.
         */
        for (i=0; i<num_rooms; i++) {
                place:
                        rec_place_random(&room[i], 0, LINES, 0, COLS);

                        for (j=0; j<num_rooms; j++) {
                                if (i!=j && !rec_intersect(&room[i], &room[j]))
                                        goto place;
                        }
        }

        /* Fill the map's background with cavesolid tiles */
        for (i=0; i<LINES; i++) {
        for (j=0; j<COLS;  j++) {
                place_cavesolid_label(mx_get(map->tile, i, j));
        }
        }

        /* Fill the areas of each room with cavefloor tiles */
        for (k=0; k<num_rooms; k++) {
                for (i=room[k].y; i<room[k].y + room[k].h; i++) {
                for (j=room[k].x; j<room[k].x + room[k].w; j++) {
                        place_cavefloor_label(mx_get(map->tile, i, j));
                }
                }
        }

        free(room); /* Yep */
}



/******************************************************************************
 * New -- Request a new dungeon (PUBLIC) 
 ******************************************************************************/
struct map_t *new_dungeon(void)
{
        struct map_t *dungeon;
        dungeon = new_map(FULLSCREEN);
        dungeon->pmap = simplex_matrix(FULLSCREEN);        

        dungeon->tag = MAP_CAVE;

        gen_dungeon(dungeon);

        return dungeon;
}


