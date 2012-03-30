#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include "boat_model.h"

#include "../lib/textutils.h"
#include "../lib/hash.h"
#include "../pan/test.h"

/*
  ⎛⎝     
  ◥■◤ 
  ⎛⎝  
  ◥◤  
  ⎛⎞  
  ◥◤  
  ⎠⎞  
  ◥◤  
   ⎠⎞ 
  ◥■◤ 
    ⎠ 
   ◥■◤
    ⎠⎞
   ◥■◤
    ⎠⎞
    ◥◤
    ⎛⎞
    ◥◤
    ⎛⎝
    ◥◤
   ⎛⎝    
   ◥■◤
*/

#define BOAT_KIND 'b'

struct rb_tree *BOAT_TREE;
uint32_t keys[10];

static struct boat_t *ACTIVE;



/*
 * Allocate and initialize a new boat, which can be referenced by the key
 * generated from the fasthash() of its name string.
 *
 * @map: The map in which the boat will be set.
 * @type: An enumerated option specifying the type of boat.
 * @name: A string constant specifying the name of the boat.
 *
 * RETURNS: The hashed key value (based on the name string)
 */
uint32_t new_boat(struct map_t *map, int type, const char *name)
{
        #define BOAT_PATH_LEN 13
        #define BOAT_PATH_Y 1
        #define BOAT_PATH_X 0
        #define BOAT_H 2
        #define BOAT_W 3

        struct boat_t *new = malloc(sizeof(struct boat_t)); 

        set_ufo(&new->ufo, BOAT_H, BOAT_W, 0, 0, 
                           map->ufo.obj.h, map->ufo.obj.w, 0, 0);

        init_path(&new->path, BOAT_PATH_Y, BOAT_PATH_X, BOAT_PATH_LEN);

        new->name   = mydup(name);
        new->id     = fasthash(name, strlen(name));
        new->type   = type;
        new->state  = 0;
        WINDOW *win = newwin(BOAT_H, BOAT_W, 0, 0);
        new->pan    = new_panel(win);

        wbkgrnd(win, &OCEAN[0]);

        if (BOAT_TREE == NULL) BOAT_TREE = new_tree(1);

        rb_store(BOAT_TREE, new->id, new);

        return (new->id);
}


////////////////////////////////////////////////////////////////////////////////
struct boat_t *get_boat(const char *name)
{
        return (struct boat_t *)rb_extra(BOAT_TREE, fasthash(name, strlen(name)));
}




int get_sail(int H, int W)
{
        return (stbl[H][W]);
}

int get_mas(int H, int W)
{
        return (mtbl[H][W]);
}
