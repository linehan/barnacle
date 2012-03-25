#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdint.h>
#include "../gfx/gfx.h"
#include "../pan/test.h"

#include "../lib/textutils.h"
#include "../lib/bitwise.h"
#include "../lib/hash.h"
#include "../lib/ufo.h"

#include "boat.h"

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



////////////////////////////////////////////////////////////////////////////////
uint32_t new_boat(struct map_t *map, int type, const char *name)
{
        #define BOAT_H 2
        #define BOAT_W 3

        struct boat_t *new = malloc(sizeof(struct boat_t)); 

        new->ufo = new_ufo(BOAT_H, BOAT_W, 0, 0, map->h, map->w, 0, 0);
        init_path(&new->path, 1, 0, 13);

        new->name = mydup(name);
        new->type = type;
        new->id   = fasthash(name, strlen(name));

        new->state = 0;

        WINDOW *win = newwin(2, 3, 0, 0);
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

uint32_t get_hdg(const char *name)
{
        struct boat_t *boat = get_boat(name);
        
        return (boat->hdg);
}

uint32_t get_helm(const char *name)
{
        struct boat_t *boat = get_boat(name);
        
        return (boat->helm);
}






////////////////////////////////////////////////////////////////////////////////




int get_sail(int H, int W)
{
        return (stbl[H][W]);
}

int get_mas(int H, int W)
{
        return (mtbl[H][W]);
}
