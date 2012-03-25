#pragma once
////////////////////////////////////////////////////////////////////////////////
#include "../geo/path_mvc.h"
#include "../lib/ufo.h"
#include "../gfx/gfx.h"

#include "compass.h"
#include "boat_enum.h"
#include "sailtable.h"


struct boat_t {
        const char *name;
        int type;
        uint32_t id;
        PANEL *pan;
        struct ufo_t *ufo;
        struct path_t path;
        struct boat_gfx *gfx;
        uint32_t state;
        uint32_t hdg;
        uint32_t helm;
};

void boat_control(struct boat_t *boat, int order, int val);

uint32_t new_boat(struct map_t *map, int type, const char *name);
struct boat_t *get_boat(const char *name);
uint32_t get_hdg(const char *name);
uint32_t get_helm(const char *name);

int get_mas(int H, int W);
int get_sail(int H, int W);




enum boat_genera { FUNBOAT, KETCH, CARAVEL };

void move_boat(struct boat_t *boat, int dir);
void boat_path(struct boat_t *boat);
void draw_boat(struct boat_t *boat);


/*
  These macros will map the 16 compass directions 
  to the 4 cardinal directions.
*/
#define NORTHING(x)        ((x<=NE)||(x>=NW))
#define EASTING(x)         ((x>NE)&&(x<SE)) 
#define SOUTHING(x)        ((x>=SE)&&(x<=SW))
#define WESTING(x)         ((x<NW)&&(x>SW))
#define STRICTNORTHING(x)  ((x<NE)||(x>NW))
#define STRICTEASTING(x)   ((x>NE)&&(x<SE))
#define STRICTSOUTHING(x)  ((x>SE)&&(x<SW))
#define STRICTWESTING(x)   ((x<NW)&&(x>SW))
#define NWESTING(x)        ((x==NW))     
#define SWESTING(x)        ((x==SW))    
#define NEASTING(x)        ((x==NE))   
#define SEASTING(x)        ((x==SE)) 


